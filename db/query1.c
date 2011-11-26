#include<stdio.h>
#include<iostream>
#include<string.h>
#include<string>
#include<math.h>
#include <stdlib.h>
#include <algorithm>
#include <functional> 
#include <locale>
#include "libpq-fe.h"

using namespace std;
PGconn *psql;

char bm_region[5][751500];
char bml_region[5][751500];
char bm_nation[25][751500];
char bm_year[10][751500];
char bml_nation[25][751500];
char bml_year[10][751500];
string regions[] = {"AFRICA", "AMERICA", "ASIA", "EUROPE", "MIDDLE EAST"};
string nations[] = {"ALGERIA", "ARGENTINA", "BRAZIL", "CANADA", "EGYPT", "ETHIOPIA", "FRANCE", "GERMANY", "INDIA", "INDONESIA", "IRAN", "IRAQ", "JAPAN", "JORDAN", "KENYA", "MOROCCO", "MOZAMBIQUE", "PERU", "CHINA", "ROMANIA", "SAUDI ARABIA", "VIETNAM", "RUSSIA", "UNITED KINGDOM", "UNITED STATES"};
int years[] ={1992, 1993,1994, 1995,1996, 1997,1998, 1999, 2000, 2001};
char num[2];

string query = "select s.sw_saleskey, c.cw_region, c.cw_nation, t.tw_year from sales_wh s, time_wh t, customer_wh c where s.SW_TIMEKEY = t.TW_TIMEKEY and s.sw_custkey = c.cw_custkey ;";

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
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

int region_key(string inp){
	for(int i=0; i<5; i++){
		if(regions[i] == trim(inp)) return i;
	}
	printf("%s %d\t", inp.c_str(), inp.length());
	printf("Region Error!\n");
}


int nation_key(string inp){
	for(int i=0; i<25; i++){
		if(nations[i] == trim(inp)) return i;
	}
	printf("%s %d\t", inp.c_str(), inp.length());
	printf("Nation Error!\n");
}

int year_key(int inp){
	for(int i=0; i<10; i++){
		if(years[i] == inp) return i;
	}
	printf("%d\t", inp);
	printf("Year Error!\n");
}

void dump_region_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "region";
	FILE *fp;
	fp = fopen(fname.c_str(), "w");
	if(fp){
		printf("writing file: %d\n", sizeof(bm_region));
		fwrite (bm_region , 1 , sizeof(bm_region) , fp );
	}
	fclose(fp);
}

void dump_nation_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "nation";
	FILE *fp;
	fp = fopen(fname.c_str(), "w");
	if(fp){
		printf("writing file: %d\n", sizeof(bm_nation));
		fwrite (bm_nation , 1 , sizeof(bm_nation) , fp );
	}
	fclose(fp);
}

void dump_year_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "year";
	FILE *fp;
	fp = fopen(fname.c_str(), "w");
	if(fp){
		printf("writing file: %d\n", sizeof(bm_year));
		fwrite (bm_year , 1 , sizeof(bm_year) , fp );
	}
	fclose(fp);
}

void load_region_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "region";
	FILE *fp;
	fp = fopen(fname.c_str(), "r");
	if(fp){
		printf("reading file: %d\n", sizeof(bml_region));
		int result = fread (bml_region , 1 , sizeof(bml_region) , fp );
		printf("result %d\n", result);
	}
	fclose(fp);
}

void load_nation_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "nation";
	FILE *fp;
	fp = fopen(fname.c_str(), "r");
	if(fp){
		printf("reading file: %d\n", sizeof(bml_nation));
		int result = fread (bml_nation , 1 , sizeof(bml_nation) , fp );
		printf("result %d\n", result);
	}
	fclose(fp);
}

void load_year_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "year";
	FILE *fp;
	fp = fopen(fname.c_str(), "r");
	if(fp){
		printf("reading file: %d\n", sizeof(bml_year));
		int result = fread (bml_year , 1 , sizeof(bml_year) , fp );
		printf("result %d\n", result);
	}
	fclose(fp);
}
bool verify_bitmaps(){
	for(int i=0;i<5;i++)
		for(int j=0;j<751500;j++)
			if(bm_region[i][j]!=bml_region[i][j]) return false;
	return true;
}

void init(){
	for(int i=0;i<5;i++)
		for(int j=0;j<751500;j++)
			bm_region[i][j] = 0;
	for(int i=0;i<25;i++)
		for(int j=0;j<751500;j++)
			bm_nation[i][j] = 0;
	for(int i=0;i<10;i++)
		for(int j=0;j<751500;j++)
			bm_year[i][j] = 0;
}	

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

bool test(int pos){
	return (num[pos/8] & (int) pow(2, 7-pos%8)) ;
}

void set(int pos){
	num[pos/8] = num[pos/8] | (int)pow(2,7 - (pos%8));
}

void set_region_bitmap(int regionid, long pos){
	bm_region[regionid][pos/8] = bm_region[regionid][pos/8] | (int)pow(2,7 - (pos%8));
}

void set_nation_bitmap(int nationid, long pos){
	bm_nation[nationid][pos/8] = bm_nation[nationid][pos/8] | (int)pow(2,7 - (pos%8));
}

void set_year_bitmap(int yearid, long pos){
	bm_year[yearid][pos/8] = bm_year[yearid][pos/8] | (int)pow(2,7 - (pos%8));
}

bool test_region_bitmap(int regionid, int pos){
	return (bml_region[regionid][pos/8] & (int) pow(2, 7-pos%8)) ;
}

bool test_nation_bitmap(int nationid, int pos){
	return (bml_nation[nationid][pos/8] & (int) pow(2, 7-pos%8)) ;
}

bool test_year_bitmap(int yearid, int pos){
	return (bml_year[yearid][pos/8] & (int) pow(2, 7-pos%8)) ;
}

void create_bitmaps(){
	char tempstr[2048];
	PGresult *result;
	result = pq_query(query.c_str());
	int num_rows = PQntuples(result);
	for(long i=0; i< num_rows; i++){
		if(i%1000 == 0) printf("Done : %ld\n", i);
		int regionid, nationid, yearid;
		long salesid;
		int year;
		string region, nation;
		sprintf(tempstr, "%s", PQgetvalue(result, i, 0));
		salesid = atol(tempstr);

		sprintf(tempstr, "%s", PQgetvalue(result, i, 1));
		region = tempstr;
		regionid = region_key(region);

		sprintf(tempstr, "%s", PQgetvalue(result, i, 2));
		nation = tempstr;
		nationid = nation_key(nation);

		sprintf(tempstr, "%s", PQgetvalue(result, i, 3));
		year = atoi(tempstr);
		yearid = year_key(year);

		set_region_bitmap(regionid, salesid);
		set_nation_bitmap(nationid, salesid);
		set_year_bitmap(yearid, salesid);
	}
}

int bitcount_in_byte (unsigned char n) {
   int count = 0;
   while (n) {
      count += n & 0x1u;
      n >>= 1;
   }
   return count;
}

int bitcount(char * bitmap_array){
	int num = (6001204/8) +1;
	long count=0;
	for(int i=0; i< num; i++){
		count+= bitcount_in_byte(bitmap_array[i]);
	}
	return count;
}

int bitcount_test(char * bitmap_array){
	int num =10;
	long count=0;
	for(int i=0; i< num; i++){
		if(i %1000 == 0) printf("%d Done\n", i);
		count+= bitcount_in_byte(bitmap_array[i]);
	}
	return count;
}

void and_bitmap(char * dest_bitmap_array, char * first_bitmap_array, char * second_bitmap_array){
	int num = (6001204/8) +1;
	long count=0;
	for(int i=0; i< num; i++){
		dest_bitmap_array[i] = first_bitmap_array[i] & second_bitmap_array[i];
	}
}

void or_bitmap(char * dest_bitmap_array, char * first_bitmap_array, char * second_bitmap_array){
	int num = (6001204/8) +1;
	long count=0;
	for(int i=0; i< num; i++){
		dest_bitmap_array[i] = first_bitmap_array[i] | second_bitmap_array[i];
	}
}

void copy_bitmap(char * dest_bitmap_array, char * source_bitmap_array){
	int num = (6001204/8) +1;
	long count=0;
	for(int i=0; i< num; i++){
		dest_bitmap_array[i] = source_bitmap_array[i];
	}
}

int main(){

	char *data_safe;
	char tempstr[2048];
	int i, j;
	PGresult *result;
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
//
//	init();
//	create_bitmaps();
//	dump_region_bit_map();
//	dump_year_bit_map();
//	dump_nation_bit_map();
	int num_bytes = 750151;
	char result_bitmap[750151];
	for(int i=0; i <750151; i++){
		result_bitmap[i]=0;
	}

	load_region_bit_map();
	load_year_bit_map();
	load_nation_bit_map();

	for(int i=0; i<5; i++){
		//copy_bitmap(result_bitmap, bml_region[i]);
		and_bitmap(result_bitmap, bml_region[i], bml_year[year_key(1995)]);
		printf("%s\t\t%d\n", regions[i].c_str(), bitcount(result_bitmap));
	}
//	printf("%d\n", bitcount(bml_region[0]));
	PQfinish(psql);
	return 0;
}
