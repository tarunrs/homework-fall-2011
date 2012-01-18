#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>
#include <set>
#include <map>
#include <locale>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>      /* for malloc, free */
#include <ctype.h>       /* for isupper, islower, tolower */

using namespace std;
string main_nodes[] = { "DATE", "TOPICS", "PLACES", "PEOPLE", "ORGS", "EXCHANGES", "COMPANIES", "UNKNOWN", "TEXT"};
string text_nodes[] = {"TITLE", "DATELINE", "BODY"};

struct article {
    int oid, nid;
    string date, title, body, dateline;
    vector <string> topics, places, people, orgs, exchanges, companies, unknown;
} ;


void load_file(){
    article a;
    string name;
    string accumulator;
    vector<string> tokens;
    int main_index = 0;
    int start_index = 0;


    //for(int i= 0; i < conf.num_files; i++){
    for(int i= 0; i < 2; i++){
        char filename[13];
        if(i <10) sprintf(filename, "reut2-00%d.sgm", i);
        else sprintf(filename, "reut2-0%d.sgm", i);
        ifstream ff( filename, ios::in );
        if ( !ff.is_open() )
             {cout << i ; exit(-1);}
        cout << "Loading file: " << filename <<endl;

        while( !ff.eof() )
        {
            //cout << tokens.size() << " " << name ;
            tokens.clear();

            //cout << tokens.size() << " " << name <<endl;
            main_index = 0;
            while( !ff.eof() && name !=  "<REUTERS"){
                ff >> name;
                // run till we find the next article
            }
            while( !ff.eof() && name.find("OLDID=") == string::npos){
                ff >> name;
                // run till we find the old id
            }
            cout << tokens.size() << " " << name << endl;
            tokens.push_back(name);
            cout << tokens.size() << " " << name << endl;

            while( !ff.eof() && name.find("NEWID=") == string::npos){
                ff >> name;
                // run till we find the new id
            }
            tokens.push_back(name);
            cout << tokens.size() << " ";
            while( !ff.eof()){//} && ){
                ff >> name;
                if(name ==  "</REUTERS>")  break;
                string end_node = "</" + main_nodes[main_index] + ">";
                string start_node = "<" + main_nodes[main_index] + ">";
                if(name.find(end_node) != string::npos) {
                    if(name.find(start_node) != string::npos)
                        accumulator = name;
                    else
                        accumulator += " " + name;
                    cout << tokens.size() << " " ;;
                    tokens.push_back(accumulator);
                    cout << tokens.size() << " " ;
                    main_index++;
                    cout << tokens.size() << " ";
                    continue;
                }
                else if(name.find(start_node) != string::npos) {
                    accumulator = "";
                }
                accumulator += " " + name;
            }
//            cout << " Next article " << articles.size() << endl;
            //if(tokens.size() == 11)
           // cout << "asd" << endl;
            if(!ff.eof())
                {
                    cout << "Token size : " << tokens.size() << endl;
                    if(tokens.size() > 11)
                    {
                        for(int k = tokens.size() -1; k > 0; k--)
                            cout << tokens.at(k) << endl;
                    }
                    //a = create_article(tokens);
                    //articles.push_back(a);
                    //print_article(a);
                }
                cout << tokens.size() << " clearing" << endl;
                tokens.clear();
        }
        if ( ff.is_open() ) ff.close();
    }

}
int main()
{
	load_file();
	return 0;
}
