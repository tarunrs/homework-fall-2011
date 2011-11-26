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
string results[26][26];

string query_nations = "select n.N_NAME from nation n order by n.N_NAME ;";

string query_head = "select c.CW_NATION , count(DISTINCT s.SW_PRODUCTKEY) from time_wh t, sales_wh s, customer_wh c where t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=";
string query_trail = " AND c.CW_CUSTKEY= s.SW_CUSTKEY group by c.CW_NATION order by c.CW_NATION;";

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
	int index =1;
	for(int k=0; k<4; k++){
		string tempQuery;
		sprintf(tempstr, "%s%d%s", query_head.c_str(), 1992+k, query_trail.c_str());
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
			if(k!=0)
				{
				sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
				results[i][index] = tempstr;	
				double prev_value;
				double this_value;
				prev_value = atof(results[i][index-1].c_str());
				this_value = atof(tempstr);
				double per = ((this_value - prev_value)/prev_value)*100;
				sprintf(tempstr, "%f", per);
				results[i][index+3] = tempstr;	
				}
			else{
				sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
				results[i][index] = tempstr;	
				}
			}
		index++;
		printf("index end:%d\n", index);
		PQclear(result);
	}

	for(i=0; i<26; i++)
		{
		for(int k=0;k<8; k++)
			printf("%s\t", results[i][k].c_str());
		printf("\n");
	}
	PQfinish(psql);
}


