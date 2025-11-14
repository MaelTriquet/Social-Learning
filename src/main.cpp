# include "brain.hpp"

int main()
{
	Brain brain;
	brain.test_add_connection(0, 2, Random::get().rand(-1, 1));
	brain.test_add_connection(1, 2, Random::get().rand(-1, 1));
	brain.test_add_node(1);
	brain.test_add_node(2);
	brain.test_add_connection(1, 4, Random::get().rand(-1, 1));
	brain.test_add_connection(0, 5, Random::get().rand(-1, 1));
	brain.print();
	float input[INPUT_SIZE] = {.1, .5};
	float output[OUTPUT_SIZE];
	float target[OUTPUT_SIZE] = {.8};
	for (int i = 0; i < 1000; i++)
	{
		for (int i = 0; i < 10; i++)
		{
			input[0] = Random::get().randint(2);
			input[1] = Random::get().randint(2);
			target[0] = input[0] * input[1];
			if (input[0] == 0 && input[1] == 0)
			{
				target[0] = 1;
			}
			brain.feedforward(input, output);
			brain.back_propagate(target, 0.01f);
			std::cout << input[0] << " " << input[1] << " " << target[0] << " " << output[0] << std::endl;
		}
		brain.update_weights();
	}
	brain.print();
	return 0;
}
