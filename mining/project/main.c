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

static char s[1024];         /* a char * (=string) pointer; passed into b above */
extern int stem(char * p, int i, int j);
using namespace std;
string main_nodes[] = { "DATE", "TOPICS", "PLACES", "PEOPLE", "ORGS", "EXCHANGES", "COMPANIES", "UNKNOWN", "TEXT"};
string text_nodes[] = {"TITLE", "DATELINE", "BODY"};

struct article {
    int oid, nid;
    string date, title, body, dateline;
    vector <string> topics, places, people, orgs, exchanges, companies, unknown;
} ;

struct feature_vector {
    set <string> class_labels;
    map <string, int> words;
    vector <int> counts;
    set <string> titular_words;
};

struct config {
    bool date, topics, places, people, orgs, exchanges, companies, unknown, title, dateline, body, stopwords, stemwords, limit;
    bool gen_fv_set, gen_fv_count, gen_fv_count_sparse, gen_fd_file, gen_fd_unique_file, gen_wf_file, gen_wf_unique_file, gen_dimension_file, gen_fd_with_strings, gen_fv_title;
    int threshold_min, threshold_max;
    int dim_limit;
    int num_files;
};

map<string, int> words;
map<string, int> words_unique;
vector< map< string, int > > candidate_sets;
set <string> stop_words;
vector<article> articles;
vector<feature_vector> features ;
multimap<int, int> frequency_distribution;
multimap<int, int> unique_frequency_distribution;
set<string> dimensions ;
config conf;


void load_conf(){
    string config_key, config_value;
    ifstream ff( "config", ios::in );
    if ( !ff.is_open() ) exit(-1);
    while (! ff.eof() ){
        ff >> config_key;
        ff >> config_value;
        if(config_key == "num-files")
            conf.num_files = atoi(config_value.c_str());
        else if(config_key == "include-date")
            conf.date = (config_value == "yes" ? true : false);
        else if(config_key == "include-topics")
            conf.topics = (config_value == "yes" ? true : false);
        else if(config_key == "include-places")
            conf.places = (config_value == "yes" ? true : false);
        else if(config_key == "include-people")
            conf.people = (config_value == "yes" ? true : false);
        else if(config_key == "include-orgs")
            conf.orgs = (config_value == "yes" ? true : false);
        else if(config_key == "include-exchanges")
            conf.exchanges = (config_value == "yes" ? true : false);
        else if(config_key == "include-companies")
            conf.companies = (config_value == "yes" ? true : false);
        else if(config_key == "include-title")
            conf.title = (config_value == "yes" ? true : false);
        else if(config_key == "include-dateline")
            conf.dateline = (config_value == "yes" ? true : false);
        else if(config_key == "include-body")
            conf.body = (config_value == "yes" ? true : false);
        else if(config_key == "stop-words")
            conf.stopwords = (config_value == "yes" ? true : false);
        else if(config_key == "stem-words")
            conf.stemwords = (config_value == "yes" ? true : false);
        else if(config_key == "limit-dimension")
            conf.limit = (config_value == "yes" ? true : false);
        else if(config_key == "dimension-size")
            conf.dim_limit = atoi(config_value.c_str());
        else if(config_key == "threshold-min")
            conf.threshold_min = atoi(config_value.c_str());
        else if(config_key == "threshold-max")
            conf.threshold_max = atoi(config_value.c_str());
        else if(config_key == "gen-fv-set")
            conf.gen_fv_set = (config_value == "yes" ? true : false);
        else if(config_key == "gen-fv-count")
            conf.gen_fv_count = (config_value == "yes" ? true : false);
        else if(config_key == "gen-fv-count-sparse")
            conf.gen_fv_count_sparse = (config_value == "yes" ? true : false);
        else if(config_key == "gen-fv-title")
            conf.gen_fv_title = (config_value == "yes" ? true : false);
        else if(config_key == "gen-fd-file")
            conf.gen_fd_file = (config_value == "yes" ? true : false);
        else if(config_key == "gen-fd-unique-file")
            conf.gen_fd_unique_file = (config_value == "yes" ? true : false);
        else if(config_key == "gen-wf-file")
            conf.gen_wf_file= (config_value == "yes" ? true : false);
        else if(config_key == "gen-wf-unique-file")
            conf.gen_wf_unique_file= (config_value == "yes" ? true : false);
        else if(config_key == "gen-dimension-file")
            conf.gen_dimension_file = (config_value == "yes" ? true : false);
        else if(config_key == "gen-fd-with-strings")
            conf.gen_fd_with_strings= (config_value == "yes" ? true : false);

    }
    if (ff.is_open() ) ff.close();
}

void print_conf(){
    cout << "Number of files to parse : " << conf.num_files << endl;
    cout << "Parse ?" << endl;
    cout << "\t<Date>  \t" << conf.date << endl;
    cout << "\t<Topics>  \t" << conf.topics<< endl;
    cout << "\t<Places>  \t" << conf.places<< endl;
    cout << "\t<People>  \t" << conf.people << endl;
    cout << "\t<Orgs>  \t" << conf.orgs << endl;
    cout << "\t<Exchanges>  \t" << conf.exchanges << endl;
    cout << "\t<Companies>  \t" << conf.companies << endl;
    cout << "\t<Title>  \t" << conf.title << endl;
    cout << "\t<Dateline>  \t" << conf.dateline << endl;
    cout << "\t<Body>  \t" << conf.body << endl;
    cout << "Exclude Stop-Words ? " << conf.stopwords<< endl;
    cout << "Stem Words ? " << conf.stemwords << endl;
    cout << "Limit Dimension ? " << conf.limit<< endl;
    cout << "Dimension Size : " << conf.dim_limit<< endl;
    cout << "Threshold Minimum : " << conf.threshold_min<< endl;
    cout << "Threshold Maximum : " << conf.threshold_max<< endl;

    cout << "Generate Feature Vector - Word set: " << conf.gen_fv_set<< endl;
    cout << "Generate Feature Vector - Word count: " << conf.gen_fv_count<< endl;
    cout << "Generate Feature Vector - Word count (sparse): " << conf.gen_fv_count_sparse<< endl;
    cout << "Generate Feature Vector - Titular Words: " << conf.gen_fv_title << endl;

    cout << "Generate Frequency Distribution : " << conf.gen_fd_file << endl;
    cout << "Generate Frequency Distribution (Unique in document) : " << conf.gen_fd_unique_file<< endl;
    cout << "Generate Word Frequency : " << conf.gen_wf_file << endl;
    cout << "Generate Word Frequency (Unique in document): " << conf.gen_fd_unique_file << endl;
    cout << "Generate Dimension : " << conf.gen_dimension_file << endl;


}
/*
struct word_freqency {
    string word;
    int count;
};

struct compare_words {
  bool operator()(const word_freqency& x, const word_freqency& y) const {
    return x.word < y.word;
 }
};
*/
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
    //cout << "Old ID: " << a.oldid << endl;
    //cout << "New ID: " << a.newid << endl;
    cout << "Old ID: " << a.oid << endl;
    cout << "New ID: " << a.nid << endl;

}
string delete_tags(string tag, string text){
    int index;
    int end_inde;
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
int get_id(string str){
    int start_index;
    int end_index;
    int num;
    start_index = str.find("\"");
    if(start_index != string::npos){
        end_index = str.find("\"", start_index+1);
        //cout << start_index << " " << end_index << endl;
        string temp = str.substr(start_index+1, end_index - start_index - 1);
        //cout << temp << endl;
        num = atoi(temp.c_str());
    }
    return num;
}

article create_article(vector<string> tokens){
    int i = 0;
    //"PEOPLE", "ORGS", "EXCHANGES", "COMPANIES", "UNKNOWN", "TEXT"
    article temp;
    //temp.oldid = tokens.at(0);
    //temp.newid = tokens.at(1);
    temp.oid = get_id(tokens.at(0));
    temp.nid = get_id(tokens.at(1));
    temp.date = delete_tags("DATE", tokens.at(2));
    temp.topics = get_list(delete_tags("TOPICS", tokens.at(3)));
    temp.places = get_list(delete_tags("PLACES", tokens.at(4)));
    temp.people = get_list(delete_tags("PEOPLE", tokens.at(5)));
    temp.orgs = get_list(delete_tags("ORGS", tokens.at(6)));
    temp.exchanges = get_list(delete_tags("EXCHANGES", tokens.at(7)));
    temp.companies = get_list(delete_tags("COMPANIES", tokens.at(8)));
    //temp.body =
    cout << tokens.at(10) << endl;
    if(tokens.size() > 9)
        get_article_content(delete_tags("TEXT", tokens.at(10)), temp);
    else {temp.body = "" , temp.title = "" , temp.dateline = "";};
    return temp;
}

string lowercase(string data){
    transform(data.begin(), data.end(), data.begin(), ::tolower);
    return data;
}

bool is_numeric(string str)
{
	istringstream iss(str.c_str());
    double dTestSink;
	iss >> dTestSink;
	// was any input successfully consumed/converted?
	if ( ! iss )
		return false;

	// was all the input successfully consumed/converted?
	return ( iss.rdbuf()->in_avail() == 0 );
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
        else if(data.rfind("'s") == data.length()-2)
            data.erase(data.length()-2);
        else if(data.find("\"") == 0)
            data.erase(0, 1);
        else if(data.find("(") == 0)
            data.erase(0, 1);
        else if(data.find(",") != string::npos)
            data.erase(data.find(",") , 1);
        else if(data.find(":") == data.length()-1)
            data.erase(data.length()-1);
        else if(data.find(":") == 0)
            data.erase(0, 1);
        else if(data.find("'") == 0)
            data.erase(0, 1);
        else if(is_numeric(data))
            data.erase(0);
        else if(data.find("&") != string::npos){
            int start_index = data.find("&") ;
            int end_index = data.find(";") ;
            if(end_index != string::npos) data.erase(start_index, end_index - start_index + 1);
        }

        else if(data.find("/") != string:: npos)
            data.erase(data.find("/"), 1);
        else if(data.find("$") != string:: npos)
            data.erase(data.find("$"), 1);
        else if(data.find("-") != string:: npos)
            data.erase(data.find("-"), 1);
        else if(data.find("+") != string:: npos)
            data.erase(data.find("+"), 1);
        else break;
    }
    return data;
}

string stem_word(string str){
    string temp;
    strcpy(s, str.c_str());
    //cout << s;
    s[stem(s,0,str.length() - 1)+1] = 0;
    temp = s;
    //if(str != temp)
    //cout << str << " " << temp << endl;
    return s;

}

set <string> get_set_from_string(string str){
    set <string> temp_set;
    string temp;
    stringstream ss(str);

    while(ss >> temp){
        temp = clean_up(temp);
        if(conf.stemwords) temp = stem_word(temp);
        if(temp.length() > 1)  {
            if(stop_words.find(temp) == stop_words.end())
                temp_set.insert(temp);
        }
    }

    return temp_set;
}

void parse_and_enter_into_set(article art){

    string temp;
    map<string, int> candidate_set;
    vector<string> fields;
    if(conf.body)
        fields.push_back (art.body);
    if(conf.title)
        fields.push_back (art.title);
    if(conf.topics)
        for(int i=0; i< art.topics.size(); i++)
            fields.push_back (art.topics.at(i));
    if(conf.places)
        for(int i=0; i< art.places.size(); i++)
            fields.push_back (art.places.at(i));
    if(conf.companies)
        for(int i=0; i< art.companies.size(); i++)
            fields.push_back (art.companies.at(i));


    for(int i = 0; i < fields.size(); i++){
        stringstream ss(fields.at(i));
        while(ss >> temp){
            temp = clean_up(temp);
            if(conf.stemwords) temp = stem_word(temp);
            if(temp.length() > 1)  {
                if(words.find(temp) != words.end()){
                    words.find(temp)->second++;
                }else{
                    if(stop_words.find(temp) == stop_words.end()) words.insert(pair<string, int> (temp, 1));
                }
                if(candidate_set.find(temp) != candidate_set.end()){
                    candidate_set.find(temp)->second++;
                }else{
                    // this word was not parsed in this document previously. first occurence
                    if(stop_words.find(temp) == stop_words.end()) {
                        candidate_set.insert(pair<string, int> (temp, 1));
                        if(words_unique.find(temp) != words_unique.end()){
                                words_unique.find(temp)->second++;
                        }else{
                                words_unique.insert(pair<string, int> (temp, 1));
                        }
                    }
                }
               }
            }
    }
    candidate_sets.push_back(candidate_set);
}

void generate_dimension_set(){
    cout << "Generating dimensions" << endl;
    map<string, int>::const_iterator pos;
    for(pos = words_unique.begin(); pos != words_unique.end(); ++pos){
        if(pos->second > conf.threshold_min && pos->second < conf.threshold_max){
            // above threshold. need to include in the dimension vector
            dimensions.insert(pos->first);
        }
    }
    cout << "Size of dimension vector : " << dimensions.size() << endl;
}

void generate_feature_vectors(){
    cout << "Generating feature vectors" << endl;
    feature_vector vec;
    article art;
    map<string, int>::const_iterator pos;
    set<string>::const_iterator pos_set;
    map<string, int> candidate_set;
    for(int i = 0; i < articles.size() ; i++){
        vec.class_labels.clear();
        vec.words.clear();
        vec.counts.clear();
        art = articles.at(i);
        candidate_set = candidate_sets.at(i);
        for(int j = 0; j < art.topics.size(); j++)
            {
            vec.class_labels.insert(art.topics.at(j));
            }
        for(pos = candidate_set.begin(); pos != candidate_set.end(); ++pos){
            // here we need to truncate based on a set of valid words.
            //if(words_unique[pos->first] > 5 && words_unique[pos->first] < 3000 )
             //   vec.words.insert(pair<string, int> (pos->first, pos->second));
             if(dimensions.find(pos->first) != dimensions.end())
                vec.words.insert(pair<string, int> (pos->first, pos->second));
        }
        for( pos_set = dimensions.begin(); pos_set != dimensions.end(); ++pos_set){
            //cout << *pos_set << endl;
            if(candidate_set.find(*pos_set) != candidate_set.end())
                vec.counts.push_back(candidate_set[*pos_set]);
            else
                vec.counts.push_back(0);

        }
        vec.titular_words = get_set_from_string(art.title);
        features.push_back(vec);

    }
}

void dump_dimensions_to_file(){
    ofstream ff( "dimensions", ios::out );
    set<string>::const_iterator pos;
    int index = 1;
    cout << "Writing Dimensions to file" << endl;
    for(pos = dimensions.begin(); pos != dimensions.end(); ++pos , index++)
        ff << index << " " << *pos << endl;

}

void dump_feature_vectors(){
    map<string, int>::const_iterator pos;
    set<string>::const_iterator pos1;
    ofstream ff;
    ofstream ff1;
    ofstream ff2;
    ofstream ff3;
    feature_vector f_vec;
    if(conf.gen_fv_set)
        ff.open ( "fv-word-set");
    if(conf.gen_fv_count)
        ff1.open ( "fv-word-count");
    if(conf.gen_fv_count_sparse)
        ff2.open ( "fv-word-count-sparse");
    if(conf.gen_fv_title)
        ff3.open ( "fv-titular-words");
    if ( (conf.gen_fv_set && !ff.is_open())
        || (conf.gen_fv_count && !ff1.is_open() )
        || (conf.gen_fv_count_sparse && !ff2.is_open() )
        || (conf.gen_fv_title && !ff3.is_open() )
        )
    return;
    cout << "Writing Feature vectors" << endl;
    //for(int i =0 ; i < articles.size(); i++ ){
    for(int i =0 ; i < features.size(); i++ ){
        f_vec  = features.at(i);
        //cout << "Writing: " << endl;
        if(conf.gen_fv_set)
            ff << i+1 << " { ";

        if(conf.gen_fv_count)
            ff1 << i+1 << " { ";

        if(conf.gen_fv_count_sparse)
            ff2 << i+1 << " { ";

        if(conf.gen_fv_title)
            ff3 << i+1 << " { ";
        // write class labels
        // if config is to generate feature vector set of strings
        if(conf.gen_fv_set){
            for(pos1 = f_vec.class_labels.begin(); pos1 != f_vec.class_labels.end(); ++pos1 )
                ff << *pos1 << " ";
            ff << "}, { ";
            // write feature vectors
            ff << f_vec.words.size();
            ff << " }, { ";
            for(pos = f_vec.words.begin(); pos != f_vec.words.end(); ++pos)
                ff << pos->first << " ";
            ff << "}" << endl;
        }

        if(conf.gen_fv_count){
            for(int j = 0; j < f_vec.counts.size(); j++)
                ff1 << f_vec.counts.at(j) << " ";
            ff1 << "}" << endl;
        }

        if(conf.gen_fv_count_sparse){
            for(int j = 0; j < f_vec.counts.size(); j++)
                if(f_vec.counts.at(j) != 0) ff2 << j+1 << " (" << f_vec.counts.at(j) << ") ";
            ff2 << "}" << endl;
        }

        if(conf.gen_fv_title){
            for(pos1 = f_vec.titular_words.begin(); pos1 != f_vec.titular_words.end(); ++pos1 )
                ff3 << *pos1 << " ";
            ff3 << "}" <<endl;
        }

    }

    if(ff.is_open()) ff.close();
    if(ff1.is_open()) ff1.close();
    if(ff2.is_open()) ff2.close();
    if(ff3.is_open()) ff3.close();

}

void load_stop_words(){
    string stop_word;
    ifstream ff( "stopwords", ios::in );
    if ( !ff.is_open() )
        return;
    while( !ff.eof() ){
        ff >> stop_word;
        stop_words.insert(stop_word);
    }
}

void dump_word_frequencies_to_file(){
    map<string, int>::const_iterator pos;

    if(conf.gen_wf_file){
        ofstream ff( "word_frequencies", ios::out );
        if ( !ff.is_open() )
        return;
        cout << "Writing word frequencies to file" << endl;

        for(pos = words.begin(); pos != words.end(); ++pos)
            ff <<pos->first<<' ' << pos->second << " " << endl;
        ff.close();

    }
    if(conf.gen_wf_unique_file){
        ofstream ff1( "unique_word_frequencies", ios::out );
        if ( !ff1.is_open() )
        return;
        for(pos = words_unique.begin(); pos != words_unique.end(); ++pos)
            ff1 <<pos->first<<' ' << pos->second << " " << endl;
        ff1.close();
    }
}

void dump_frequency_distribution_to_file(){
    map<string, int>::const_iterator pos;
    multimap<int, int>::const_iterator pos1;
    multimap<int, int>::const_iterator pos2;

    ofstream ff, ff1;
    if(conf.gen_fd_file)
        ff.open( "frequency_distribution");
    if(conf.gen_fd_unique_file)
        ff1.open( "unique_frequency_distribution");

    if ( (conf.gen_fd_file && !ff.is_open() )|| ( conf.gen_fd_unique_file && !ff1.is_open() ))
        return;

    cout << "Writing frequency distribution to file" << endl;

    if(conf.gen_fd_file){
        pos1 = frequency_distribution.end();
        pos1--;
        for(; pos1 != frequency_distribution.begin(); pos1--){
            ff << pos1->first <<' ' << pos1->second << endl;
        }
        ff << pos1->first <<' ' << pos1->second << endl;
        if(ff.is_open()) ff.close();
    }


    if(conf.gen_fd_file){
        pos2 = unique_frequency_distribution.end();
        pos2--;
        for(; pos2 != unique_frequency_distribution.begin(); pos2--){
            ff1 << pos2->first <<' ' << pos2->second << endl;
        }

        ff1 << pos2->first <<' ' << pos2->second << endl;
        if(ff1.is_open()) ff1.close();
    }

}

void dump_frequency_distribution_to_file_with_strings(){
        map<string, int>::const_iterator pos;
    multimap<int, int>::const_iterator pos1;
    multimap<int, int>::const_iterator pos2;

    ofstream ff, ff1;
    if(conf.gen_fd_file)
        ff.open( "frequency_distribution");
    if(conf.gen_fd_unique_file)
        ff1.open( "unique_frequency_distribution");

    if ( (conf.gen_fd_file && !ff.is_open() )|| ( conf.gen_fd_unique_file && !ff1.is_open() ))
        return;

    cout << "Writing frequency distribution to file" << endl;

    if(conf.gen_fd_file){
        pos1 = frequency_distribution.end();
        pos1--;
        for(; pos1 != frequency_distribution.begin(); pos1--){
            pos = words.begin();
            std::advance(pos, pos1->second);
            ff << pos1->first <<' ' << pos1->second << " " << pos->first << endl;
        //            ff << pos1->first <<' ' << pos1->second << endl;
        }
        pos = words.begin();
        std::advance(pos, pos1->second);
        ff << pos1->first <<' ' << pos1->second << " " << pos->first << endl;
        //ff << pos1->first <<' ' << pos1->second << endl;
        if(ff.is_open()) ff.close();
    }


    if(conf.gen_fd_file){
        pos2 = unique_frequency_distribution.end();
        pos2--;
        for(; pos2 != unique_frequency_distribution.begin(); pos2--){
            pos = words_unique.begin();
            std::advance(pos, pos2->second);
            ff1 << pos2->first <<' ' << pos2->second << " " << pos->first << endl;
            //ff1 << pos2->first <<' ' << pos2->second << endl;
        }
        pos = words_unique.begin();
        std::advance(pos, pos2->second);
        ff1 << pos2->first <<' ' << pos2->second << " " << pos->first << endl;
        //ff1 << pos2->first <<' ' << pos2->second << endl;
        if(ff1.is_open()) ff1.close();
    }
// ends

    cout << endl;
}


void create_frequency_distribution(){

    map<string, int>::const_iterator pos;

    int index = 0;
    cout << "Creating frequency distribution" << endl;
    for(pos = words.begin(); pos != words.end(); ++pos)
            frequency_distribution.insert(pair<int, int> (pos->second, index++));

    index = 0;
    for(pos = words_unique.begin(); pos != words_unique.end(); ++pos)
            unique_frequency_distribution.insert(pair<int, int> (pos->second, index++));

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

void parse_articles(){
    cout << "Parsing articles" << endl;
    for(int i =0; i < articles.size(); i++){
        article temp_a = articles.at(i);
        parse_and_enter_into_set(temp_a);
    }
}

void load_file(){
    article a;
    string name;
    string accumulator;
    vector<string> tokens;
    int main_index = 0;
    int start_index = 0;


    for(int i= 0; i < conf.num_files; i++){
    //for(int i= 0; i < 2; i++){
        char filename[13];
        if(i <10) sprintf(filename, "reut2-00%d.sgm", i);
        else sprintf(filename, "reut2-0%d.sgm", i);
        ifstream ff( filename, ios::in );
        if ( !ff.is_open() )
             {cout << i ; exit(-1);}
        cout << "Loading file: " << filename <<endl;

        while( !ff.eof() )
        {
            main_index = 0;
            while( !ff.eof() && name !=  "<REUTERS"){
                ff >> name;
            }
            while( !ff.eof() && name.find("OLDID=") == string::npos){
                ff >> name;
            }
            tokens.push_back(name);
            while( !ff.eof() && name.find("NEWID=") == string::npos){
                ff >> name;
                // run till we find the new id
            }
            tokens.push_back(name);

            while( !ff.eof()){//} && ){
                ff >> name;

                if(name ==  "</REUTERS>")  break;
                string end_node = "</" + main_nodes[main_index] + ">";
                string start_node = "<" + main_nodes[main_index];//+ ">";
                if(name.find(end_node) != string::npos) {
                    cout << name << endl;
                    cout << accumulator << endl;
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
                accumulator += " " + name;
            }
//            cout << " Next article " << articles.size() << endl;
            //if(tokens.size() == 11)
           // cout << "asd" << endl;
            if(!ff.eof())
                {
                    a = create_article(tokens);
                    articles.push_back(a);
                    //print_article(a);
                }
                //cout << tokens.size() << " clearing" << endl;
                tokens.clear();
        }
        if ( ff.is_open() ) ff.close();
    }

}

int main()
{
    double clkbegin, clkend, t;
    load_stop_words();
    load_conf();
    print_conf();
    clkbegin = rtclock();

    load_file();
    //exit(0);
    cout << "Total number of articles : " << articles.size() << endl;
    int count = 0 ;
    for(int i =0; i < articles.size(); i++){
        if(articles.at(i).body == "") {cout << i+1 << " " << articles.at(i).nid << " | "; count++;}
        //if(i == 29)
        {
            print_article(articles.at(i));//for(int)
        }

    }
    cout << "count : " << count;
    //print_article(articles.at(610));
    exit(0);
    parse_articles();
    cout << "Total number of words : " << words.size() << endl;

    create_frequency_distribution();

    if(conf.gen_wf_file || conf.gen_wf_unique_file)
        dump_word_frequencies_to_file();

    if(conf.gen_fd_with_strings)
        dump_frequency_distribution_to_file_with_strings();
    else
        dump_frequency_distribution_to_file();

    clkend = rtclock();
    t = clkend-clkbegin;
    cout << "Time: " << t << "sec " <<endl;

    generate_dimension_set();
    if(conf.gen_dimension_file)
        dump_dimensions_to_file();

    clkbegin = rtclock();
    generate_feature_vectors();

    dump_feature_vectors();

    if(conf.gen_dimension_file)
        dump_dimensions_to_file();

    clkend = rtclock();
    t = clkend-clkbegin;
    cout << "Time: " << t << "sec " <<endl;

    return 0;
}
