# include "brain.hpp"

int main()
{
	Brain brain;
	brain.add_node(1, 0);
	brain.add_node(2, 0);
	brain.add_node(3, 0);
	brain.add_node(4, 0);
	brain.add_node(5, 0);

	brain.add_connection(1, 2, 0.5);
	brain.add_connection(2, 3, 0.5);
	brain.add_connection(3, 4, 0.5);
	brain.add_connection(4, 5, 0.5);

	brain.propagate();

	return 0;
}
