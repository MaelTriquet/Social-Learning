#include "node.hpp"
#include "connection.hpp"
#include <vector>
#include "history.hpp"

class Brain
{
private:
	std::vector<Node*> m_nodes = {};
	std::vector<Connection*> m_connections = {};
	static History history;

public:
	Brain();
	~Brain();

	void add_node(int layer, int act_fun_idx);
	void add_connection(int from, int to, float weight);
	void propagate();
	void mutate();
	void mutate_add_node();
	void mutate_add_connection();
	void mutate_weight();
	void back_propagate();
};
