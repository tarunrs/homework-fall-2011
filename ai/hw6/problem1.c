#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>

using namespace std;
const string policy_strings[] = {"scarlet", "grey", "black"};

struct transition {
    int from_state;
    int to_state;
    int action;
    float probability;
} transition_table[21];

struct description {
    int id;
    string name;
    string pic;
    float reward;
    string remark;
    bool is_terminating;
} place_descriptions[11];

struct state_utility {
    int id;
    float utility;
    int policy;
} policy_table[11];


float utility(int id){

}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s ) {
    char delim = ',';
    vector<string> elems;
    return split(s, delim, elems);
}


void update_transitions(string line, int i){
    vector<string> trans = split(line);
    transition_table[i].from_state = atoi(trans.at(0).c_str());
    if(trans.at(1) == "S")
        transition_table[i].action = 0;
    else if(trans.at(1) == "G")
        transition_table[i].action = 1;
    else
        transition_table[i].action = 2;

    transition_table[i].to_state = atoi(trans.at(2).c_str());
    transition_table[i].probability = atof(trans.at(3).c_str());
}


void update_desc(string line, int i){
    vector<string> descs = split(line);
    place_descriptions[i].id = atoi(descs.at(0).c_str());
    place_descriptions[i].name = descs.at(1);
    place_descriptions[i].pic = descs.at(2);
    place_descriptions[i].reward = atof(descs.at(3).c_str());
    if(descs.size() > 4)
        place_descriptions[i].remark = descs.at(4);
}



void read_file(void){

    string line;
    ifstream transfile ("problem1.transitions");
    ifstream descfile ("problem1.desc");
    if (transfile.is_open())
    {
        int i = 1;
        while ( i < 19 && !transfile.eof())
        {
        getline (transfile,line);
        cout << line << endl;
        update_transitions(line, i);
        i++;
        }
    transfile.close();
    }

    if (descfile.is_open())
    {
        int i = 1;
        while ( i < 11)
        {
        getline (descfile,line);
        update_desc(line, i);
        cout << line << endl;
        i++;
        }
    descfile.close();

    }
}


int main(void) {
    read_file();
    return 0;
}
