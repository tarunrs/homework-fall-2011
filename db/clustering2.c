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

string query_details[] = {
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_WEEKDAY_FLAG='t' group by s.SW_CUSTKEY;",
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_WEEKDAY_FLAG='f' group by s.SW_CUSTKEY;",
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_HOLIDAY_FLAG='t' group by s.SW_CUSTKEY;",
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_HOLIDAY_FLAG='f' group by s.SW_CUSTKEY;",

"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_QUARTER=1 group by s.SW_CUSTKEY;",
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_QUARTER=2 group by s.SW_CUSTKEY;",
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_QUARTER=3 group by s.SW_CUSTKEY;",
"select s.SW_CUSTKEY , sum(s.SW_TOTALAMOUNT) from sales_wh s, time_wh t where s.SW_TIMEKEY = t.TW_TIMEKEY and t.TW_QUARTER=4 group by s.SW_CUSTKEY;"


};

long total_changes;
long cluster_centroids[] = {7000, 22000, 37000, 52000, 67000, 82000, 97000, 112000, 127000, 142000};
double centroid_values[10][8];
double customer_values[160000][9];

void print_centroid_values(){
	for(int i=0; i<10; i++){
		for(int j=0; j<8; j++)
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

void get_data_for_customer(long custid, double &amount_spent_weekday, double &amount_spent_weekend, double &amount_spent_holiday, double &amount_spent_nonholiday, double &amount_spent_q1, double &amount_spent_q2, double &amount_spent_q3, double &amount_spent_q4){
	amount_spent_weekday = customer_values[custid-1][0];
	amount_spent_weekend = customer_values[custid-1][1];
	amount_spent_holiday = customer_values[custid-1][2];
	amount_spent_nonholiday = customer_values[custid-1][3];
	amount_spent_q1 = customer_values[custid-1][4];
	amount_spent_q2 = customer_values[custid-1][5];
	amount_spent_q3 = customer_values[custid-1][6];
	amount_spent_q4 = customer_values[custid-1][7];

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

	if(customer_values[custid -1][8] != cluster)
		total_changes++;
	customer_values[custid -1][8] = cluster;
	
}
void load_from_db(){
	char tempstr[2048];
	int num_queries =8;
	PGresult *result;
	long cust_id;
	long j=0;
	for(long i=0; i<150000; i++){
		customer_values[i][0]=0;
		customer_values[i][1]=0;
		customer_values[i][2]=0;
		customer_values[i][3]=0;
		customer_values[i][4]=0;
		customer_values[i][5]=0;
		customer_values[i][6]=0;
		customer_values[i][7]=0;
		customer_values[i][8]=-1;
	}
	for(int query_num=0; query_num<num_queries; query_num++ ){

		result = pq_query(query_details[query_num].c_str());
		int num_rows = PQntuples(result);
		printf("Query %d: returned %d rows\n", query_num, num_rows);
		for(long i=0; i<num_rows; i++){
			sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
			cust_id = atol(tempstr);
			sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
			customer_values[cust_id-1][query_num] = atof(tempstr);	
		}
		PQclear(result);
	}
}
void init(){
	int i, j, k;
	load_from_db();
	for(int i=0; i<10; i++){
		printf("Initialising %d\n", i);\
		get_data_for_customer(cluster_centroids[i], centroid_values[i][0], centroid_values[i][1], centroid_values[i][2], centroid_values[i][3], centroid_values[i][4], centroid_values[i][5], centroid_values[i][6], centroid_values[i][7]);
	}

}

double distance (int centroid_id, double &amount_spent_weekday, double &amount_spent_weekend, double &amount_spent_holiday, double &amount_spent_nonholiday, double &amount_spent_q1, double &amount_spent_q2, double &amount_spent_q3, double &amount_spent_q4){
	double a,b,c,d,e,f,g,h;
	a = pow((amount_spent_weekday - centroid_values[centroid_id][0]) , 2);
	b = pow((amount_spent_weekend - centroid_values[centroid_id][1]) , 2);
	c = pow((amount_spent_holiday - centroid_values[centroid_id][2]) , 2);
	d = pow((amount_spent_nonholiday - centroid_values[centroid_id][3]) , 2);
	e = pow((amount_spent_q1 - centroid_values[centroid_id][4]) , 2);
	f = pow((amount_spent_q2 - centroid_values[centroid_id][5]) , 2);
	g = pow((amount_spent_q3 - centroid_values[centroid_id][6]) , 2);
	h = pow((amount_spent_q4 - centroid_values[centroid_id][7]) , 2);

	return sqrt(a+b+c+d+e+f+g+h);
}

int find_cluster(double amount_spent_weekday, double amount_spent_weekend, double amount_spent_holiday, double amount_spent_nonholiday, double amount_spent_q1, double amount_spent_q2, double amount_spent_q3, double amount_spent_q4){
	int i, cluster;
	long min_distance, ret_distance;	
	min_distance = distance(0, amount_spent_weekday, amount_spent_weekend, amount_spent_holiday, amount_spent_nonholiday, amount_spent_q1, amount_spent_q2, amount_spent_q3, amount_spent_q4);
	cluster = 0;
	for(i =1; i<10; i++){
		ret_distance  = distance(i, amount_spent_weekday, amount_spent_weekend, amount_spent_holiday, amount_spent_nonholiday, amount_spent_q1, amount_spent_q2, amount_spent_q3, amount_spent_q4);
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
		double a,b,c,d,e,f,g,h;
		get_data_for_customer(custkey,a,b,c,d,e,f,g,h);
		int cluster = find_cluster(a,b,c,d,e,f,g,h);
//		printf("Custkey: %ld  Cluster : %d\n", custkey, cluster);
		update_cluster(custkey, cluster);
	}
}

void update_centroids(){
	printf("Updating centroids\n");
	double sum[10][8], count[10];
	
	for(int i=0; i<10; i++)
	{
//		printf("Initializing centroids\n");
		sum[i][0] = sum[i][1]= sum[i][2] = sum[i][3] = sum[i][4]= sum[i][5] =sum[i][6] = sum[i][7] = count[i] =0;
	}
//	printf("Summing values for centroids\n");
	for(long i=0; i<150000; i++){
		int clustid = customer_values[i][8];
		sum[clustid][0] += customer_values[i][0];
		sum[clustid][1] += customer_values[i][1];
		sum[clustid][2] += customer_values[i][2];
		sum[clustid][3] += customer_values[i][3];
		sum[clustid][4] += customer_values[i][4];
		sum[clustid][5] += customer_values[i][5];
		sum[clustid][6] += customer_values[i][6];
		sum[clustid][7] += customer_values[i][7];
		count[clustid]++;
	}
//	printf("Averaging centroids\n");
	for(int i=0; i<10; i++)
	{
		centroid_values[i][0] = sum[i][0]/count[i];
		centroid_values[i][1] = sum[i][1]/count[i];
		centroid_values[i][2] = sum[i][2]/count[i];
		centroid_values[i][3] = sum[i][3]/count[i];
		centroid_values[i][4] = sum[i][4]/count[i];
		centroid_values[i][5] = sum[i][5]/count[i];
		centroid_values[i][6] = sum[i][6]/count[i];
		centroid_values[i][7] = sum[i][7]/count[i];

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
		print_centroid_values();
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


