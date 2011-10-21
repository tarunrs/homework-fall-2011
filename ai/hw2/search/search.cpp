#include "main.h"
#include <stdio.h>

Node_State::Node_State(){

	start_side = "";
	end_side = "";
	move ="";
}

Node_State::Node_State(std::string s1, std::string s2, std::string s3){
//Node_State::Node_State(std::string s1, std::string s2){
	start_side = s1;
	end_side = s2;
	move = s3;
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
        //cout<<"parent is not null"<<endl;
		m_path_cost = parent_node->m_path_cost + path_cost(m_state->move);
		m_depth = parent_node->m_depth + 1;
	} else {
	    //cout<<"parent is null"<<endl;
		m_path_cost = 0;
		m_depth = 0;
	}
}

int get_min_time(std::string move){
	if(move.find("A") != std::string::npos)
		return 1;
	else if(move.find("B") != std::string::npos)
		return 2;
	else if(move.find("C") != std::string::npos)
		return 5;
	else if(move.find("D") != std::string::npos)
		return 10;
    else
        return 0;
}

void print_vector(std::vector <Search_Node*> vec){
    cout<<endl;
    cout<<"\tStart Side\t|\tEnd Side\t|\tAction\t\n\n";
    for(int i =vec.size()-1; i>=0; i--){
        cout << "\t"<<vec.at(i)->m_state->start_side<<"\t\t|";
        cout << "\t"<<vec.at(i)->m_state->end_side<<"\t\t|";
        //cout<< vec.at(i)->m_action;
		cout << "\t"<< (vec.at(i)->m_action != -1 ? (vec.at(i)->m_action == 1 ? vec.at(i)->m_state->move + " ->" : "<- "+vec.at(i)->m_state->move) : " - ")<<"\t"<<endl;
		//temp = temp->m_parent_node;
    }
	cout << endl<<"Path Cost :"<< vec.at(0)->m_path_cost<<endl;
	cout << "Depth :"<< vec.at(0)->m_depth<<endl;
}

int Search_Node::path_cost(std::string move){
    return get_max_time(move);
}

int Search_Node::get_max_time(std::string move){
	if(move.find("D") != std::string::npos)
		return 10;
	else if(move.find("C") != std::string::npos)
		return 5;
	else if(move.find("B") != std::string::npos)
		return 2;
	else if(move.find("A") != std::string::npos)
		return 1;
    else
		return 0;
}

bool Search_Node::is_goal(){
	return (this->m_state->isSolution());
}

std::vector <Search_Node *> Search_Node::expand(){
	//vector <Search_Node> leaf_nodes;
	if(this->m_state->start_side.find("F") != string::npos)
		return(this->enumerate_states(1));
	else
		return(this->enumerate_states(0));

}

std::vector <Search_Node*> Search_Node::get_solution(void){
    std::vector <Search_Node*> vec_nodes;
    Search_Node * temp_node;
    temp_node= this;
	do{
		vec_nodes.push_back(temp_node);
		temp_node= temp_node->m_parent_node;

	}while(temp_node!= NULL);
    return vec_nodes;
}


std::vector <Search_Node *>  Search_Node::enumerate_states(bool flag){
	string e_str ="";
	string s_str = "";
	Node_State * temp_state;
	//vector<Search_Node> list_of_leaves;
	Search_Node * temp_node;
	std::vector <Search_Node*> vec_nodes;
	if (flag){
		s_str = this->m_state->start_side;
		e_str = this->m_state->end_side;
	}
	else{
		s_str = this->m_state->end_side;
		e_str = this->m_state->start_side;
	}
	for (int i = 0; i < s_str.length() - 1;i++){
		for(int j = i; j < s_str.length() - 1; j++){
			string temp_start_state = "", temp_end_state="", temp_move="";
			temp_start_state = s_str;
			temp_end_state = e_str;
			if( i == j ){
				temp_start_state.erase(i,1);
				temp_start_state.erase(temp_start_state.length()-1, 1);
				temp_end_state.append(s_str, i, 1);
				temp_end_state.append(s_str, s_str.length()-1, 1);
				temp_move.append(s_str, i,1);
				temp_move.append(s_str,s_str.length()-1, 1);
			}
			else{
				// j is always higher when i != j
				temp_start_state.erase(j,1);
				temp_start_state.erase(i,1);
				temp_start_state.erase(temp_start_state.length()-1, 1);
				temp_end_state.append(s_str, i, 1);
				temp_end_state.append(s_str, j, 1);
				temp_end_state.append(s_str, s_str.length()-1, 1);
				temp_move.append(s_str, i,1);
				temp_move.append(s_str,j,1);
				temp_move.append(s_str, s_str.length()-1,1);
			}
			if(flag)
				temp_state = new Node_State(sort(temp_start_state), sort(temp_end_state), sort(temp_move));
            else
				temp_state = new Node_State(sort(temp_end_state), sort(temp_start_state), sort(temp_move));

			temp_node = new Search_Node(temp_state, this, flag);

			if((is_dfs && !this->path_to_root_contains_state(temp_state)) || !is_dfs)
				vec_nodes.push_back(temp_node);
		}
	}
	return vec_nodes;
}

bool Node_State::isSolution(){
	if(start_side.length() == 0)
		return true;
	else
		return false;
}

void Search_Node::print_solution(void){
	cout << "Path Cost :"<< this->m_path_cost<<endl;
	cout << "Depth :"<< this->m_depth<<endl;
	Search_Node * temp;
	temp = this;
	do{
		cout << temp->m_state->start_side<<"\t";
		cout << (temp->m_action == 1 ? "( "+ temp->m_state->move + " -> )" : "( <- "+temp->m_state->move+" )")<<"\t";
		cout <<temp->m_state->end_side<<endl;
		temp = temp->m_parent_node;

	}while(temp != NULL);
}

void Search_Node::print_state(void){
    cout<< "State : "<<endl;
        this->m_state->print_state();
    cout<< "Parent State : "<<endl;
        if(this->m_parent_node!= NULL) this->m_parent_node->m_state->print_state();
    cout<< "Total Path Cost: ";
        cout<< this->m_path_cost<<endl;
    cout<< "Path Cost for last action: ";
        cout<< this->path_cost(this->m_state->move)<<endl;
    cout<< "Depth: ";
        cout<< this->m_depth<<endl<<endl;

	return;
}

void Node_State::print_state(void){
	std::cout
		<< "Start Side: "<<this->start_side
		<< "\t|\tEnd Side: "<< this->end_side
		<< "\t|\tMove: "<< this->move
		//<< " (" << this->move<< ") "
		<<std::endl;
	return;
}

bool Search_Node::path_to_root_contains_state(Node_State * n_state){
        Search_Node *temp;
        temp = this;
        do{
                if(*(temp->m_state) == *n_state){
                    return true;
                    }
                temp = temp->m_parent_node;
        }while(temp !=NULL);
        return false;
}

bool Node_State::operator==(const Node_State& node_state) const {
        return (start_side == node_state.start_side && end_side == node_state.end_side);
}

std::queue<Search_Node*> print_queue(std::queue<Search_Node*> Q){
    Search_Node* sn;
    std::queue<Search_Node*> Q2;
    int size = Q.size();
    for(int i =0; i<size; i++)
        {
            sn = Q.front();
            sn->print_state();
            Q2.push(sn);
            Q.pop();
        }
        return Q2;
}


bool operator<(const Search_Node &a, const Search_Node &b)
{
  return a.get_path_cost()< b.get_path_cost();
}

bool operator>(const Search_Node &a, const Search_Node &b)
{
  return a.get_path_cost()> b.get_path_cost();
}

bool operator==(const Search_Node &a, const Search_Node &b)
{
  return a.m_path_cost == b.m_path_cost;
}

std::string sort(std::string str){
    std::string temp;
    if(str.find("A") != std::string::npos)
        temp.append("A");
    if(str.find("B") != std::string::npos)
        temp.append("B");
    if(str.find("C") != std::string::npos)
        temp.append("C");
    if(str.find("D") != std::string::npos)
        temp.append("D");
    if(str.find("F") != std::string::npos)
        temp.append("F");
    return temp;
}

std::stack<Search_Node*> print_queue(std::stack<Search_Node*> Q){
    Search_Node* sn;
    std::stack<Search_Node*> Q2;
    int size = Q.size();
    for(int i =0; i<size; i++)
        {
            sn = Q.top();
            sn->print_state();
            Q2.push(sn);
            Q.pop();
        }
        return Q2;
}

int Search_Node::get_path_cost(void) const{
    int const ret = m_path_cost;
    return ret;
}

int Search_Node::get_heuristics(void) const{

    std::string side_with_flashlight;

    int fastest_time;

    if(m_state->start_side.find("F") != std::string::npos)
        side_with_flashlight = m_state->start_side;
    else
        side_with_flashlight = m_state->end_side;

    fastest_time = get_min_time(side_with_flashlight);

    int const ret = (m_path_cost + fastest_time);
    return ret;
}








