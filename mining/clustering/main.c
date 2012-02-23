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
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>      /* for malloc, free */
#include <ctype.h>       /* for isupper, islower, tolower */

#define BITMAP_SIZE (1506/8) + 1

using namespace std;

struct cluster_details{
    int num_docs;
    map <string, int> class_counts;
    float min_distance;
    int metoid;

};

struct doc_bitmap {
    int doc_id;
    char bitmap[BITMAP_SIZE];
    int cluster_id;
    int count;
    set <string> class_labels;
    vector <int> word_counts;
    int temp_count;
};

struct config {
    int num_clusters, dimension_size, min_points;
    float radius;
};

struct dbscan_point{
    int doc_id;
    int cluster_id;
    bool is_core_point;
    bool is_border_point;
    bool is_noise_point;
    vector <int> border_points;
};

set <string> class_labels;
vector<doc_bitmap> doc_bitmaps;
vector<cluster_details> clusters;
vector< vector<int> > cluster_doc_ids;
vector < dbscan_point > dbscan_points;
config conf;
float jaccard_distance[12000][12000];
float cosine_distance[12000][12000];

int num_changes;

int bitcount_in_byte (unsigned char n) {
   int count = 0;
   while (n) {
      count += n & 0x1u;
      n >>= 1;
   }
   return count;
}

int bitcount(char * bitmap_array){
	int num = BITMAP_SIZE;
	long count=0;
	for(int i=0; i< num; i++){
		count+= bitcount_in_byte(bitmap_array[i]);
	}
	return count;
}

void and_bitmap(char * dest_bitmap_array, char * first_bitmap_array, char * second_bitmap_array){
	int num = BITMAP_SIZE;
	long count=0;
	for(int i=0; i< num; i++){
		dest_bitmap_array[i] = first_bitmap_array[i] & second_bitmap_array[i];
	}
}

void or_bitmap(char * dest_bitmap_array, char * first_bitmap_array, char * second_bitmap_array){
	int num =  BITMAP_SIZE;
	long count=0;
	for(int i=0; i< num; i++){
		dest_bitmap_array[i] = first_bitmap_array[i] | second_bitmap_array[i];
	}
}

void copy_bitmap(char * dest_bitmap_array, char * source_bitmap_array){
	int num =  BITMAP_SIZE;
	long count=0;
	for(int i=0; i< num; i++){
		dest_bitmap_array[i] = source_bitmap_array[i];
	}
}



void load_conf(){
    string config_key, config_value;
    ifstream ff( "config", ios::in );
    if ( !ff.is_open() ) exit(-1);
    while (! ff.eof() ){
        ff >> config_key;
        ff >> config_value;
        if(config_key == "num-clusters")
            conf.num_clusters = atoi(config_value.c_str());
        else if(config_key == "dimension-size")
            conf.dimension_size = atoi(config_value.c_str());
        else if(config_key == "min-points")
            conf.min_points = atoi(config_value.c_str());
        else if(config_key == "radius")
            conf.radius = atof(config_value.c_str());

    }
    if (ff.is_open() ) ff.close();
}

void print_conf(){
    cout << "Number of Clusters : " << conf.num_clusters << endl;
    cout << "Number of Dimensions in file : " << conf.dimension_size << endl;
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



void init_word_counts(vector<int> &wc){
    wc.clear();
    //cout << "Conf dimensions" << conf.dimension_size << endl;
    for(int i = 0; i < conf.dimension_size; i++)
        { //cout << i <<endl;
            wc.push_back(0);
        }
    //cout << "wc = " << wc.size() << endl;
}

void init_bitmap(char * bmap){
    for(int i = 0; i < BITMAP_SIZE; i++)
        bmap[i] = 0;
}

void set_bitmap(char * bmap, long pos){
	bmap[pos/8] = bmap[pos/8] | (int)pow(2,7 - (pos%8));
}

int get_number_in_brackets(string num_in_brackets){
    num_in_brackets.erase(num_in_brackets.size() - 1, 1);
    num_in_brackets.erase(0, 1);
    return atoi(num_in_brackets.c_str());
}

void read_feature_vectors(){
    ifstream ff( "results/fv-word-count-sparse", ios::in );
    string temp;
    int c=0;
    doc_bitmap temp_dmap;
    if ( !ff.is_open() )
         {cout << "Could not open feature vector file" ; exit(-1);}
    cout << "Loading feature vectors" <<endl;

    while( !ff.eof() ){
        ff >> temp;
        temp_dmap.doc_id = atoi(temp.c_str());
        init_bitmap(temp_dmap.bitmap);
        init_word_counts(temp_dmap.word_counts);
        //cout << "WC = " << temp_dmap.word_counts.size() << endl;
        temp_dmap.class_labels.clear();
        temp_dmap.cluster_id = -1;
        temp_dmap.count = 0;
        temp_dmap.temp_count = 0;
        ff >> temp;
        if(temp == "{"){
            while(temp != "},"){
                // read the topics
                ff >> temp;
                //cout << temp << " ";
                if(temp.size() > 2)
                    temp_dmap.class_labels.insert(temp.c_str());

            }
        }
        // done with class labels.
        // insert count of words
        ff >> temp; // "{"
        ff >> temp; // "number of words"
        temp_dmap.count = atoi(temp.c_str());
        ff >> temp; // "}"
        //insert feature words
        ff >> temp;
        if(temp== "{"){
            ff >> temp;
            while(temp != "}"){
                // read the topics

                //if(temp.size() > 2)
                int word_index = atoi(temp.c_str());
                set_bitmap(temp_dmap.bitmap, word_index);
                temp_dmap.temp_count++;
                ff >> temp; // "number in brackets of the count"
                temp_dmap.word_counts.at(word_index - 1) = get_number_in_brackets(temp); // word index - 1 because index in dimension starts at 1
                //cout << temp << " ";

                ff >> temp; // " next word"
            }
        }
        // include a condition so that we push the doc vector only iof it contains topics and has a body
        if(temp_dmap.class_labels.size() > 0 && temp_dmap.count > 0)
            doc_bitmaps.push_back(temp_dmap);
        //cout << c++ <<  endl;
    }
}
void init_clusters(){
    cluster_details temp_clust;
    for(int i=0; i < conf.num_clusters; i++){
        vector <int> docids;
        temp_clust.min_distance = FLT_MAX;
        temp_clust.num_docs = 0;
        temp_clust.metoid = rand() % (doc_bitmaps.size() - 1); // uncomment later
        clusters.push_back(temp_clust);
        cluster_doc_ids.push_back(docids);// push empty vector
    }
}

void make_new_cluster(cluster_details &temp_clust){
    vector <int> doc_ids;
    temp_clust.min_distance = FLT_MAX;
    temp_clust.num_docs = 0;
    temp_clust.metoid = -1;
    clusters.push_back(temp_clust);
    cluster_doc_ids.push_back(doc_ids);
}


void reset_clusters(){
    for(int i=0; i < conf.num_clusters; i++){
        clusters.at(i).num_docs = 0;
        cluster_doc_ids.at(i).clear();
    }
}

void print_cluster_details(){

   for(int i=0; i < conf.num_clusters; i++){
       // cout << "Min Cluster Distance : " << clusters.at(i).min_distance<< endl;
        cout << "Number of documents in cluster ( " <<  i << " ) : " << clusters.at(i).num_docs << endl;
        cout << "Metoid Doc_ID = " << clusters.at(i).metoid<< endl;
        //cout << "Number of elements in set : " << cluster_doc_ids.at(i).size() << endl;;
        //cout << "Class count :" << clusters.at(i).class_counts.size() <<  endl<< endl;

    //    for(int j =0; j < clusters.at(i).class_counts.size(); j++){
            //cout
      //  }
    }
}

float jaccard_similarity(char * source, char* destination){
    char res_bitmap[BITMAP_SIZE];
    and_bitmap(res_bitmap, source, destination);
    float c = bitcount(res_bitmap);
    float a = bitcount(source);
    float b = bitcount(destination);
    return c / ( a + b - c );
}

float cosine_similarity(vector<int> &source, vector<int> &destination){

    float sum_a_2 = 0.0, sum_b_2 = 0.0, sum_a_b = 0.0;
    for(int i = 0 ; i < conf.dimension_size; i++){
        float a = source.at(i);
        float b = destination.at(i);
        sum_a_2 += pow(a, 2);
        sum_b_2 += pow(b, 2);
        sum_a_b += a * b;
    }
    return (sum_a_b /(sqrt(sum_a_2) * sqrt(sum_b_2)));
}

void update_cluster(doc_bitmap &db, int index){

    float min_distance = FLT_MAX;
    int temp_cluster = -1;
    float dist ;
    for(int i=0; i< clusters.size(); i++){
        //dist = jaccard_distance[index][clusters.at(i).metoid];
        dist = cosine_distance[index][clusters.at(i).metoid];
        //cout << dist << " ";
        if(min_distance > dist && dist != -1){
            // found a closer cluster
            temp_cluster = i;
            min_distance = dist;
        }
    }
    if(temp_cluster != -1){
    // found a cluster.
        if(db.cluster_id != temp_cluster)
            num_changes++; // cluster changed from previous iteration

        db.cluster_id = temp_cluster; // set ne cluster id

        //if(db.doc_id != 0)
        //cluster_doc_ids.at(temp_cluster).push_back(db.doc_id);
        cluster_doc_ids.at(temp_cluster).push_back(index);

        clusters.at(temp_cluster).num_docs ++;
        if(db.class_labels.size() > 0){
            for(set<string>::iterator it = db.class_labels.begin() ; it != db.class_labels.end(); it++){
                string cl = *it;
                if(clusters.at(temp_cluster).class_counts.find(cl) != clusters.at(temp_cluster).class_counts.end()){
                    clusters.at(temp_cluster).class_counts[cl]++;
                }else{
                    clusters.at(temp_cluster).class_counts.insert( pair<string, int>( cl, 1));
                }
            }
        }
    }
}

void make_clusters(){
    num_changes = 0;
    for(int i =0; i < doc_bitmaps.size(); i++){ //doc_bitmaps.size(); uncomment
        update_cluster(doc_bitmaps.at(i), i);
    }
    cout << "Number of changes in cluster = " << num_changes << endl;
}

void print_cluster(int i){
    vector <int> doc_ids;
    int it;
    doc_ids = cluster_doc_ids.at(i);
    for(it = 0; it < doc_ids.size(); it++)
        cout << doc_ids.at(it) << " " ;

}


void dump_clusters(){
    vector <int> doc_ids;
    char filename[18];
    char filename1[18];

    for(int i=0; i < cluster_doc_ids.size(); i++){
        doc_ids = cluster_doc_ids.at(i);
        sprintf(filename,"results/cluster%d", i);
        sprintf(filename1,"results/classes%d", i);
        ofstream ff (filename, ios::out);
        ofstream ff1 (filename1, ios::out);
        for(int j = 0; j< doc_ids.size(); j++){
            ff << doc_ids.at(j) << endl;
        }
        int max = -1, sum = 0, num_topics = 0;
        for(map<string, int>::iterator it = clusters.at(i).class_counts.begin(); it != clusters.at(i).class_counts.end(); it++){
            if(it->second > max) max = it->second;
            sum+= it-> second;
            num_topics++;
            ff1 << it->first << " " << it->second << endl;

        }
        ff1 << endl << "Sum = " << sum << endl << "Max = " << max <<endl << "Num Topics = " << num_topics << endl;
        double entropy_sum = 0;
        for(map<string, int>::iterator it = clusters.at(i).class_counts.begin(); it != clusters.at(i).class_counts.end(); it++){
            double prob;
            prob = (double)it->second / (double)sum;
            entropy_sum += (-1) * (prob  * log2(prob)/ log2(num_topics));
        }
        ff1 << endl << "Entropy = " << entropy_sum <<endl;
        ff.close();
        ff1.close();
    }

}

void update_metoids(){
    int it = 0;
    int jt = 0 ;
    vector <int> docs ;
    int new_metoid = -1;
    int i ;
    for(i = 0 ; i < clusters.size(); i++){
        //update_metoid(clusters.at(i));
        float global_min = FLT_MAX;
        new_metoid = -1;
        docs = cluster_doc_ids.at(i);

        cout << "Updating metoid for cluster " << i << "(" << docs.size()  << ") "<< endl;
        //cout << docs.at(docs.size() - 1) << endl;
        for(it = 0; it < docs.size(); it++ ){
            float local_max = -1;
            int doc_being_compared = docs.at(it);
            //cout << doc_being_compared << endl;
            for(jt = 0; jt < docs.size(); jt++ ){
                float dist;
               // cout << doc_being_compared << " " << docs.at(jt) - 1 << endl;
                dist =  cosine_distance[doc_being_compared][docs.at(jt)]; // indexes of doc IDs start from 1. but index os distances start from 0. hence
                if(local_max < dist){ local_max = dist;}
            }
            //cout << doc_being_compared << endl;
           // cout << "." ;
            if(global_min > local_max) {
                global_min = local_max;
                new_metoid = doc_being_compared;
            }
        }

        clusters.at(i).metoid = new_metoid;
    }
}

void create_jaccard_distance_matrix(){
    for(int i = 0; i < doc_bitmaps.size();  i++){
        for(int j = 0; j < doc_bitmaps.size(); j++)
            jaccard_distance[i][j] = 1.0 - jaccard_similarity(doc_bitmaps.at(i).bitmap, doc_bitmaps.at(j).bitmap);
        //cout << i << endl;
    }
}

void create_cosine_distance_matrix(){
    for(int i = 0; i < doc_bitmaps.size();  i++){
        for(int j = i+1; j < doc_bitmaps.size(); j++){
            cosine_distance[i][j] = 1.0 - cosine_similarity(doc_bitmaps.at(i).word_counts, doc_bitmaps.at(j).word_counts);
            cosine_distance[j][i] = cosine_distance[i][j];
        }

        cout << i << endl;
    }
}

void dump_jaccard_distance_matrix(){
    ofstream ff("results/jaccard_distance1", ios::out);
    for(int i = 0; i < doc_bitmaps.size();  i++){
        for(int j = 0; j < doc_bitmaps.size(); j++) // doc_bitmaps.size()
            ff << jaccard_distance[i][j] << " ";
        ff << endl;
    }
}

void dump_cosine_distance_matrix(){
    ofstream ff("results/cosine_distance", ios::out);
    for(int i = 0; i < doc_bitmaps.size(); i++){
        for(int j = 0; j < doc_bitmaps.size(); j++) // doc_bitmaps.size()
            ff << cosine_distance[i][j] << " ";
        ff << endl;
    }
}
void load_jaccard_distance_matrix(){
    ifstream ff("results/jaccard_distance1", ios::in);
    cout << doc_bitmaps.size();
    for(int i = 0; i < doc_bitmaps.size();  i++){
        for(int j = 0; j < doc_bitmaps.size(); j++)
            ff >> jaccard_distance[i][j];
    }
    ff.close();
}

void load_cosine_distance_matrix(){
    ifstream ff("results/cosine_distance", ios::in);
    cout << doc_bitmaps.size();
    for(int i = 0; i < doc_bitmaps.size(); i++){
        for(int j = 0; j < doc_bitmaps.size(); j++)
            ff >> cosine_distance[i][j];
    }
    ff.close();
}

void init_jaccard_distance_matrix(){
    for(int i = 0; i < doc_bitmaps.size(); i++){
        for(int j = 0; j < doc_bitmaps.size(); j++)
            jaccard_distance[i][j] = -1 ;
    }
}

void init_cosine_distance_matrix(){
    for(int i = 0; i < doc_bitmaps.size(); i++){
        for(int j = 0; j < doc_bitmaps.size(); j++)
            cosine_distance[i][j] = -1 ;
            cosine_distance[i][i] = 0;
    }
}

void init_db_scan(){
    for (int i = 0; i < doc_bitmaps.size(); i++){
        dbscan_point temp_pt;
        temp_pt.doc_id = doc_bitmaps.at(i).doc_id;
        temp_pt.cluster_id = -1;
        temp_pt.is_core_point = false;
        temp_pt.is_border_point = false;
        temp_pt.is_noise_point = true;
        dbscan_points.push_back(temp_pt);
    }
}

void db_scan(){
    int num_points = 0;
    for(int i = 0; i < doc_bitmaps.size(); i++){
        num_points = 0;
        for(int j = 0; j < doc_bitmaps.size(); j++){
            if ( i == j) continue;
            if(cosine_distance[i][j] < conf.radius){
                num_points++;
                dbscan_points.at(i).border_points.push_back(j);
                //dbscan_point.at(j).is_border_point = true;
            }

        }
        cout << "Num points for " << i << " is " << num_points << endl;
        if(num_points > conf.min_points){
            cout << i << " is a core point with " << num_points << " border points" << endl;
            //getchar();
            dbscan_points.at(i).is_core_point = true;
        }

    }
}

void update_class_counts(int doc_index, int cluster_id ){
    doc_bitmap db = doc_bitmaps.at(doc_index);
    if(db.class_labels.size() > 0){
        for(set<string>::iterator it = db.class_labels.begin() ; it != db.class_labels.end(); it++){
            string cl = *it;
            if(clusters.at(cluster_id).class_counts.find(cl) != clusters.at(cluster_id).class_counts.end()){
                clusters.at(cluster_id).class_counts[cl]++;
            }else{
                clusters.at(cluster_id).class_counts.insert( pair<string, int>( cl, 1));
            }
        }
    }
}

void db_scan_cluster(){
    int cluster_id = -1;
    for(int i = 0; i < dbscan_points.size(); i++){
        //dbscan_point temp_dbscan_pt = ;
        int current_cluster_id;
        if(dbscan_points.at(i).is_core_point == false) continue;

        if(dbscan_points.at(i).cluster_id == -1){
            cluster_details temp_clust_details;
            make_new_cluster(temp_clust_details);
            cluster_id++;
            dbscan_points.at(i).cluster_id = cluster_id;
            clusters.at(cluster_id).num_docs++;
            update_class_counts(i, cluster_id);
            cluster_doc_ids.at(cluster_id).push_back(i);
        }
        current_cluster_id = dbscan_points.at(i).cluster_id; // to set the border points to this cluster

        for(int j = 0; j < dbscan_points.at(i).border_points.size(); j++){
            int border_point_index = dbscan_points.at(i).border_points.at(j);
            if(dbscan_points.at(border_point_index).cluster_id != -1){ // border point does not have a cluster id associated with it
                dbscan_points.at(border_point_index).cluster_id = current_cluster_id;
                update_class_counts(j, current_cluster_id);
                cluster_doc_ids.at(current_cluster_id).push_back(j);
            }
        }

    }
}



int main()
{
    cout << "starting" <<endl;
    srand ( time(NULL) );
    load_conf();
    read_feature_vectors();
    //cout << "Initializing clusters" << endl;
    //init_clusters();
    cout << "Creating distance matrix " << endl;
    //init_jaccard_distance_matrix();
    //create_jaccard_distance_matrix();
    //dump_jaccard_distance_matrix();
    //load_jaccard_distance_matrix();
    cout << "Radius : " << conf.radius << endl;
    cout << "Num points : " << conf.min_points << endl;
    init_cosine_distance_matrix();
    //create_cosine_distance_matrix();
    //dump_cosine_distance_matrix();
    load_cosine_distance_matrix();
    //exit(0);
    //print_cluster_details();
    num_changes = 9999999;
    cout << "Initializing DB Scan" << endl;
    init_db_scan();
    cout << "Scanning for core points" << endl;
    db_scan();
    cout << "Creating clusters" << endl;
    db_scan_cluster();

    /*while(num_changes > 1){
        cout  << num_changes << endl;
        reset_clusters();
        make_clusters();
        //print_cluster_details();
        update_metoids();
        print_cluster_details();
    }*/
    dump_clusters();
    return 0;
}













