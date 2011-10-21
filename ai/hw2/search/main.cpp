#include "main.h"//
#include <stdio.h>

//using namespace std;
//stack<Search_Node*> pq;
bool is_dfs = false;
struct compare_path_costs: public std::binary_function<Search_Node*, Search_Node*, bool>
{
    bool operator()(const Search_Node* lhs, const Search_Node* rhs) const
    {
        return lhs->get_path_cost() > rhs->get_path_cost();
    }
};
struct compare_heuristics: public std::binary_function<Search_Node*, Search_Node*, bool>
{
    bool operator()(const Search_Node* lhs, const Search_Node* rhs) const
    {
        return lhs->get_heuristics() > rhs->get_heuristics();
    }
};

std::vector<Search_Node*> search_agent_bfs(string start_side, string end_side){
	Node_State * initial_state;
	Search_Node * temp;
	Search_Node * leaf;
	queue<Search_Node*> pq;
	vector<Search_Node*> vec;
	initial_state = new Node_State(start_side, end_side, "");
	temp = new Search_Node(initial_state, NULL, -1);
	pq.push(temp);
	do{

		if(pq.empty()) {
			cout << "Queue is Empty. Terminating" <<endl;
			vec.clear();
			return vec;//(std::vector<Search_Node*> )NULL;
		}
		leaf = pq.front();
		if (leaf->is_goal()){
			cout<<"Solution Found using BFS"<<endl;
			//leaf->print_solution();
			return leaf->get_solution();
		}
		else{
		    leaf->print_state();
			vec.clear();
			vec = leaf->expand();
			pq.pop();
			for(int i =0; i < vec.size(); i++){
				temp = vec[i];
				pq.push(temp);
			}
		}
	}while(!pq.empty());
    vec.clear();
	return vec;
}

std::vector<Search_Node*> search_agent_uc(string start_side, string end_side){
	Node_State * initial_state;
	Search_Node * temp;
	Search_Node * leaf;
	priority_queue<Search_Node*, vector<Search_Node*>, compare_path_costs> pq;

	vector<Search_Node*> vec;
	initial_state = new Node_State(start_side, end_side, "");
	temp = new Search_Node(initial_state, NULL, -1);
	pq.push(temp);
	do{

		if(pq.empty()) {
			cout << "Queue is Empty. Terminating" <<endl;
			vec.clear();
			return vec;
		}
		leaf = pq.top();
		if (leaf->is_goal()){
			cout<<"Solution Found using UCS"<<endl;
			//leaf->print_solution();
			return leaf->get_solution();
		}
		else{
		    leaf->print_state();
			vec.clear();
			vec = leaf->expand();
			pq.pop();
			for(int i =0; i < vec.size(); i++){
				temp = vec[i];
				//temp->print_state();
				pq.push(temp);
			}

		}
		//pq = print_queue(pq);
		//getchar();
	}while(!pq.empty());
	vec.clear();
	return vec;
}

std::vector<Search_Node*> search_agent_as(string start_side, string end_side){
	Node_State * initial_state;
	Search_Node * temp;
	Search_Node * leaf;
	priority_queue<Search_Node*, vector<Search_Node*>, compare_heuristics> pq;

	vector<Search_Node*> vec;
	initial_state = new Node_State(start_side, end_side, "");
	temp = new Search_Node(initial_state, NULL, -1);
	pq.push(temp);
	do{

		if(pq.empty()) {
			cout << "Queue is Empty. Terminating" <<endl;
			vec.clear();
			return vec;
		}
		leaf = pq.top();
		if (leaf->is_goal()){
			cout<<"Solution Found using A* Search"<<endl;
			//leaf->print_solution();
			return leaf->get_solution();
		}
		else{
		    leaf->print_state();
			vec.clear();
			vec = leaf->expand();
			pq.pop();
			for(int i =0; i < vec.size(); i++){
				temp = vec[i];
				//temp->print_state();
				pq.push(temp);
			}

		}
		//pq = print_queue(pq);
		//getchar();
	}while(!pq.empty());
	vec.clear();
	return vec;
}



std::vector<Search_Node*> search_agent_dfs(string start_side, string end_side){
	Node_State * initial_state;
	Node_State * initial_state1;
	Search_Node * temp;
	Search_Node * temp1;
	Search_Node * leaf;
	stack<Search_Node*> pq;
	vector<Search_Node*> vec;

	initial_state = new Node_State(start_side, end_side, "");
	temp = new Search_Node(initial_state, NULL, -1);

	pq.push(temp);
	do{

		if(pq.empty()) {
			cout << "Queue is Empty. Terminating" <<endl;
			vec.clear();
			return vec;
		}
		leaf = pq.top();
//		cout<<"leaf popped"<<endl;
	//	leaf->print_state();
		if (leaf->is_goal()){
			cout<<"Solution Found using DFS"<<endl;
			//leaf->print_solution();
			return leaf->get_solution();
		}
		else{
		    leaf->print_state();
			vec.clear();
			vec = leaf->expand();
			pq.pop();
			for(int i =0; i < vec.size(); i++){
				temp = vec[i];
				pq.push(temp);
			}
		}

	}while(!pq.empty());
	vec.clear();
	return vec;
}

int main(int argc, char* argv[]){

	string start_side, end_side, search_choice;
    vector <Search_Node*> list_of_actions;
	start_side = "ABCDF";
	end_side = "";
	is_dfs= false;


    if(argc < 3){
        cout<<"USAGE ERROR:\n Needs to be of the form\n\n ./search <search-choice> <start-side> <end-side>\n"<<endl
        <<"Example: \n\n./search bfs ABCDF\n./search dfs ABC DF"<<endl;
        exit(0);
    }
    if(argc == 3){
        search_choice = argv[1];
        start_side = argv[2];
        end_side == "";
    }else{
        search_choice = argv[1];
        start_side = argv[2];
        end_side = argv[3];
    }
    start_side = sort(start_side);
    end_side = sort(end_side);
    //cout << search_choice;

    if(search_choice == "bfs")
        { is_dfs = false; list_of_actions = search_agent_bfs( start_side, end_side);}
    else if(search_choice == "dfs")
        { is_dfs = true; list_of_actions = search_agent_dfs( start_side, end_side); }
    else if(search_choice == "ucs")
        { is_dfs = false; list_of_actions = search_agent_uc( start_side, end_side); }
    else if(search_choice == "ass")
        { is_dfs = false; list_of_actions = search_agent_as( start_side, end_side); }
    else
        { cout<< "Invalid Search choice\n\n"; exit(0);}

    // report the list of actions
    print_vector(list_of_actions);
	return 1;
}
