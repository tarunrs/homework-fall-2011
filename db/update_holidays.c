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
string query_holidays = "select month, day_of_week, nth from floating_holidays;";

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
	PGresult *result1;
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
	result = pq_query(query_holidays.c_str());
	if (!result || !(j = PQntuples(result)))
	{
		fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
		PQfinish(psql);
		exit(0);
	}
	for (i = 0; i < j; i++)
	{
		int month, nth;
		string day_of_week;
		// for each holiday update time_wh
		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		month = atoi(tempstr);
		sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
		day_of_week = tempstr;
		sprintf(tempstr, "%s", PQgetvalue(result, i, 2));
		nth = atoi(tempstr);
		if(nth > 0){
		// nth is positive
			int ending_day_of_month = nth * 7;
			int starting_day_of_month = ending_day_of_month - 6;
			string query_update;
			sprintf(tempstr,"%s%d%s%s%s%d%s%d;","update time_wh set tw_holiday_flag = 't' where tw_month =",month," and tw_day_of_week ='", day_of_week.c_str(),"' and ", starting_day_of_month," <= tw_day_in_number and tw_day_in_number <= ", ending_day_of_month);
			query_update =tempstr;
			printf("%s\n", query_update.c_str());
//			result1 = pq_query(query_update.c_str());
		}
		else{
		//nth is negative
			int ii,jj,k = 0;
			string query_mem;
			sprintf(tempstr, "%s%d%s%s%s","select tw_day_in_number from time_wh where tw_month = ", month, " and tw_day_of_week = '", day_of_week.c_str(), "' order by tw_day_in_number desc;");
			query_mem = tempstr;
			result1 = pq_query(query_mem.c_str());
			if (!result1 || !(jj = PQntuples(result1)))
			{
				fprintf(stderr, "libpq error: no rows returned or bad result set\n\n");
				PQfinish(psql);
				exit(0);
			}
			int day_num;

			for (ii = 0; ii < jj; ii++)
			{
				sprintf(tempstr, "%s", PQgetvalue(result, ii, 0));
				day_num = atoi(tempstr);
				k = k - 1;
				if (k == nth){
					sprintf(tempstr, "%s%d%s%d;", "update time_wh set tw_holiday_flag = 't' where tw_month = ",month," and tw_day_in_number = ", day_num);
					printf("%s\n", tempstr);
					break;

				} 
			}


		}
	}
	
	PQclear(result);
	PQfinish(psql);
}


