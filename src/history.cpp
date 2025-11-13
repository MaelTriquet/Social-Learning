#include "history.hpp"

History::History()
{
}

History::~History()
{
}

int History::get_node(int from)
{
	for (int i = 0; i < (int)m_nodes.size(); i++)
	{
		if (m_nodes[i] == from)
		{
			return i;
		}
	}
	m_nodes.push_back(from);
	return m_nodes.size() - 1 + INPUT_SIZE + OUTPUT_SIZE;
}

int History::get_connection(int from, int to)
{
	for (int i = 0; i < (int)m_connections.size(); i++)
	{
		if (m_connections[i].from == from && m_connections[i].to == to)
		{
			return i;
		}
	}
	m_connections.emplace_back(from, to);
	return m_connections.size() - 1;
}
