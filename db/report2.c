#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include<iostream>
#include <vector>
#include "libpq-fe.h"
using namespace std;
PGconn *psql;
string results[26][20];
string total;
string query_head = "select sum(s.sw_quantity) from product_wh p, sales_wh s, time_wh t, customer_wh c where s.SW_TIMEKEY = t.TW_TIMEKEY and p.PW_PRODUCTKEY = s.SW_PRODUCTKEY and t.TW_YEAR=1994 and t.TW_MONTH=";
string query_trail = " and c.CW_CUSTKEY = s.SW_CUSTKEY and c.CW_NATION='RUSSIA' group by p.PW_BRAND order by PW_BRAND;";

string query_total_by_brand = "select sum(s.sw_quantity) from product_wh p, sales_wh s, time_wh t, customer_wh c where s.SW_TIMEKEY = t.TW_TIMEKEY and p.PW_PRODUCTKEY = s.SW_PRODUCTKEY and t.TW_YEAR=1994  and t.TW_MONTH>4 and c.CW_CUSTKEY = s.SW_CUSTKEY and c.CW_NATION='RUSSIA' group by p.PW_BRAND order by PW_BRAND;";

string query_total = "select sum(s.sw_quantity) from product_wh p, sales_wh s, time_wh t, customer_wh c where s.SW_TIMEKEY = t.TW_TIMEKEY and p.PW_PRODUCTKEY = s.SW_PRODUCTKEY and t.TW_YEAR=1994  and t.TW_MONTH>4 and c.CW_CUSTKEY = s.SW_CUSTKEY and c.CW_NATION='RUSSIA';";

string query_brands = "select p.PW_BRAND from product_wh p, sales_wh s, time_wh t, customer_wh c where s.SW_TIMEKEY = t.TW_TIMEKEY and p.PW_PRODUCTKEY = s.SW_PRODUCTKEY and t.TW_YEAR=1994 and t.TW_MONTH>4 and c.CW_CUSTKEY = s.SW_CUSTKEY and c.CW_NATION='RUSSIA' group by p.PW_BRAND order by PW_BRAND;";

string query_total_head = "select sum(s.sw_quantity) from product_wh p, sales_wh s, time_wh t, customer_wh c where s.SW_TIMEKEY = t.TW_TIMEKEY and p.PW_PRODUCTKEY = s.SW_PRODUCTKEY and t.TW_YEAR=1994 and t.TW_MONTH=";
string query_total_trail = " and c.CW_CUSTKEY = s.SW_CUSTKEY and c.CW_NATION='RUSSIA';";


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

	//result = pq_query("SELECT * from nations;");
// populate brands

	result = pq_query(query_brands.c_str());
	if (!result || !(j = PQntuples(result)))
	{
		fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
		PQfinish(psql);
		exit(0);
	}
	for (i = 0; i < j; i++)
	{
		printf("%d\n", i);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		results[i][0] = tempstr;	
	}

	for(int k=5; k<=12; k++){
		printf("%d\n", k);
		string tempQuery;
		sprintf(tempstr, "%s%d%s", query_head.c_str(), k, query_trail.c_str());
		tempQuery = tempstr;
		result = pq_query(tempQuery.c_str());
		if (!result || !(j = PQntuples(result)))
		{
			fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
			PQfinish(psql);
			exit(0);
		}
		for (i = 0; i < j; i++)
			{
			sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
			results[i][k-4] = tempstr;	
			}
		
		printf("%d\n", k);
		PQclear(result);
	}
// find the total

	result = pq_query(query_total.c_str());
	sprintf(tempstr, "%s", PQgetvalue(result, 0, 0));
	total = tempstr;
	PQclear(result);

// total by brand and percentage of total
	result = pq_query(query_total_by_brand.c_str());

	for(i=0; i<25; i++)
	{
		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		results[i][9] = tempstr;
		double reg_tot;
		double all_tot;
		all_tot = atof(total.c_str());
		reg_tot = atof(tempstr);
		double per = (reg_tot/all_tot)*100;
		sprintf(tempstr, "%f", per);
		results[i][10] = tempstr;	
	}
		
// total by month

	for(int k=5; k<=12; k++){
		printf("%d\n", k);
		string tempQuery;
		sprintf(tempstr, "%s%d%s", query_total_head.c_str(), k, query_total_trail.c_str());
		tempQuery = tempstr;
		result = pq_query(tempQuery.c_str());
		if (!result || !(j = PQntuples(result)))
		{
			fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
			PQfinish(psql);
			exit(0);
		}
		sprintf(tempstr, "%s", PQgetvalue(result, 0, 0));
		results[25][k-4] = tempstr;	
		
		printf("2 %d\n", k);
		PQclear(result);
	}
	results[25][0] = "Total";
	results[25][9] = total;
	results[25][10] ="100%";

	for(i=0; i<26; i++)
		{
		for(int k=0;k<11; k++)
			printf("%s\t\t", results[i][k].c_str());
		printf("\n");
	}
	PQfinish(psql);
}


