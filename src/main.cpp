# include "brain.hpp"

int main()
{
	Brain brain;
	brain.print();
	brain.test_add_connection(0, 2, .5);
	brain.test_add_connection(1, 2, .5);
	brain.test_add_node(0);
	brain.print();
	return 0;
}
