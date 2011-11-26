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

string query_cust_key = "select CW_CUSTKEY from customer_wh order by CW_CUSTKEY;";
string query_amount_spent = "select sum(SW_TOTALAMOUNT) from sales_wh where SW_CUSTKEY=";
string query_number_products ="select count(distinct sw_productkey) from sales_wh where sw_custkey=";
string query_number_orders ="select count(distinct sw_orderkey) from sales_wh where sw_custkey=";
string query_all = "select sum(SW_TOTALAMOUNT), count(distinct sw_productkey), count(distinct sw_orderkey) from sales_wh where SW_CUSTKEY=";
string query_update_cluster_head = "update customer_wh set cw_cluster1=";
string query_update_cluster_trail = " where cw_custkey=";
string query_all_customers = "select SW_CUSTKEY , sum(SW_TOTALAMOUNT), count(distinct sw_productkey), count(distinct sw_orderkey) from sales_wh group by SW_CUSTKEY;";
long total_changes;
long cluster_centroids[] = {7000, 22000, 37000, 52000, 67000, 82000, 97000, 112000, 127000, 142000};
double centroid_values[10][3];
double customer_values[160000][4];

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

void get_data_for_customer(long custid, double &amount_spent, double &num_products, double & num_orders){
	amount_spent = customer_values[custid-1][0];
	num_products =customer_values[custid-1][1];
	num_orders = customer_values[custid-1][2];
	//printf("%ld %f %f %f\n", custid, amount_spent, num_products, num_orders);
}
void update_to_db(){
	char tempstr[2048];
	PGresult *result;
	printf("Updating in Database\n Please wait..");
	for(long i=0; i<150000; i++){
		string tempQuery;
		int temp_cluster= customer_values[i][3];
		sprintf(tempstr, "%s%d%s%ld;", query_update_cluster_head.c_str(), temp_cluster, query_update_cluster_trail.c_str(), i+1);
		tempQuery = tempstr;
		printf("%s\n", tempQuery.c_str());
		result = pq_query(tempQuery.c_str());
	}

	
}
void update_cluster(long custid, int cluster){
/*	char tempstr[2048];
	PGresult *result;
	string tempQuery;		
	sprintf(tempstr, "%s%d%s%ld;", query_update_cluster_head.c_str(), cluster, query_update_cluster_trail.c_str(), custid);
	tempQuery = tempstr;
	printf("%s\n", tempQuery.c_str());
	result = pq_query(tempQuery.c_str());
*/
	if(customer_values[custid -1][3] != cluster)
		total_changes++;
	customer_values[custid -1][3] = cluster;
	
}
void load_from_db(){
	char tempstr[2048];
	PGresult *result;
	long cust_id;
	result = pq_query(query_all_customers.c_str());
	int num_rows = PQntuples(result);
	long j=0;
	for(long i=0; i<160000; i++){
		customer_values[i][0]=0;
		customer_values[i][1]=0;
		customer_values[i][2]=0;
		customer_values[i][3]=-1;
	}

	for(long i=0; i<num_rows; i++){
		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		cust_id = atol(tempstr);
		
		//printf("%ld %ld\n", i,cust_id);

		sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
		customer_values[cust_id-1][0] = atof(tempstr);	
		//printf("Amount Spent: %f\n", customer_values[cust_id][0]);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 2));
		customer_values[cust_id-1][1] = atof(tempstr);	
		//printf("Number products: %f\n", customer_values[cust_id][1]);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 3));
		customer_values[cust_id-1][2] = atof(tempstr);	
		//printf("Number Orders: %f\n", customer_values[cust_id][2]);

	}
	PQclear(result);
}
void init(){
	int i, j, k;
	load_from_db();
	for(int i=0; i<10; i++){
		printf("Initialising %d\n", i);\
		get_data_for_customer(cluster_centroids[i], centroid_values[i][0], centroid_values[i][1], centroid_values[i][2]);
	}

}

double distance (int centroid_id, double amount, double num_products, double num_orders){
	double x,y,z;
	x = pow((amount - centroid_values[centroid_id][0]) , 2);
	y = pow((num_products - centroid_values[centroid_id][1]) , 2);
	z = pow((num_orders - centroid_values[centroid_id][2]) , 2);
	return sqrt(x+y+z);
}

int find_cluster(double amount, double num_products, double num_orders){
	int i, cluster;
	long min_distance, ret_distance;	
	min_distance = distance(0, amount, num_products, num_orders);
	//printf("0 : %ld\n", min_distance);
	cluster = 0;
	for(i =1; i<10; i++){
		ret_distance  = distance(i, amount, num_products, num_orders);
		//printf("%d : %ld\n", i, ret_distance);
		if(ret_distance < min_distance){
			min_distance = ret_distance;
			cluster = i;
		}
	}
	return cluster;
}

void create_clusters(){
	for(long custkey = 1; custkey <= 150000; custkey++)
	{
		double x,y,z;
		get_data_for_customer(custkey,x,y,z);
		int cluster = find_cluster(x,y,z);
		//printf("Custkey: %ld  Cluster : %d\n", custkey, cluster);
		update_cluster(custkey, cluster);
	}
}

void update_centroids(){
	printf("Updating centroids\n");
	double sum[10][3], count[10];
	
	for(int i=0; i<10; i++)
	{
//		printf("Initializing centroids\n");
		sum[i][0] = sum[i][1]= sum[i][2] = count[i] =0;
	}
//	printf("Summing values for centroids\n");
	for(long i=0; i<150000; i++){
		int clustid = customer_values[i][3];
		sum[clustid][0] += customer_values[i][0];
		sum[clustid][1] += customer_values[i][1];
		sum[clustid][2] += customer_values[i][2];
		count[clustid]++;
	}
//	printf("Averaging centroids\n");
	for(int i=0; i<10; i++)
	{
		centroid_values[i][0] = sum[i][0]/count[i];
		centroid_values[i][1] = sum[i][1]/count[i];
		centroid_values[i][2] = sum[i][2]/count[i];
	}
}

int main (int argc, char **argv)
{

	char *data_safe;
	char tempstr[2048];
	int i, j;
	PGresult *result;
	//printf("here");
	psql = PQconnectdb("hostaddr = '127.0.0.1' port = '' dbname = 'dbproject' user = 'tarun' password = 'tarun123' connect_timeout = '10'");
	/* init connection */
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
			update_to_db();
		}
	}


//	long custkey = 7287;	
//	while(1){
//		cin >> custkey;
//		double x,y,z;
//		get_data_for_customer(custkey,x,y,z);
//		int cluster = find_cluster(x,y,z);
//		printf("Custkey: %ld  Cluster : %d\n", custkey, cluster);
//	}
	PQfinish(psql);
}


