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
string results[10][10];
string total;

string queries[] = {
	"select c.CW_REGION from customer_wh c group by c.CW_REGION order by c.CW_REGION;",

	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_HOLIDAY_FLAG = 't'and t.TW_YEAR=1996  group by c.CW_REGION order by c.CW_REGION;",
	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_HOLIDAY_FLAG = 'f' and t.TW_YEAR=1996 group by c.CW_REGION order by c.CW_REGION;",

	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_WEEKDAY_FLAG = 't' and t.TW_YEAR=1996  group by c.CW_REGION order by c.CW_REGION;",
	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_WEEKDAY_FLAG = 'f' and t.TW_YEAR=1996  group by c.CW_REGION order by c.CW_REGION;",

	"select sum(s.SW_TOTALAMOUNT) from  time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=1996 ;",

	"select sum(s.SW_TOTALAMOUNT) from  time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_YEAR=1996 group by c.CW_REGION order by c.CW_REGION;",

	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_HOLIDAY_FLAG = 't' and t.TW_YEAR=1996 ;",
	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_HOLIDAY_FLAG = 'f' and t.TW_YEAR=1996 ;",

	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_WEEKDAY_FLAG = 't' and t.TW_YEAR=1996 ;",
	"select sum(s.SW_TOTALAMOUNT) from time_wh t, customer_wh c, sales_wh s where s.SW_CUSTKEY = c.CW_CUSTKEY and t.TW_TIMEKEY = s.SW_TIMEKEY and t.TW_WEEKDAY_FLAG = 'f' and t.TW_YEAR=1996 ;"

};

struct region_data{
	vector<string> total_sales;
} regions[6];


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

void update_result(){};

int main (int argc, char **argv)
{

	char *data_safe;
	char tempstr[200];
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

	for(int k=0; k<7; k++){
		result = pq_query(queries[k].c_str());
		if (!result || !(j = PQntuples(result)))
		{
			fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
			PQfinish(psql);
			exit(0);
		}
		if(k!=5)
			for (i = 0; i < j; i++)
			{
				if(k <5){
					sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
					results[k][i] = tempstr;
					}
				else{

					double reg_tot;
					double all_tot;
					all_tot = atof(total.c_str());
					sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
					results[k-1][i] = tempstr;
					reg_tot = atof(tempstr);
					double per = (reg_tot/all_tot)*100;
					sprintf(tempstr, "%f", per);
					results[k][i] = tempstr;
				}
			}
		else{
			sprintf(tempstr, "%s", PQgetvalue(result, 0, 0));
			total = tempstr;
		}
		printf("%d\n", k);
		PQclear(result);
	}
	for(i=7; i<11; i++){
		//cout<< i;
		result = pq_query(queries[i].c_str());
		sprintf(tempstr, "%s", PQgetvalue(result, 0, 0));
		results[i-6][5] = tempstr;					
		PQclear(result);
	}
	results[0][5] = "Total";
	results[6][5] = results[5][5] = total;
	for(i=0; i<6; i++)
		{
		for(int k=0;k<7; k++)
			printf("%s\t", results[k][i].c_str());
		printf("\n");
	}
	PQfinish(psql);
}


