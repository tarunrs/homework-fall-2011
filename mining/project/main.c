#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
using namespace std;
string main_nodes[] = { "DATE", "TOPICS", "PLACES", "PEOPLE", "ORGS", "EXCHANGES", "COMPANIES", "UNKNOWN", "TEXT"};
string text_nodes[] = {"TITLE", "DATELINE", "BODY"};
set<string> words;

struct article {
    string old_id, new_id;
    string oldid, newid;
    string date, title, body, dateline;
    vector <string> topics, places, people, orgs, exchanges, companies, unknown;
} ;

struct word_freqency {
    string word;
    int frequency;
};

struct compare_words {
  bool operator()(const word_freqency& x, const word_freqency& y) const {
    return x.word < y.word;
 }
};
void print_article(article a){
    int i;
    //"PEOPLE", "ORGS", "EXCHANGES", "COMPANIES", "UNKNOWN", "TEXT"
    cout << "Date : " << a.date << endl;
    cout << "Dateline : " << a.dateline << endl;
    cout << "Title : " << a.title<< endl;

    cout << "Topics : ";
    for(i=0; i < a.topics.size(); i++)
        cout << a.topics.at(i) << " , ";
    cout << endl;

    cout << "Body : " << a.body << endl;
    cout << "Places : ";
    for(i=0; i < a.places.size(); i++)
        cout << a.places.at(i) << " , ";
    cout << endl;

    cout << "Orgs : ";
    for(i=0; i < a.orgs.size(); i++)
        cout << a.orgs.at(i) << " , ";
    cout << endl;

    cout << "Exchanges : ";
    for(i=0; i < a.exchanges.size(); i++)
        cout << a.exchanges.at(i) << " , ";
    cout << endl;

    cout << "Companies: ";
    for(i=0; i < a.companies.size(); i++)
        cout << a.companies.at(i) << " , ";
    cout << endl;
    cout << "Old ID: " << a.oldid << endl;
    cout << "New ID: " << a.newid << endl;

}
string delete_tags(string tag, string text){
    int index;
    index = text.find("<" + tag + ">");
    if(index != string::npos)
        text.erase(index, tag.length()+2);
    index = text.find("</" + tag + ">");
    if(index != string::npos)
        text.erase(index, tag.length()+3);
    return text;
}

vector<string> get_list(string node){
    int start_index = 0;
    int end_index = 0;
    string temp;
    vector<string> lst;
    end_index = node.find("</D>", start_index);
    while(end_index != string::npos)
    {
        temp = node.substr(start_index, end_index - start_index);
        temp = delete_tags("D", temp);
        //cout << temp << endl ;
        lst.push_back(temp);
        start_index = end_index+4;
        end_index = node.find("</D>", start_index);
    }
    return lst;
}
void get_article_content (string text, article &a){
    int start_index = 0;
    int end_index = 0;
    string temp = "";
    start_index = text.find("<TITLE>", 0);
    end_index = text.find("</TITLE>", start_index);
    if(end_index != string::npos)
        temp = text.substr(start_index, end_index - start_index);
    temp = delete_tags("TITLE", temp);
    //cout << temp << endl;
    a.title = temp;
    temp.clear();
    // find dateline

    start_index = text.find("<DATELINE>", 0);
    end_index = text.find("</DATELINE>", start_index);
    if(end_index != string::npos)
        temp = text.substr(start_index, end_index - start_index);

    temp = delete_tags("DATELINE", temp);
    a.dateline = temp;
      //  cout << temp << endl;
    temp.clear();

    // find text body
    start_index = text.find("<BODY>", 0);
    end_index = text.find("</BODY>", start_index);
    if(end_index != string::npos)
        temp = text.substr(start_index, end_index - start_index);
    temp = delete_tags("BODY", temp);
    a.body= temp;
        //cout << temp << endl;
    temp.clear();
}
//
article create_article(vector<string> tokens){
    int i = 0;
    //"PEOPLE", "ORGS", "EXCHANGES", "COMPANIES", "UNKNOWN", "TEXT"
    article temp;
    temp.oldid = tokens.at(0);
    temp.newid = tokens.at(1);
    temp.date = delete_tags("DATE", tokens.at(2));
    temp.topics = get_list(delete_tags("TOPICS", tokens.at(3)));
    temp.places = get_list(delete_tags("PLACES", tokens.at(4)));
    temp.people = get_list(delete_tags("PEOPLE", tokens.at(5)));
    temp.orgs = get_list(delete_tags("ORGS", tokens.at(6)));
    temp.exchanges = get_list(delete_tags("EXCHANGES", tokens.at(7)));
    temp.companies = get_list(delete_tags("COMPANIES", tokens.at(8)));
    //temp.body =
    get_article_content(delete_tags("TEXT", tokens.at(10)), temp);
    return temp;
}

string lowercase(string data){
    transform(data.begin(), data.end(), data.begin(), ::tolower);
    //cout << data <<endl;
    return data;
}
string clean_up(string data){
    data = lowercase(data);
    while(data.length() > 1)
    {
        //break;
        if(data.rfind(".") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.rfind(",") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.rfind("\"") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.rfind(")") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.rfind(">") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.rfind("'") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.find("\"") == 0)
            data.erase(0, 1);
        else break;
    }
    return data;
}

void parse_and_enter_into_set(const string &s){
    stringstream ss(s);
    string temp;
    while(ss >> temp){
        temp = clean_up(temp);
        if(temp.length() > 1) words.insert(temp);
        }

}

int main()
{
    article a;
    ifstream ff( "test", ios::in );
    if ( !ff.is_open() )
    return -1;

    string name;
    string accumulator;
    word_freqency wf1, wf2;
    vector<string> tokens;
    vector<article> articles;
    int main_index = 0;

    /*set<word_freqency, compare_words> wfs;

    wf1.frequency = 0;
    wf1.word = "first";
    wf2.frequency = 0;
    wf2.word = "asecond";
    wfs.insert(wf1);
    wfs.insert(wf2);
    set<int>::const_iterator pos;
    for(pos = wfs.begin(); pos != wfs.end(); ++pos)
        cout<<"a"<<' ';

*/
    int start_index =0;
    while( !ff.eof() )
    {
        main_index = 0;
        while( !ff.eof() && name !=  "<REUTERS"){
            ff >> name;
            // run till we find the next article
        }
        while( !ff.eof() && name.find("OLDID=") == string::npos){
            ff >> name;
            // run till we find the old id
        }
        tokens.push_back(name);

        while( !ff.eof() && name.find("NEWID=") == string::npos){
            ff >> name;
            // run till we find the new id
        }
        tokens.push_back(name);
        //cout << "next article" << endl;
        //cout << name << endl;
        // now we iterate through and parse the article
        while( !ff.eof()){//} && ){
            //cout << name << endl;
            //cout << "!" << name << "!" << endl;

            ff >> name;
            if(name ==  "</REUTERS>")  break;
            //cout << "!" << name << endl;
            //cout << name << endl;
            //cout << "out" << endl;
            string end_node = "</" + main_nodes[main_index] + ">";
            string start_node = "<" + main_nodes[main_index] + ">";
            //cout << end_node;
            if(name.find(end_node) != string::npos) {
                //cout << name <<endl;
                //words.insert(name);
                if(name.find(start_node) != string::npos)
                    accumulator = name;
                else
                    accumulator += " " + name;
                tokens.push_back(accumulator);
                main_index++;
                continue;
            }
            else if(name.find(start_node) != string::npos) {
                accumulator = "";
            }
            //cout << "1";
            accumulator += " " + name;
            //cout << accumulator << endl;
        }
        //cout << "next article" << endl;
        //article a;
        if(tokens.size() == 11)
            {
                a = create_article(tokens);
                articles.push_back(a);
                //print_article(a);
            }
            tokens.clear();
    }
    //for(int j=0; j< tokens.size(); j++){
      //  cout << tokens.at(j)<<endl<<endl;
    //}
    for(int i =0; i < articles.size(); i++){
        article temp_a = articles.at(i);
        cout << temp_a.newid << endl;
        //for(int j=0; j<temp_a.places.size(); j++)
          //  words.insert(temp_a.places.at(j));
        parse_and_enter_into_set(temp_a.body);
    }
    set<string>::const_iterator pos;
    for(pos = words.begin(); pos != words.end(); ++pos)
        cout<<*pos<<' ';
    return 0;
}










