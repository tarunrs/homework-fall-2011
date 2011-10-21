#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>

class Node_State{
  public:
	int board[4][4];
	Node_State (int board[4][4]);
	Node_State(int state[4][4], int pos, int val);
	Node_State(void);
	bool isSolution();
	void enumerate(bool flag);
	void print_state(void);
};
class Search_Node{
  public:
	Node_State * m_state;
	Search_Node * m_parent_node;
	int m_depth;
    int m_action;
	Search_Node(Node_State * n_state, Search_Node * parent_node, int action);
	Search_Node(void);
	bool is_goal();
	std::vector<Search_Node *> expand();
	void print_solution(void);
	int find_next_empty_position(void);
	bool can_fill(int pos, int val);
};


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s );
void read_file(std::string filename, int board[4][4]);
using namespace std;
