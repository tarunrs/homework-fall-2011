#include "main.h"//
#include <stdio.h>

Search_Node* search_agent_dfs(int board[4][4]){
	Node_State * initial_state;
	Search_Node * temp;
	Search_Node * leaf;
	stack<Search_Node*> pq;
	vector<Search_Node*> vec;

	initial_state = new Node_State(board);
	temp = new Search_Node(initial_state, NULL, 0);

	pq.push(temp);
	do{

		if(pq.empty()) {
			cout << "Queue is Empty. Terminating" <<endl;
			return NULL;
		}
		leaf = pq.top();
        //leaf->print_solution();
		if (leaf->is_goal()){
			cout<<endl<<"Solution Found using DFS"<<endl<<endl;
			leaf->print_solution();
			return leaf;
		}
		else{
			vec.clear();
			vec = leaf->expand();
			pq.pop();
			for(int i =0; i < vec.size(); i++){
				temp = vec[i];
				pq.push(temp);
			}
		}

	}while(!pq.empty());
	return NULL;
}

void read_file(string filename, int board[4][4]){
    ifstream ifile;
    ifile.open(filename.c_str());
    string element;
    int i=0, j=0;
    while(!ifile.eof() && i< 16){
        ifile >> element;
        board[i/4][i%4] = atoi(element.c_str());
        i++;
    }
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
    char delim = ' ';
    vector<string> elems;
    return split(s, delim, elems);
}

int main(int argc, char* argv[]){

    int board[4][4];

    if(argc != 2){
        cout<<"USAGE ERROR:\n Needs to be of the form\n\n ./sudoku <input_filename>"<<endl;
        exit(0);
    }

    string filename = argv[1];
    read_file(filename, board);
    cout << "Input Sudoku configuration:\n\n";
    for (int i =0; i<4; i++){
        for (int j =0; j<4; j++)
            cout<< board[i][j]<<"\t";
        cout<< endl;
    }

    search_agent_dfs(board);

	return 1;
}
