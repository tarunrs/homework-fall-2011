#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <set>
#include <float.h>
#define NUM_TUPLES 200

#define DEFAULT_CLASS_LABEL 0

using namespace std;

//bool used_columns[] = {false,false, false, false, false, false, false, false, false, false};
int choices[] = {3, 3, 2, 4, 4, 3, 3, 5, 3};
int training_data[200][10]; // there are 200 rows and 10 columns


struct tree_node{
    int num_children;
    vector <tree_node *> children;
    int column;
    vector<int> partition_indexes;
    int decision;
};

void split(const string &s, int elems[10]) {
    char delim = ',';
    int index = 0;
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems[index] = atoi(item.c_str());
        index++;
    }
}

void read_file(int data[][10], string filename){
    ifstream ff(filename.c_str(), ios::in);
    string temp;
    int index = 0;
    while(!ff.eof()){
        ff >> temp;
        split(temp, data[index]);
        index++;
//        cout << index;
    }
    ff.close();
}

void print_training_data(){
    for(int i = 0; i < NUM_TUPLES; i++){
        for(int j = 0; j < 10; j++)
            cout << training_data[i][j] << " ";
        cout << endl;
    }
}


int count(int data[][10], vector<int> &indexes, int rest_index){
    int count = 0;
    for(unsigned int i = 0; i < indexes.size(); i++){
        if(data[indexes.at(i)][9] == rest_index) count++;
    }
    return count;
}

void count_class_labels(int data[][10], vector<int> &counts, vector<int> &indexes){
    for(int i = 0; i < 4; i++)
        counts.push_back(count(data, indexes, i));
}

bool empty_node(vector<int> &indexes){
    if(indexes.size() == 0) return true;
    return false;
}

int homogeneous_class_labels(int data[][10], vector<int> &indexes){
    vector<int> counts;
    int non_zero_class_label = -1;
    int num_zeroes = 0;
    count_class_labels(data, counts, indexes);
    //cout << "Count : "<< counts.size() << endl;
    for(unsigned int i=0; i < counts.size(); i++){
        if(counts.at(i) == 0)
            num_zeroes++;
        else
            non_zero_class_label = i;
    }
    if(num_zeroes == 3) return non_zero_class_label;
    return -1;
}

void print_counts(vector<int> &counts){
    for(unsigned int i = 0; i < counts.size(); i++)
        cout << "Counts of class label " << i << ": " << counts.at(i) << endl;

}

double entropy(int data[][10], vector<int> &indexes){
    vector<int> counts;
    int total = 0;
    count_class_labels(data, counts, indexes);
    for(unsigned int i=0; i < counts.size(); i++){
        total += counts.at(i);
    }
    double ent = 0;
    for(unsigned int i = 0; i < counts.size(); i++){
        double prob = (double)counts.at(i)/(double)total;
        if(prob != 0)
            ent += (-1) * prob * log(prob) / log(2);
        //cout << "Prob : " << prob;
    }
    return ent;
}

int find_optimal_split(int training_data[][10], vector<int> indexes, set <int> used_columns){
//    int total_tuple_in_parent = indexes.size();
    int col;
    double min_entropy = FLT_MAX;
    int optimal_col = -1;
    for(col = 0; col < 9; col++){
        if(used_columns.find(col) != used_columns.end()) continue;

        vector<int> temp_indexes[choices[col]];

        for(unsigned int i = 0; i < indexes.size(); i++){
            temp_indexes[training_data[indexes.at(i)][col]].push_back(indexes.at(i));
        }
        vector <double> entropies;
        int size = 0;
        for(int i=0; i < choices[col]; i++){
            if(temp_indexes[i].size() > 0)
                entropies.push_back(entropy(training_data, temp_indexes[i]));
            else
                entropies.push_back(0);
            //cout << "Size for column "<< col << " choice" << i << " : " << temp_indexes[i].size() << endl;
            size += temp_indexes[i].size();
        }
        double weighted_sum_of_entropies = 0;
        for(int i = 0; i < choices[col]; i++){
          
            weighted_sum_of_entropies += ( (double) temp_indexes[i].size() / size ) * entropies.at(i);
        }
        if(min_entropy > weighted_sum_of_entropies){
            min_entropy = weighted_sum_of_entropies;
            optimal_col = col;
        }
    }
    return optimal_col;
}

void construct_tree(tree_node * parent, set<int> used_columns){
    int homogeneous = homogeneous_class_labels(training_data, parent->partition_indexes);
    if (homogeneous != -1){
        parent->decision = homogeneous;
        return;
    }
    if(empty_node(parent->partition_indexes)) return;

    int split_col = find_optimal_split(training_data, parent->partition_indexes, used_columns);
    if(split_col == -1) return; // done with all the columns
    vector<int> temp_indexes[choices[split_col]];
    for(unsigned int i = 0; i < parent->partition_indexes.size(); i++){
        temp_indexes[training_data[parent->partition_indexes.at(i)][split_col]].push_back(parent->partition_indexes.at(i));
    }
    //cout << "Split col : " << split_col << endl;
    parent->column = split_col;
    used_columns.insert(split_col);// = true;
    //cout << "Choices : "<< choices[split_col] << endl;
    parent->num_children = choices[split_col];

    for(int i = 0; i < choices[split_col]; i++){
        tree_node* leaf;
        leaf = new tree_node;
        leaf->partition_indexes = temp_indexes[i];
        //cout << "Child " << i << " has " << temp_indexes[i].size() << " children " << endl << endl;;
        //getchar();
        leaf->decision = -1;
        leaf->num_children = -1;
        leaf->column = -1;
        construct_tree(leaf, used_columns);
        parent->children.push_back(leaf);
    }

}

void print_tree(tree_node * parent, int level){
    for(int i = 0; i < level - 1; i++) cout << "\t|" ;
        if(level != 0)cout << "-----> " ;
    if(parent->column != -1)
        cout << parent->column << endl;
    else
        cout << parent->decision << endl;
    for(int i = 0 ; i < parent->num_children; i++){
        //cout << "\t";
        print_tree(parent->children.at(i), level + 1);

    }

}

int classify(tree_node* node, int test[10]){
    if(node->column == -1) 
        return node->decision;
    return classify(node->children.at(test[node->column]), test);
}

int main(){

    int test_data[51][10];
    vector<int> indexes;
    read_file(training_data, "train");
    read_file(test_data, "test");
    for(int i=0; i < NUM_TUPLES; i++){
        indexes.push_back(i);
    }

    tree_node * root;
    root = new tree_node;
    root->num_children = -1;
    root->column = -1;
    root->partition_indexes = indexes;
    set<int> used_columns;
    construct_tree(root, used_columns);
    print_tree(root, 0);

    int diff_count = 0;
    int default_labels = 0;
    for(int i =0 ; i<50; i++){
        int predicted_label = classify(root, test_data[i]);
        if (predicted_label != -1 )
            { 
                cout << "Predicted label : " <<  predicted_label << " Actual label : " << test_data[i][9] << endl;
                if(predicted_label != test_data[i][9]) diff_count++;
            }
        else
            { 
                cout << "Predicted label : " <<  DEFAULT_CLASS_LABEL << " Actual label : " << test_data[i][9] << endl;
                default_labels++;
                if(DEFAULT_CLASS_LABEL != test_data[i][9]) diff_count++;
            }

    }
    cout << endl << "Number mislabeled : " << diff_count << endl;
    cout << endl << default_labels << " records could not be classified and were assigned a Default class label" << endl;
    cout << endl << "Accuracy = " << (50 - diff_count) * 100 /50 << "%" <<endl;
    
    exit(1);
}
