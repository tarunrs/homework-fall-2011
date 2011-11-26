#include<stdio.h>
#include<iostream>
#include<string.h>
#include<string>
#include<math.h>
using namespace std;
char bm_region[10][751500];
char bm_nation[25][751500];
char bm_year[10][751500];
string regions[] = {"AFRICA", "AMERICA", "ASIA", "EUROPE", "MIDDLE EAST"};
string nations[] = {"ALGERIA", "ARGENTINA", "BRAZIL", "CANADA", "EGYPT", "ETHIOPIA", "FRANCE", "GERMANY", "INDIA", "INDONESIA", "IRAN", "IRAQ", "JAPAN", "JORDAN", "KENYA", "MOROCCO", "MOZAMBIQUE", "PERU", "CHINA", "ROMANIA", "SAUDI ARABIA", "VIETNAM", "RUSSIA", "UNITED KINGDOM", "UNITED STATES"};
int years[] ={1992, 1993,1994, 1995,1996, 1997,1998, 1999, 2000, 2001};
char num[2];
void dump_region_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "region";
	FILE *fp;
	for(i=0;i<5;i++){
		sprintf(tmpstr, "%s%d", fname.c_str(), i);
		fp = fopen(tmpstr, "w");
		if(fp){
			printf("writting file: %d\n", sizeof(bm_region[i]));
			fwrite (bm_region[i] , 1 , sizeof(bm_region[i]) , fp );
		}
		fclose(fp);
	}
}

void load_region_bit_map(){
	int i;
	char tmpstr[1024];
	string fname = "region";
	FILE *fp;
	for(i=0;i<5;i++){
		sprintf(tmpstr, "%s%d", fname.c_str(), i);
		fp = fopen(tmpstr, "r");
		if(fp){
			printf("reading file: %d\n", sizeof(bm_region[i]));
			int result = fread (bm_region[5+i] , 1 , sizeof(bm_region[i]) , fp );
			printf("result %d\n", result);
		}
		fclose(fp);
	}
}

bool verify_bitmaps(){
	for(int i=0;i<5;i++)
		for(int j=0;j<751500;j++)
			if(bm_region[i][j]!=bm_region[5+i][j]) return false;
	return true;
}

void init(){
	for(int i=0;i<5;i++)
		for(int j=0;j<751500;j++)
			bm_region[i][j] = j%127;
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
int main(){

	num[0]=0;
	num[1] =0;
	int q[] = {0,1,1,0,0,1,0,1, 1,0,1,0,1,1,0,1};
	for(int i=15; i>=0; i--){
		if(q[i] ==1)
			set(i);
	}
	printf("%d\n", sizeof(int));
	printf("%s\n%d\n",byte_to_binary(num[0]),num[0]);
	printf("%s\n%d\n",byte_to_binary(num[1]),num[1]);
	printf("%s\n%d\n",byte_to_binary(num[1]&num[0]),num[1]&num[0]);
	init();
	dump_region_bit_map();
	load_region_bit_map();
	if(verify_bitmaps())
		printf("Verified\n");
	else
		printf("Not same\n");
	return 0;
}
