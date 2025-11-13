#include <vector>

struct History_Connection
{
	int from;
	int to;
	History_Connection(int from_, int to_):
		from(from_),
		to(to_)
	{
	}
};

class History
{
private:
	std::vector<int> m_nodes = {};
	std::vector<History_Connection> m_connections = {};

public:
	History();
	~History();

	int get_node(int from);
	int get_connection(int from, int to);
};
