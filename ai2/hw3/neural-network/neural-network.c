#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <set>
#include <float.h>
#include <limits.h>
#define NUM_TUPLES 200
// 0.5 0 0 23
// 0.45 0 0 23
// 0.01 -10.5 1 24
// 0.5 -1 1 19
//#define NUM_ITERATIONS 10000

using namespace std;

double weights[4][31];
double prev_weights[4][31];
double min_sum = FLT_MAX;;
float ALPHA  = 0;//0.01; // 0.01 // 24
float BIAS  = 0;//-10.5; // -10.5
float INITIAL_WEIGHT  = 0;//1; // 1
float NUM_ITERATIONS = 100000;
//float NUM_ITERATIONS = 1;

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
        for(int j = 0; j < 31; j++)
            cout << "w" << j << "\t" << weights[0][j] << "\t"
                                    << weights[1][j] << "\t"
                                    << weights[2][j] << "\t"
                                    << weights[3][j] << "\t" << endl;
}

void copy_weights(){
    for(int i = 0 ; i < 31; i++){
        prev_weights[0][i] = weights[0][i];
        prev_weights[1][i] = weights[1][i];
        prev_weights[2][i] = weights[2][i];
        prev_weights[3][i] = weights[3][i];
    }
}


void train(int  p_num , int training_data[][34]){
    int output;
    for(int i = 0; i < 200; i++){
            output = output_of_perceptron(p_num, training_data[i]);
            update_weights(p_num, output, training_data[i]);
    }
}

int test(int vec[34]){
    int count = 0;
    for(int i =0; i < 4; i++){
        int output = output_of_perceptron(i,vec) ;
        cout << output << " (" << vec[30+i] << ")\t\t";
        if( output != vec[30 + i]) {
            count++;
        }
    }
    if(count != 0)
        cout << "Fail";
    cout << endl;
    return count;

}

bool convergence(int p_num, unsigned int epoch){
    //cout << "Num : " << NUM_ITERATIONS <<endl;
    if (epoch > NUM_ITERATIONS) return true;
    //cout << epoch << endl;
    double sum = 0.0;
    for(int i = 0; i < 31; i++){
        sum += pow(fabs(weights[p_num][i] - prev_weights[p_num][i]),2)/2;

    }
//    cout << sum << " " ;
    if (sum < 0.000001) return true;
//    if(sum < 0.03) cout << "Conv" << endl; //return true;
    return false;
}

int evaluate_model(int data[][34]){
    int count_mismatches = 0;
    cout << "Output format: Output-Of-Perceptron (Actual-Output)" << endl <<endl;
    cout << "Buck-I-Mart\tOxley's\t\tOxley's cafe\tBrennan's" << endl<<endl;
    for(int i = 0; i < 50; i++){
        int ret;
        ret = test(data[i]);
        if(ret != 0){
            count_mismatches++;
        }
    }
    return count_mismatches;
}

int main(int argc, char * argv[]){
    if(argc != 5) {
        cout <<endl << "USAGE: \n./neural-network <alpha> <bias> <initial-weight> <number-of-terations>" << endl;
        exit(0);
    }
    // parse commandline options
    sscanf(argv[1], "%f" , &ALPHA);
    sscanf(argv[2], "%f" , &BIAS);
    sscanf(argv[3], "%f" , &INITIAL_WEIGHT);
    sscanf(argv[4], "%f" , &NUM_ITERATIONS);
    cout << "Alpha : " << ALPHA <<endl;
    cout << "Bias : " << BIAS << endl;
    cout << "Init Weight : " << INITIAL_WEIGHT << endl;
    cout << "Epoch : " << NUM_ITERATIONS << endl;
//    cout << "Asd" << argv[4] << NUM_ITERATIONS << endl;
    int test_data[51][34];
    int training_data[201][34]; // there are 200 rows and 34 columns

    read_file(training_data, "train-nn");
    read_file(test_data, "test-nn");
    init_weights();
    int last_min_error = INT_MAX;
    int error;
    for(int i =0; i<4; i++){
        unsigned int epoch = 0;
        do{
            copy_weights();
            train(i, training_data);
            epoch++;
        }while(!convergence(i, epoch));
        //cout <<endl<< endl;
    }
    float errors = evaluate_model(test_data) ;
    cout << endl << "Number of errors = " << errors << endl << endl ;
    cout << "Accuracy = " << ((50.0 - errors) * 100  / 50 ) << "%" <<endl << endl;
    
    cout << "Weights" << endl<<endl;
    print_weights();
    return 0;
}
