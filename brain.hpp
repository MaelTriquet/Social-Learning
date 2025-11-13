#include "node.hpp"
#include "connection.hpp"
#include <vector>
#include "history.hpp"
#include "const.hpp"
#include <iostream>

class Brain
{
private:
	std::vector<Node*> m_nodes = {};
	std::vector<Connection*> m_connections = {};
	static History history;

public:
	Brain();
	~Brain();

	void add_node(Connection* connection);
	void add_connection(Node* from, Node* to, float weight);
	void propagate();
	void mutate();
	void mutate_add_node();
	void mutate_add_connection();
	void mutate_weight();
	void print();
	
	void test_add_node(int connection_idx);
	void test_add_connection(int from_idx, int to_idx, float weight);
};
