#include "main.h"
#include <stdio.h>

Node_State::Node_State(){
	for(int i=0; i <4; i++)
        for(int j=0;j<4; j++)
            board[i][j]=-1;
}

Node_State::Node_State(int state[4][4]){
	for(int i=0; i <4; i++)
        for(int j=0;j<4; j++)
            board[i][j]=state[i][j];
}


Node_State::Node_State(int state[4][4], int pos, int val){
    int i,j;
	for(i=0; i <4; i++)
        for(j=0;j<4; j++)
            board[i][j]=state[i][j];
    i = pos/4;
    j= pos%4;
    board[i][j] = val;
}

Search_Node::Search_Node(void){
	m_state = NULL;
	m_parent_node = NULL;
	m_action = -1;
}
Search_Node::Search_Node(Node_State * n_state, Search_Node * parent_node, int action){
	m_state = n_state;
	m_parent_node = parent_node;
	m_action = action;
	if(parent_node){
		m_depth = parent_node->m_depth + 1;
	} else {
		m_depth = 0;
	}
}

bool Search_Node::is_goal(){
	return (this->m_state->isSolution());
}
int Search_Node::find_next_empty_position(){
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            if(this->m_state->board[i][j] == -1) return ((i*4)+j);

}

bool Search_Node::can_fill(int pos, int val){
        int i , j;
        i = pos/4;
        j = pos%4;

        //std::cout<<"checking for : "<< pos<<" "<<val<<std::endl<<i<<"," <<j<<std::endl;
        for(int k =0; k<4; k++){
            if(this->m_state->board[i][k] == val) {
                //std::cout<<"in same row"<<i<<","<<k<<std::endl;
                return false;
            }
            if(this->m_state->board[k][j] == val) {
                //std::cout<<"in same col"<<k<<","<<j<<std::endl;
                return false;
            }
        }
        //std::cout<<"Not in row or col"<<std::endl;
        if(i == 0 || i == 2){
                //check ith row and i+1th row
                if(j==0 || j == 2){
                    //check jth column and j+1th column
                    if(this->m_state->board[i][j+1] == val || this->m_state->board[i+1][j] == val || this->m_state->board[i+1][j+1] == val ) return false;
                } else {
                  //check jth column and j-1th column
                    if(this->m_state->board[i][j-1] == val || this->m_state->board[i+1][j] == val || this->m_state->board[i+1][j-1] == val ) return false;
                }
            }
        else{
                //check ith row and i-1th row
                if(j==0 || j == 2){
                    //check jth column and j+1th column
                    if(this->m_state->board[i][j+1] == val || this->m_state->board[i-1][j] == val || this->m_state->board[i-1][j+1] == val ) return false;
                } else {
                  //check jth column and j-1th column
                    if(this->m_state->board[i][j-1] == val || this->m_state->board[i-1][j] == val || this->m_state->board[i-1][j-1] == val ) return false;
                }

        }
        return true;
}

std::vector <Search_Node *> Search_Node::expand(){

	Node_State * temp_state;
	Search_Node * temp_node;
	std::vector <Search_Node*> vec_nodes;

    int pos = find_next_empty_position();

	for (int i = 1; i <= 4;i++){

            if(can_fill(pos, i)){
                //std::cout<<"Filling "<<i;
                temp_state = new Node_State(this->m_state->board, pos, i);
                temp_node = new Search_Node(temp_state, this, i);

                //if(!this->path_to_root_contains_state(temp_state))
                    vec_nodes.push_back(temp_node);
            }
	}
	return vec_nodes;
}

bool Node_State::isSolution(){
	for(int i =0; i<4;i++)
        for(int j =0; j<4;j++)
            {
            if (this->board[i][j] == -1){
                //std::cout << i<< ","<< j<<endl;
                return false;
            }
            }
    return true;
}

void Search_Node::print_solution(void){

    for(int i= 0; i <4;i++){
        for(int j= 0 ;j <4;j++)
            std::cout<< this->m_state->board[i][j] << "  ";
        cout<<endl;
    }
}
