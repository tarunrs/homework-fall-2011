#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <set>
#include <float.h>
#define NUM_TUPLES 200
// 0.5 0 0 23
// 0.45 0 0 23
// 0.01 -10.5 1 24
// 0.5 -1 1 19
#define ALPHA 0.5 // 0.01 // 24
#define BIAS -1 // -10.5
#define INITIAL_WEIGHT 1 // 1

using namespace std;

double weights[4][31];

void split(const string &s, int elems[34]) {
    char delim = ',';
    int index = 0;
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)) {
        elems[index] = atoi(item.c_str());
        index++;
    }
}

void read_file(int data[][34], string filename){
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

void print_training_data(int training_data[][34]){
    for(int i = 0; i < NUM_TUPLES; i++){
        for(int j = 0; j < 34; j++)
            cout << training_data[i][j] << " ";
        cout << endl;
    }
}

void init_weights(){
    weights[0][0] = weights[1][0] = weights[2][0] = weights[3][0] = BIAS;
    for(int i = 1; i < 31; i++)
        weights[0][i] = weights[1][i] = weights[2][i] = weights[3][i] = INITIAL_WEIGHT;
}

int output_of_perceptron(int p_num, int vec[34]){
    double sum = weights[p_num][0];
    for(int i = 0; i < 30; i++){
        sum += vec[i] * weights[p_num][i+1];
    }
    if ( sum > 0 ) return 1;
    return 0;
}

void update_weights(int p_num, int output, int vec[34]){
    int error = vec[30 + p_num] - output;
    weights[p_num][0] = weights[p_num][0] + ( ALPHA * error );
    for(int i = 1; i< 31; i++){
        weights[p_num][i] = weights[p_num][i] + ( ALPHA * error * vec[i-1]);
    }
}

void print_weights(){
 //   for(int i =0; i < 4; i++)
    //int i = 0;
    {
//        cout << "Weights for perceptron number " << i << endl;
        for(int j = 0; j < 31; j++)
            cout << "w" << j << "\t" << weights[0][j] << "\t"
                                    << weights[1][j] << "\t"
                                    << weights[2][j] << "\t"
                                    << weights[3][j] << "\t" << endl;
//        cout << endl;// << endl;
    }
}

void train(int training_data[][34]){
    int output;
    for(int i = 0; i < 200; i++){
        for(int j = 0; j < 4; j++){
            output = output_of_perceptron(j, training_data[i]);
            update_weights(j, output, training_data[i]);
        }
        
    }
}

int test(int vec[34]){
    int count = 0;
    for(int i =0; i < 4; i++){
//        cout << "Output of perceptron " << i << " : " << output_of_perceptron(i,vec) << " Actual value : " << vec[30+i] << endl;
        if( output_of_perceptron(i,vec) != vec[30 + i]) {
            count++;
           // cout << "Output of perceptron " << i << " : " << output_of_perceptron(i,vec) << " Actual value : " << vec[30+i] << endl;
        }
            
    }
    return count;
    
}

int evaluate_model(int data[][34]){
    int count_mismatches = 0;
    for(int i = 0; i < 50; i++){
//        cout << "I = " << i << endl;
        int ret;
        ret = test(data[i]);
        if(ret != 0){
            //cout << "Number of mismatches in perceptron output for tuple" << i << " = " << ret << endl;
            count_mismatches++;
        }
    }
    return count_mismatches;
}

int main(){
    int test_data[51][34];
    int training_data[201][34]; // there are 200 rows and 34 columns

    read_file(training_data, "train-nn");
    read_file(test_data, "test-nn");
    init_weights();
    train(training_data);
    cout << "Number of errors = " << evaluate_model(test_data) << endl;
    print_weights();
    return 0;
}
