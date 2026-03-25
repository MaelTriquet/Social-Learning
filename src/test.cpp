#include "test.hpp"
#include "renderer/renderer.hpp"
#include <unordered_map>
#include "global.hpp"

void draw(Brain& brain, int width, int height, int corner_x, int corner_y);
void create_xor_brain(Brain& brain)
{
	brain.max_size = 6;
	brain.add_node(brain.m_connections[0]);
	brain.add_node(brain.m_connections[1]);
	brain.add_connection(brain.m_nodes[0], brain.m_nodes[5], Random::get().rand(-1, 1));
	brain.add_connection(brain.m_nodes[1], brain.m_nodes[4], Random::get().rand(-1, 1));
}

void test_xor(bool verbose, int tries)
{
	Renderer::init(800, 800);
	std::cout << "Test: XOR, with given right architecture, with backprop from all batches right" << std::endl;
	int amount_success = 0;
	float output[OUTPUT_SIZE];
	for (int _ = 0; _ < tries; _++)
	{
		bool success = false;
		Random::get().rand();
		Brain brain;
		create_xor_brain(brain);
		if (verbose)
			brain.print();
		float inputs[4][INPUT_SIZE] = {{0,0},{0,1},{1,0},{1,1}};
		float targets[4][OUTPUT_SIZE] = {{0},{1},{1},{0}};

		// for (int epoch = 0; epoch < 10000; epoch++)
		bool succeeded;
		int frames = 0;
		while (Renderer::loop())
		{
			// sf::sleep(sf::seconds(0.1));
			// if (!Renderer::loop()) break;
			if (frames++ > 10000)
				break;
				// brain.weight_exploration();
			Renderer::background();
			draw(brain, Renderer::width, Renderer::height, 0, 0);
			for (int i = 0; i < 4; i++)
				brain.backpropagate(inputs[i], targets[i]);
			brain.update_weights(0.3, 4);
			// std::cout << "Brain output:" << std::endl;
			succeeded = true;
			for (int i = 0; i < 4; i++)
			{
				brain.feedforward(inputs[i], output);
				// std::cout << inputs[i][0] << inputs[i][1] << " => " << output[0] << std::endl;
				if (output[0] - targets[i][0] > 0.3f || output[0] - targets[i][0] < -0.3f)
					succeeded = false;
			}
			if (succeeded)
			{
				success = true;
				break;
			}
		}
		std::cout << "Number of frames: " << frames << std::endl;

		if (verbose)
			brain.print();

		float output[OUTPUT_SIZE];
		for (int i = 0; i < 4; i++)
		{
			brain.feedforward(inputs[i], output);
			int guess = output[0] > 0.5 ? 1 : 0;
			if (guess != targets[i][0])
				success = false;
			if (verbose)
				std::cout << inputs[i][0] << inputs[i][1] << " => " << output[0] << std::endl;
		}
		if (success)
			amount_success++;
	}
	std::cout << "Success rate: " << amount_success << "/" << tries << std::endl;
}

void draw(Brain& brain, int width = Renderer::width, int height = Renderer::height, int corner_x = 0, int corner_y = 0)
{
	float radius = 10;
	std::vector<int> layers_amount;
	for (Node* n : brain.m_ordered_nodes)
	{
		if (n->depth_index >= layers_amount.size())
			layers_amount.push_back(0);
		layers_amount[n->depth_index]++;
	}
	int count_layer = -1;
	int layer = 0;
	std::unordered_map<Node*, sf::Vector2f> coords;
	for (int i = 0; i < brain.m_ordered_nodes.size(); i++)
	{
		Node* n = brain.m_ordered_nodes[i];
		if (n->depth_index != layer)
		{
			layer = n->depth_index;
			count_layer = -1;
		}
		count_layer++;
		float x = (float)n->depth_index / (float)layers_amount.size() * width;
		float y = (float)count_layer / (float)layers_amount[layer] * height;
		sf::Vector2f offset(1.0f / (float)layers_amount.size() * width / 2 + corner_x, 1.0f / (float)layers_amount[layer] * height / 2 + corner_y);
		coords[n] = sf::Vector2f(x + offset.x, y + offset.y);



	}
	Renderer::center_mode = CENTER;
	
	for (Node* n : brain.m_nodes)
	{
		for (Connection* c : n->connections)
		{
			Renderer::stroke_color = c->weight > 0 ? sf::Color::Red : sf::Color::Blue;
			Renderer::stroke_width = std::clamp((float)std::abs(c->weight) * 5.0f, 0.0f, 5.0f);
			Renderer::line(coords[n], coords[c->from]);
		}
	}
	
	Renderer::fill = true;
	Renderer::stroke = true;
	Renderer::stroke_color = sf::Color::White;
	Renderer::fill_color = sf::Color(50, 50, 50);
	Renderer::stroke_width = 2.0f;
	for (Node* n : brain.m_ordered_nodes)
	{
		Renderer::circle(coords[n], radius);
	}
}

void test_population(bool verbose)
{
	bool show = false;
	if (show)
	{
		Renderer::init(800, 800);
		Population population(100);
		while (Renderer::loop())
		{
			// sf::sleep(sf::seconds(1));
			Renderer::background();
			draw(population.best_agent->brain, Renderer::width, Renderer::height, 0, 0);
			population.update();
			// if (population.best_fitness > .7)
			// 	break;
		}
		std::sort(population.agents.begin(), population.agents.end(), [](Agent* a, Agent* b) { return a->fitness > b->fitness; });
		for (int i = 0; i < 10; i++)
		{
			population.best_agent = population.agents[i];
			population.end();
			while (Renderer::loop())
			{
				// sf::sleep(sf::seconds(1));
				Renderer::background();
				draw(population.agents[i]->brain, Renderer::width, Renderer::height, 0, 0);
				// if (population.best_fitness > .7)
				// 	break;
			}
		}
		population.end();
	}
	else
	{
		Population population(100);
		int count = 0;
		while (count++ < 10000)
		{
			population.update();
		}
		population.end();
	}
}

void test_small_population(bool verbose)
{
	Renderer::init(800, 800);
	int n = 2;
	int m = 1;
	Population population(n*m);
	int count = 0;
	create_xor_brain(population.agents[0]->brain);
	while (Renderer::loop())
	{
		// sf::sleep(sf::seconds(1));
		Renderer::background();
		// std::sort(population.agents.begin(), population.agents.end(), [](Agent* a, Agent* b) { return a->fitness > b->fitness; });
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				if (population.ordered_agents[i*m + j] == population.best_agent)
				{
					Renderer::center_mode = CORNER;
					Renderer::fill_color = sf::Color(130, 130, 130);
					Renderer::rectangle(sf::Vector2f(i*Renderer::width/n, j*Renderer::height/m), Renderer::width/n, Renderer::height/m);
					Renderer::center_mode = CENTER;
				}
				draw(population.ordered_agents[i*m + j]->brain, Renderer::width/n, Renderer::height/m, i*Renderer::width/n, j*Renderer::height/m);
			}
		}
		population.update();
		// if (population.best_fitness > .9)
		// 	break;
		if (count++ > 10000)
			break;
	}
	// std::sort(population.agents.begin(), population.agents.end(), [](Agent* a, Agent* b) { return a->fitness > b->fitness; });
	// for (int i = 0; i < 4; i++)
	// {
	// 	population.best_agent = population.agents[i];
	// 	population.end();
	// 	while (Renderer::loop())
	// 	{
	// 		// sf::sleep(sf::seconds(1));
	// 		Renderer::background();
	// 		draw(population.agents[i]->brain);
	// 		// if (population.best_fitness > .7)
	// 		// 	break;
	// 	}
	// }
	population.end();
}

