#include "brain.hpp"
#include <iostream>

Brain::Brain()
{
}

Brain::~Brain()
{
	for (auto node : m_nodes)
	{
		delete node;
	}
	for (auto connection : m_connections)
	{
		delete connection;
	}
}

void Brain::add_node(int layer, int act_fun_idx)
{
	m_nodes.push_back(new Node(layer, act_fun_idx));
	std::cout << "Added node with layer " << layer << " and activation function " << act_fun_idx << std::endl;
}

void Brain::add_connection(int from, int to, float weight)
{
	m_connections.push_back(new Connection(m_nodes[from], m_nodes[to], weight));
	std::cout << "Added connection from " << from << " to " << to << " with weight " << weight << std::endl;
}

void Brain::propagate()
{
	for (auto node : m_nodes)
	{
		node->forward();
	}
}

void Brain::mutate()
{
	mutate_add_node();
	mutate_add_connection();
	mutate_weight();
}

void Brain::mutate_add_node()
{
	int node_idx = rand() % m_nodes.size();
	int layer = rand() % 4;
	int act_fun_idx = rand() % 4;
	add_node(layer, act_fun_idx);
}

void Brain::mutate_add_connection()
{
	int from = rand() % m_nodes.size();
	int to = rand() % m_nodes.size();
	float weight = rand() % 100 / 100.0;
	add_connection(from, to, weight);
}

void Brain::mutate_weight()
{
	int connection_idx = rand() % m_connections.size();
	float weight = rand() % 100 / 100.0;
	m_connections[connection_idx]->weight = weight;
}

History Brain::history = {};
