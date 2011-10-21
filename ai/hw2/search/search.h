#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <stdlib.h>

extern bool is_dfs;

class Node_State{
  public:
	std::string start_side, end_side;
	std::string move;
	Node_State (std::string s1, std::string s2, std::string move);
	Node_State(void);
	bool isSolution();
	void enumerate(bool flag);
	bool operator==(const Node_State& node_state) const;
	void print_state(void);
};
class Search_Node{
  public:
	Node_State * m_state;
	Search_Node * m_parent_node;
	int m_action;
	int m_path_cost;
	int m_depth;

	Search_Node(Node_State * n_state, Search_Node * parent_node, int action);
	Search_Node(void);
	bool is_goal();
	std::vector<Search_Node *> expand();
	void print_solution(void);
	std::vector <Search_Node*> get_solution(void);
	std::vector<Search_Node  *> enumerate_states (bool flag);
	int path_cost(std::string move);
	int get_max_time(std::string move);
	bool path_to_root_contains_state(Node_State*);
	void print_state(void);
	int get_path_cost(void) const;
	int get_heuristics(void) const;
};
bool operator<(const Search_Node &a, const Search_Node &b);
bool operator==(const Search_Node &a, const Search_Node &b);
bool operator>(const Search_Node &a, const Search_Node &b);
std::string sort(std::string str);
std::queue<Search_Node*> print_queue(std::queue<Search_Node*> Q);
std::stack<Search_Node*> print_queue(std::stack<Search_Node*> Q);
void print_vector(std::vector <Search_Node*> vec);
//std::priority_queue<Search_Node*> print_queue(std::priority_queue<Search_Node*> Q);
//std::priority_queue<Search_Node*> print_queue(std::priority_queue<Search_Node*, std::vector<Search_Node*>,greater<std::vector<Search_Node*>::value_type> > Q);
//struct DereferenceCompareNode : public std::binary_function<Search_Node*, Search_Node*, bool>{
 //   bool operator()(const Search_Node* lhs, const Search_Node* rhs) const;
//};
using namespace std;
