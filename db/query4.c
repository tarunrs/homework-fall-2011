#include<stdio.h>
#include<iostream>
#include<string.h>
#include<string>
#include<math.h>
#include <stdlib.h>
#include <algorithm>
#include <functional> 
#include <locale>
#include <sys/time.h>
#include "libpq-fe.h"

using namespace std;

char bm_region[5][751500];
char bml_region[5][751500];
char bm_nation[25][751500];
char bm_year[10][751500];
char bml_nation[25][751500];
char bml_year[10][751500];
string regions[] = {"AFRICA", "AMERICA", "ASIA", "EUROPE", "MIDDLE EAST"};
string nations[] = {"ALGERIA", "ARGENTINA", "BRAZIL", "CANADA", "EGYPT", "ETHIOPIA", "FRANCE", "GERMANY", "INDIA", "INDONESIA", "IRAN", "IRAQ", "JAPAN", "JORDAN", "KENYA", "MOROCCO", "MOZAMBIQUE", "PERU", "CHINA", "ROMANIA", "SAUDI ARABIA", "VIETNAM", "RUSSIA", "UNITED KINGDOM", "UNITED STATES"};
int years[] ={1992, 1993,1994, 1995,1996, 1997,1998, 1999, 2000, 2001};

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
double rtclock()
{
  struct timezone Tzp;
  struct timeval Tp;
  int stat;
  stat = gettimeofday (&Tp, &Tzp);
  if (stat != 0) printf("Error return from gettimeofday: %d",stat);
  return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}
int main(){

	int num_bytes = 750151;
	char result_bitmap[750151];
	for(int i=0; i <750151; i++){
		result_bitmap[i]=0;
	}

	load_region_bit_map();
	load_year_bit_map();
	load_nation_bit_map();
        double clkbegin, clkend, t;

        clkbegin = rtclock();
	or_bitmap(result_bitmap, bml_nation[nation_key("FRANCE")], bml_nation[nation_key("GERMANY")]);
	or_bitmap(result_bitmap, result_bitmap, bml_nation[nation_key("BRAZIL")]);
	or_bitmap(result_bitmap, result_bitmap, bml_nation[nation_key("ALGERIA")]);
	or_bitmap(result_bitmap, result_bitmap, bml_nation[nation_key("JAPAN")]);

	int cnt = bitcount(result_bitmap);
	if(cnt)
                printf("Result : %d\n", cnt );
        clkend = rtclock();
        t = clkend-clkbegin;
        printf("\nTime: %f sec\n", t);
	return 0;
}
