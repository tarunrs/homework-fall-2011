#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include<iostream>
#include <vector>
#include<math.h>
#include "libpq-fe.h"

using namespace std;
PGconn *psql;

string query_update_cluster_head = "update customer_wh set cw_cluster1=";
string query_update_cluster_trail = " where cw_custkey=";
string query_all_product = "select avg(p.pw_retailprice), avg(p.pw_avgcost), sum(s.sw_totalamount)/((max(t.tw_year)-min(t.tw_year))*12) ,sum(s.sw_quantity)/((max(t.tw_year)-min(t.tw_year)) *12) from sales_wh s, time_wh t, product_wh p where pw_productkey=sw_productkey and sw_timekey =t.tw_timekey group by p.pw_brand order by p.pw_brand;";


long total_changes;
long cluster_centroids[] = {1,2,3,4,5,6,7,8,9,10};
double centroid_values[10][4];
double product_values[25][5];

void print_centroid_values(){
	for(int i=0; i<10; i++){
		for(int j=0; j<4; j++)
			printf("%f\t", centroid_values[i][j]);
		printf("\n");
	}
}

PGresult *pq_query(const char *format)
{
	PGresult *result;
	result = PQexec(psql, format);
	return(result);
}

char * pq_escape (char *input)
{
	int len;
	char *output;
	len = strlen(input);
	if (len < 1)
		return(NULL);
	output = (char *)malloc((len * 2) + 1);
	if (!output)
		return(NULL);
	PQescapeString(output, input, len);
	return(output);
}

void get_data_for_product(long prodid, double &avg_price, double &avg_cost, double &monthly_sales_units, double &monthly_sales){
	avg_price = product_values[prodid-1][0];
	avg_cost =product_values[prodid-1][1];
	monthly_sales_units = product_values[prodid-1][2];
	monthly_sales = product_values[prodid-1][3];
	//printf("%ld %f %f %f\n", custid, amount_spent, num_products, num_orders);
}
void update_to_db(){
	char tempstr[2048];
	PGresult *result;
	printf("Updating in Database\n Please wait..");
	for(long i=0; i<25; i++){
		string tempQuery;
		int temp_cluster= product_values[i][3];
		sprintf(tempstr, "%s%d%s%ld;", query_update_cluster_head.c_str(), temp_cluster, query_update_cluster_trail.c_str(), i+1);
		tempQuery = tempstr;
		printf("%s\n", tempQuery.c_str());
		result = pq_query(tempQuery.c_str());
	}

	
}
void update_cluster(long prodid, int cluster){
	if(product_values[prodid -1][4] != cluster)
		total_changes++;
	product_values[prodid -1][4] = cluster;
	
}
void load_from_db(){
	char tempstr[2048];
	PGresult *result;
	long product_id;
	result = pq_query(query_all_product.c_str());
	int num_rows = PQntuples(result);
	long j=0;
	for(long i=0; i<25; i++){
		product_values[i][0]=0;
		product_values[i][1]=0;
		product_values[i][2]=0;
		product_values[i][3]=0;
		product_values[i][4]=-1;
	}

	for(long i=0; i<num_rows; i++){
	//	sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
	//	product_id = atol(tempstr);
		
		//printf("%ld %ld\n", i,cust_id);

		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		product_values[i][0] = atof(tempstr);	
		//printf("Amount Spent: %f\n", customer_values[cust_id][0]);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
		product_values[i][1] = atof(tempstr);	
		//printf("Number products: %f\n", customer_values[cust_id][1]);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 2));
		product_values[i][2] = atof(tempstr);	
		//printf("Number Orders: %f\n", customer_values[cust_id][2]);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 3));
		product_values[i][3] = atof(tempstr);	

	}
	PQclear(result);
}
void init(){
	int i, j, k;
	load_from_db();
	for(int i=0; i<10; i++){
		printf("Initialising %d\n", i);\
		get_data_for_product(cluster_centroids[i], centroid_values[i][0], centroid_values[i][1], centroid_values[i][2], centroid_values[i][3]);
	}

}

double distance (int centroid_id, double avg_price, double avg_cost, double monthly_sales_units, double monthly_sales){
	double x,y,z,a;
	x = pow((avg_price - centroid_values[centroid_id][0]) , 2);
	y = pow((avg_cost - centroid_values[centroid_id][1]) , 2);
	z = pow((monthly_sales_units - centroid_values[centroid_id][2]) , 2);
	a = pow((monthly_sales - centroid_values[centroid_id][3]) , 2);
	return sqrt(x+y+z+a);
}

int find_cluster(double avg_price, double avg_cost, double monthly_sales_units, double monthly_sales){
	int i, cluster;
	long min_distance, ret_distance;	
	min_distance = distance(0, avg_price, avg_cost, monthly_sales_units, monthly_sales);
	//printf("0 : %ld\n", min_distance);
	cluster = 0;
	for(i =1; i<10; i++){
		ret_distance  = distance(i, avg_price, avg_cost, monthly_sales_units, monthly_sales);
		//printf("%d : %ld\n", i, ret_distance);
		if(ret_distance < min_distance){
			min_distance = ret_distance;
			cluster = i;
		}
	}
	return cluster;
}

void create_clusters(){
	for(long prodkey = 1; prodkey <= 25; prodkey++)
	{
		double x,y,z,a;
		get_data_for_product(prodkey,x,y,z,a);
		int cluster = find_cluster(x,y,z,a);
		//printf("Custkey: %ld  Cluster : %d\n", custkey, cluster);
		update_cluster(prodkey, cluster);
	}
}

void update_centroids(){
	printf("Updating centroids\n");
	double sum[10][4], count[10];
	
	for(int i=0; i<10; i++)
	{
//		printf("Initializing centroids\n");
		sum[i][0] = sum[i][1]= sum[i][2] = sum[i][3] = count[i] =0;
	}
//	printf("Summing values for centroids\n");
	for(long i=0; i<25; i++){
		int prodkey = product_values[i][4];
		sum[prodkey][0] += product_values[i][0];
		sum[prodkey][1] += product_values[i][1];
		sum[prodkey][2] += product_values[i][2];
		sum[prodkey][3] += product_values[i][3];
		count[prodkey]++;
	}
//	printf("Averaging centroids\n");
	for(int i=0; i<10; i++)
	{
		centroid_values[i][0] = sum[i][0]/count[i];
		centroid_values[i][1] = sum[i][1]/count[i];
		centroid_values[i][2] = sum[i][2]/count[i];
		centroid_values[i][3] = sum[i][3]/count[i];
	}
}

int main (int argc, char **argv)
{

	char *data_safe;
	char tempstr[2048];
	int i, j;
	PGresult *result;
	psql = PQconnectdb("hostaddr = '127.0.0.1' port = '' dbname = 'dbproject' user = 'tarun' password = 'tarun123' connect_timeout = '10'");
	if (!psql)
	{
		fprintf(stderr, "libpq error: PQconnectdb returned NULL.\n\n");
		exit(0);
	}
	if (PQstatus(psql) != CONNECTION_OK)
	{
		fprintf(stderr, "libpq error: PQstatus(psql) != CONNECTION_OK\n\n");
		exit(0);
	}
	total_changes =0;
	init();
	int flag =1;
	while(flag){
		total_changes = 0;
		create_clusters();
		printf("Total Changes : %ld\n", total_changes);
		update_centroids();
		printf("Updated Centroids\n");
		if(total_changes == 0)
		{
			flag =0;
			print_centroid_values();
			//update_to_db();
		}
	}

	PQfinish(psql);
}


