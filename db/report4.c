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
string results[27][26];

string query_nations = "select n.N_NAME from nation n order by n.N_NAME ;";

string query_head = "select c.CW_NATION , count(DISTINCT s.SW_CUSTKEY) from  time_wh t, sales_wh s, customer_wh c where t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=1995 and t.TW_MONTH=";
string query_trail = " AND c.CW_CUSTKEY= s.SW_CUSTKEY group by c.CW_NATION order by c.CW_NATION;";

string query_head_month = "select count(DISTINCT s.SW_CUSTKEY) from  time_wh t, sales_wh s, customer_wh c where t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=1995 and t.TW_MONTH=";
string query_trail_month = " AND c.CW_CUSTKEY= s.SW_CUSTKEY ;";

string query_total_per_nation = "select c.CW_NATION , count(DISTINCT s.SW_CUSTKEY) from  time_wh t, sales_wh s, customer_wh c where t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=1995  AND c.CW_CUSTKEY= s.SW_CUSTKEY group by c.CW_NATION order by c.CW_NATION;";

string query_total = "select count(DISTINCT s.SW_CUSTKEY) from  time_wh t, sales_wh s, customer_wh c where t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=1995  AND c.CW_CUSTKEY= s.SW_CUSTKEY;";

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

// populate nations

	result = pq_query(query_nations.c_str());
	if (!result || !(j = PQntuples(result)))
	{
		fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
		PQfinish(psql);
		exit(0);
	}
	for (i = 0; i < j; i++)
	{
//		printf("%d\n", i);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		results[i][0] = tempstr;	
	}
	//int index =1;
	for(int k=1; k<13; k++){
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
				sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
				results[i][k] = tempstr;	
			}
                //printf("Done:%d\n", k);
		PQclear(result);
	}
// populate totals per nation
	result = pq_query(query_total_per_nation.c_str());
	if (!result || !(j = PQntuples(result)))
	{
		fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
		PQfinish(psql);
		exit(0);
	}
	for (i = 0; i < j; i++)
	{
		sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
		results[i][13] = tempstr;	
	}

// totals per month
	results[25][0] = "Total     ";
	for(int k=1; k<13; k++){
                //printf("Doing:%d\n", k);
		string tempQuery;
		sprintf(tempstr, "%s%d%s", query_head_month.c_str(), k, query_trail_month.c_str());
		tempQuery = tempstr;
		result = pq_query(tempQuery.c_str());
		if (!result || !(j = PQntuples(result)))
		{
			fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
			PQfinish(psql);
			exit(0);
		}
		sprintf(tempstr, "%s", PQgetvalue(result, 0, 0));
                //printf("Storing:%d\n", k);
		results[25][k] = tempstr;	
                //printf("Done:%d\n", k);
		PQclear(result);
	}
	
// complete total
	result = pq_query(query_total.c_str());
	if (!result || !(j = PQntuples(result)))
	{
		fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
		PQfinish(psql);
		exit(0);
	}
	sprintf(tempstr, "%s", PQgetvalue(result, 0, 0));
	//printf("Storing:%d\n", k);
	results[25][13] = tempstr;	

	for(i=0; i<26; i++)
		{
		for(int k=0;k<14; k++)
			printf("%s\t", results[i][k].c_str());
		printf("\n");
	}
	PQfinish(psql);
}


