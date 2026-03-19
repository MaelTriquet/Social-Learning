#pragma once
#include "agent.hpp"
#include <vector>
#include <iostream>
#include <algorithm>

class Population
{
public:
	std::vector<Agent*> agents = {};
	std::vector<Agent*> ordered_agents = {};
	int size = 0;
	int generation = 0;
	float best_fitness = 0;
	float total_fitness = 0;
	int time_since_upgrade = 0;
	Agent* best_agent = nullptr;


	Population(int size_):
		size(size_)
	{
		for (int i = 0; i < size; i++)
		{
			agents.push_back(new Agent());
			ordered_agents.push_back(agents.back());
		}
		best_agent = agents[0];
	}

	~Population()
	{
		for (int i = 0; i < size; i++)
		{
			delete agents[i];
		}
	}

	void evaluate()
	{
		generation++;
		total_fitness = 0;
		for (int i = 0; i < size; i++)
		{
			agents[i]->fitness_function();
			if (agents[i]->fitness > best_fitness)
			{
				best_fitness = agents[i]->fitness;
				best_agent = agents[i];
			}
			total_fitness += agents[i]->fitness;
		}
	}
	
	void group()
	{
		Random::get().shuffle(agents);
		for (Agent* a : agents)
			a->brain.max_size = best_agent->brain.m_nodes.size() + 1;
		life_cycle();

		std::vector<Agent*> group;
		float group_fitness = 0;
		Agent* teacher = nullptr;
		for (int i = 0; i < agents.size(); i++)
		{
			if (agents[i] == best_agent) teacher = agents[i];
			group_fitness += agents[i]->fitness;
			group.push_back(agents[i]);
			if (i % GROUP_SIZE == GROUP_SIZE - 1)
			{
				float a = Random::get().rand(0, group_fitness);
				for (int j = 0; j < GROUP_SIZE; j++)
				{
					if (a > group[j]->fitness && teacher == nullptr)
						a -= group[j]->fitness;
					else
					{
						if (!teacher)
							teacher = group[j];
						break;
					}
				}
				for (int j = 0; j < GROUP_SIZE; j++)
				{
					if (group[j] == teacher) continue;
					teacher->show(group[j]);
					if (Random::get().rand(0, 1) < 0.2)
						teacher->teach(group[j]);
					group[j]->update_score();
				}
				teacher = nullptr;
				group_fitness = 0;
				group.clear();
			}
		}
	}

	void life_cycle()
	{
		float a = Random::get().rand();
		if (a < 0.05)
			return;
		int max_staleness = 0;
		for (int i = 0; i < size; i++)
		{
			if (agents[i]->staleness > max_staleness)
			{
				std::swap(agents[i], agents.back());
				max_staleness = agents[i]->staleness;
			}
		}
		if (max_staleness < 300)
			return;
		if (agents.back() == best_agent) return;
		Agent* agent = agents.back();
		int i;
		for (i = 0; i < size; i++)
		{
			if (ordered_agents[i] == agent)
				break;
		}
		delete agents.back();
		agents.pop_back();
		agents.push_back(new Agent());
		ordered_agents[i] = agents.back();
	}

	void update()
	{
		ConceptArchive::get().compute_centroid();
		evaluate();
		group();
		// std::cout << "Best fitness: " << best_fitness << std::endl;
		// std::cout << "Average fitness: " << total_fitness / size << std::endl;
		int max_nodes = 0;
		for (Agent* a : agents)
		{
			if (a->fitness == best_fitness) continue;
			a->brain.age++;
			// if (a->brain.age % 10 == 0)
			// 	a->brain.weight_exploration();
			if (a->brain.m_nodes.size() > max_nodes)
				max_nodes = a->brain.m_nodes.size();
		}
		// if (time_since_upgrade > 5000)
		// {
		// 	std::cout << "Simulation Over" << std::endl;
		// 	std::cout << "Best agent: " << best_agent->fitness << std::endl;
		// 	float inputs[4][INPUT_SIZE] = {{0,0},{0,1},{1,0},{1,1}};
		// 	for (int i = 0; i < 4; i++)
		// 	{
		// 		best_agent->brain.feedforward(inputs[i], best_agent->output);
		// 		std::cout << inputs[i][0] << " " << inputs[i][1] << " => " << best_agent->output[0] << std::endl;
		// 	}
		// 	exit(0);
		// }
		std::cout << "Max score: " << best_fitness << std::endl;
		std::cout << "Max size: " << best_agent->brain.m_nodes.size() << std::endl;
		std::cout << "Generation: " << generation << std::endl;
		// std::cout << "Max nodes: " << max_nodes << std::endl;
		// std::cout << "Best agent: \n" << best_agent->fitness << std::endl;
		// float inputs[4][INPUT_SIZE] = {{0,0},{0,1},{1,0},{1,1}};
		// for (int i = 0; i < 4; i++)
		// {
		// 	best_agent->brain.feedforward(inputs[i], best_agent->output);
		// 	std::cout << inputs[i][0] << " " << inputs[i][1] << " => " << best_agent->output[0] << std::endl;
		// }

		// if (max_nodes > 30)
		// 	exit(0);
	}

	void end()
	{
		std::cout << "Simulation Over" << std::endl;
		std::cout << "Best agent: " << best_agent->fitness << std::endl;
		float inputs[4][INPUT_SIZE] = {{0,0},{0,1},{1,0},{1,1}};
		for (int i = 0; i < 4; i++)
		{
			best_agent->brain.feedforward(inputs[i], best_agent->output);
			std::cout << inputs[i][0] << " " << inputs[i][1] << " => " << best_agent->output[0] << std::endl;
		}
		std::cout << "Generation: " << generation << std::endl;
	}

};
