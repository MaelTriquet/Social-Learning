#pragma once
#include <algorithm>
#include "node.hpp"
#include "connection.hpp"
#include <vector>
#include "history.hpp"
#include "const.hpp"
#include <iostream>
#include "random.hpp"

class Brain
{
private:
	std::vector<Node*> m_nodes = {};
	std::vector<Node*> m_ordered_nodes = {};
	std::vector<Connection*> m_connections = {};
	static History history;
	static const Random random;
	int m_layers = 2;

public:
	Brain();
	~Brain();

	void add_node(Connection* connection);
	void add_connection(Node* from, Node* to, float weight);
	void mutate();
	void mutate_add_node();
	void mutate_add_connection();
	void mutate_weight();
	void print();
	void back_propagate(float target[OUTPUT_SIZE], float lr);
	void feedforward(float input[INPUT_SIZE], float output[OUTPUT_SIZE]);
	void sort_nodes();
	void reset_nodes();
	void update_weights();
	void learn_from_example(float input[INPUT_SIZE], float target[OUTPUT_SIZE]);
	bool fully_connected();
	
	void test_add_node(int connection_idx);
	void test_add_connection(int from_idx, int to_idx, float weight);

};
