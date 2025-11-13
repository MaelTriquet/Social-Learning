#include "brain.hpp"


Brain::Brain()
{
	// Form a basic brain, empty with only input and output nodes
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		m_nodes.push_back(new Node(0, 3, i));
	}
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		m_nodes.push_back(new Node(1, 2, i + INPUT_SIZE));
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
	Node* candidate = new Node(connection->from->layer + 1, 2, Brain::history.get_node(connection->history_idx));

	// if necesary, add a layer
	if (candidate->layer == connection->to->layer)
	{
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
	
	connection->from->remove_connection(connection->history_idx);

	for (int i = 0; i < (int)m_connections.size(); i++)
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

void Brain::propagate()
{
	for (auto node : m_nodes)
	{
		node->forward();
	}
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
	for (auto node : m_nodes)
	{
		std::cout << node->history_idx << ": " << node->connections.size() << std::endl;
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

History Brain::history = {};
