#include "brain.hpp"
#include "const.hpp"


Brain::Brain()
{
	// Form a basic brain, empty with only input and output nodes
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		m_nodes.push_back(new Node(0, IDENTITY, i));
	}
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		m_nodes.push_back(new Node(1, RELU, i + INPUT_SIZE));
	}
	// Add bias node
	m_nodes.push_back(new Node(0, 4, INPUT_SIZE + OUTPUT_SIZE));
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		Connection* connection = new Connection(m_nodes.back(), m_nodes[INPUT_SIZE + i], RELU, Brain::history.get_connection(OUTPUT_SIZE + INPUT_SIZE, INPUT_SIZE + i));
		m_connections.push_back(connection);
		m_nodes[INPUT_SIZE + OUTPUT_SIZE]->connections.push_back(connection);
	}
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

void Brain::add_node(Connection* connection)
{
	Node* candidate = new Node(connection->from->layer + 1, 0, Brain::history.get_node(connection->history_idx));

	// if necesary, add a layer
	if (candidate->layer == connection->to->layer)
	{
		m_layers++;
		for (auto node : m_nodes)
		{
			if (node->layer >= candidate->layer)
			{
				node->layer++;
			}
		}
	}

	add_connection(connection->from, candidate, connection->weight);
	add_connection(candidate, connection->to, 1);
	add_connection(m_nodes[INPUT_SIZE + OUTPUT_SIZE], candidate, 0);
	
	connection->from->remove_connection(connection->history_idx);

	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i]->history_idx == connection->history_idx)
		{
			// remove connection from this node
			std::swap(m_connections[i], m_connections.back());
			m_connections.pop_back();
			break;
		}
	}
	delete connection;
	m_nodes.push_back(candidate);

}

void Brain::add_connection(Node* from, Node* to, float weight)
{
	m_connections.push_back(new Connection(from, to, weight, Brain::history.get_connection(from->history_idx, to->history_idx)));
	from->connections.push_back(m_connections.back());
}

void Brain::mutate()
{
}

void Brain::mutate_add_node()
{
}

void Brain::mutate_add_connection()
{
}

void Brain::mutate_weight()
{
}

void Brain::print()
{
	sort_nodes();
	for (auto node : m_ordered_nodes)
	{
		std::cout << node->history_idx << " is connected to " << std::endl;
		for (auto connection : node->connections)
		{
			std::cout << connection->to->history_idx << " with a weight of " << connection->weight << std::endl;
		}
		std::cout << std::endl;
	}
}

void Brain::test_add_node(int connection_idx)
{
	Connection* connection = m_connections[connection_idx];
	add_node(connection);
}

void Brain::test_add_connection(int from_idx, int to_idx, float weight)
{
	Node* from = m_nodes[from_idx];
	Node* to = m_nodes[to_idx];
	add_connection(from, to, weight);
}

void Brain::feedforward(float input[INPUT_SIZE], float output[OUTPUT_SIZE])
{
	reset_nodes();
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		m_nodes[i]->value = input[i];
	}
	sort_nodes();
	for (Node* node : m_ordered_nodes)
	{
		node->forward();
	}
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		output[i] = m_nodes[i + INPUT_SIZE]->value;
	}
}

void Brain::sort_nodes()
{
	m_ordered_nodes.clear();
	for (int i = 0; i < m_layers; i++)
	{
		for (auto node : m_nodes)
		{
			if (node->layer == i)
			{
				m_ordered_nodes.push_back(node);
			}
		}
	}
}

void Brain::back_propagate(float target[OUTPUT_SIZE], float lr)
{
    // 1. Compute output gradients
	for (size_t i = 0; i < OUTPUT_SIZE; ++i) {
		Node* n = m_nodes[i + INPUT_SIZE];
		float error = n->value - target[i];
		n->calculate_gradient(error);
	}

	// 2. Propagate gradients backward
	int k = m_ordered_nodes.size() - 1 - OUTPUT_SIZE;
	// k checks every hidden layer
	for (int i = m_layers - 2; i > 0; --i) {
		while (m_ordered_nodes[k]->layer == i) {
			Node* n = m_ordered_nodes[k];
		
			float downstream_sum = 0.0f;
			for (Connection* c : n->connections) {
				downstream_sum += c->weight * c->to->gradient;
			}

			n->calculate_gradient(downstream_sum);
			k--;
		}
	}
	for (Node* n : m_nodes) {
		for (Connection* c : n->connections) {
			c->delta += lr * c->to->gradient * n->value;
		}
	}

}

void Brain::update_weights()
{
	// 3. Update weights
	for (Node* n : m_nodes) {
		for (Connection* c : n->connections) {
			c->weight -= c->delta;
			c->weight = std::clamp(c->weight, -1.f, 1.f);
			c->delta = 0.f;
		}
	}
	for (Node* n : m_nodes)
	{
		n->gradient = 0.f;
	}
}

void Brain::reset_nodes()
{
	for (auto node : m_nodes)
	{
		node->value = 0;
	}
}

History Brain::history = {};
