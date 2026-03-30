#pragma once
#include "agent.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include "group.hpp"
#include "threadpool.hpp"
#include <unordered_map>

class Population
{
public:
	std::vector<Agent*> agents = {};
	std::vector<Agent*> ordered_agents = {};
	std::vector<Group> groups = {};
	// std::vector<Agent> books = {};
	std::unordered_map<Agent*, Agent*> books = {};
	float life_expectancy = 500;
	int size = 0;
	int generation = 0;
	float best_fitness = 0;
	float total_fitness = 0;
	int time_since_upgrade = 0;
	Agent* best_agent = nullptr;
	ThreadPool m_pool;
	float group_threshold = GROUP_THRESHOLD;
	float group_adjustment = 0.00f;


	Population(int size_):
		size(size_)
	{
		for (int i = 0; i < size; i++)
		{
			agents.push_back(new Agent(life_expectancy));
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
		for (auto i = books.begin(); i != books.end(); i++)
		{
			delete i->second;
		}
	}

	// void evaluate()
	// {
	// 	generation++;
	// 	total_fitness = 0;
	// 	bool best_fitness_changed = false;
	// 	for (Agent* a : agents)
	// 	{
	// 		a->fitness_function();
	// 		if (a->fitness > best_fitness)
	// 		{
	// 			best_agent->brain.best = false;
	// 			best_fitness_changed = true;
	// 			best_fitness = a->fitness;
	// 			best_agent = a;
	// 			best_agent->brain.best = true;
	// 			std::cout << "Best fitness: " << best_fitness << " at generation " << generation << std::endl;
	// 		}
	// 		total_fitness += a->fitness;
	// 	}
	// }

	void evaluate()
	{
		generation++;
		total_fitness = 0;

		// Parallel: fitness evaluations are fully independent
		std::vector<std::future<void>> futures;
		futures.reserve(agents.size());
		for (Agent* a : agents)
			futures.push_back(m_pool.submit([a]() { a->fitness_function(); }));
		for (auto& f : futures)
			f.wait();

		// Serial: aggregate results — fast, no point parallelising
		bool best_fitness_changed = false;
		for (Agent* a : agents)
		{
			a->age++;
			total_fitness += a->fitness;
			if (a->fitness > best_fitness)
			{
				best_agent->brain.best = false;
				best_fitness_changed = true;
				best_fitness = a->fitness;
				best_agent = a;
				best_agent->brain.best = true;
				std::cout << "Best fitness: " << best_fitness
						  << " at generation " << generation << std::endl;

				if (books.count(best_agent))
					delete books[best_agent];
				books[best_agent] = new Agent(*best_agent);
				for (Node* n : books[best_agent]->brain.m_nodes)
					ConceptArchive::get().update_cluster(n, n->depth_index);
			}
		}
		if (best_fitness_changed)
			for (Agent* a : agents)
				a->witness_improvement = true;
	}
	
	void group()
	{
		for (Agent* a : agents)
			a->brain.max_size = best_agent->brain.m_nodes.size() + 1;

		for (Group& g : groups)
		{
			if (g.agents.size() == 1)
				continue;
			for (Agent* a : g.agents)
			{
				float inside = Random::get().rand(0, 1) < INSIDE_GROUP_PROBABILITY;
				Agent* other = nullptr;
				if (inside)
				{
					do
					{
						other = g.agents[Random::get().randint(0, g.agents.size())];
					} while (other == a);
				} else {
					Group& other_group = groups[Random::get().randint(0, groups.size())];
					do {
						other = other_group.agents[Random::get().randint(0, other_group.agents.size())];
					} while (other == a);
				}

				if (Random::get().rand(0, 1) < 0.1)
				{
					float random = Random::get().rand(0, a->fitness + other->fitness);
					if (a == best_agent)
						a->teach(other);
					else if (other == best_agent)
						other->teach(a);
					else if (random < a->fitness)
						a->teach(other);
					else
						other->teach(a);
				}
				else
				{
					if (Random::get().rand(0, 1) < 1)
						a->debate(other);
					else	
						a->show(other);
				}
			}
		}

		// for (auto i = books.begin(); i != books.end(); i++)
		// {
		// 	Agent* b = i->second;
		// 	int index = Random::get().randint(0, agents.size());
		// 	if (Random::get().rand(0, 1) < 0.1 && agents[index] != best_agent)
		// 		b->teach(agents[index]);
		// 	else
		// 		b->debate(agents[index]);
		// }
		Agent* b = books[best_agent];
		int index = Random::get().randint(0, agents.size());
		if (Random::get().rand(0, 1) < 0.1 && agents[index] != best_agent)
			b->teach(agents[index]);
		else
			b->debate(agents[index]);

	}

	void life_cycle()
	{
		return;
		int index = Random::get().randint(0, agents.size());
		if (agents[index]->staleness < agents[index]->life_expectancy) return;
		// if (Random::get().rand(0, 1) < 0.5) return;
		bool best = false;
		for (Group& g : groups)
			if (g.getBestAgent() == agents[index])
			{
				best = true;
				break;
			}
		// if (best) return;
		Agent* agent = agents[index];
		if (agent == best_agent) return;
		// if (agent->debate_alignment() > Random::get().rand(0, 0.1)) return;
		int i;
		for (i = 0; i < size; i++)
		{
			if (ordered_agents[i] == agent)
				break;
		}
		if (agent->fitness > .9 * best_fitness && !books.count(agent))
		{
			books[agent] = new Agent(*agent);
			for (Node* n : books[agent]->brain.m_nodes)
				ConceptArchive::get().update_cluster(n, n->depth_index);
		}
		if (!agent->witness_improvement)
		{
			life_expectancy *= 1.01f;
			for (Agent* a : agents)
				a->witness_improvement = true;
		}
		delete agent;
		agents[index] = new Agent(life_expectancy);
		ordered_agents[i] = agents[index];
	}

	int find_group_index(Agent* a)
	{
		int n = (int)groups.size(); // snapshot size before any potential reallocation
		std::vector<std::future<bool>> futures;
		futures.reserve(n);

		for (int i = 0; i < n; i++)
			futures.push_back(m_pool.submit([this, i, a]()
			{
				return groups[i].isInGroup(a, group_threshold);
			}));

		for (int i = 0; i < n; i++)
			if (futures[i].get())
				return i;

		return -1;
	}

	void update()
	{
		life_cycle();

		for (Group& g : groups)
			g.agents.clear();
		// Parallelized version
		// for (Agent* a : agents)
		// {
		// 	bool in_group = false;
		// 	int index = find_group_index(a);
		// 	if (index != -1)
		// 		groups[index].agents.push_back(a);
		// 	else
		// 		groups.push_back(Group(a));
		//
		// }
		for (Agent* a : agents) // sequential version
		{
			bool in_group = false;
			for (Group& g : groups)
				if (g.isInGroup(a, group_threshold))
				{
					in_group = true;
					g.agents.push_back(a);
					break;
				}
			if (!in_group)
				groups.push_back(Group(a));
		}
		if (groups.size() * groups.size() > agents.size())
			group_threshold += group_adjustment;
		else
			group_threshold -= group_adjustment;
		for (int i = groups.size() - 1; i >= 0; i--)
		{
			Group& g = groups[i];
			if (g.agents.size() == 0)
			{
				std::swap(g, groups.back());
				groups.pop_back();
			}
		}
		// std::cout << "Groups: " << groups.size() << std::endl;
		ConceptArchive::get().compute_centroid();
		// std::cout << "Clusters: " << ConceptArchive::get().m_clusters.size() << " at generation " << generation << std::endl;
		// std::cout << "Books: " << books.size() << std::endl;
		evaluate();
		group();
	}

	void end()
	{
		best_agent = books[best_agent];
		std::cout << "Simulation Over" << std::endl;
		std::cout << "Best agent's fitness: " << best_agent->fitness << std::endl;
		int n_cases = 1 << INPUT_SIZE; // 2^INPUT_SIZE

		float input[INPUT_SIZE] = {0};
		float output[OUTPUT_SIZE] = {0};
		for (int i = 0; i < n_cases; i++)
		{
			// Generate input from binary representation of i
			int ones = 0;
			for (int j = 0; j < INPUT_SIZE; j++)
			{
				input[j] = (i >> j) & 1;
				ones += input[j];
			}
			float target = ones % 2 == 1 ? 1.0f : 0.0f;

			best_agent->brain.feedforward(input, output);
			for (int j = 0; j < INPUT_SIZE; j++)
				std::cout << input[j] << " ";
			std::cout << "=> " << output[0] << std::endl;
		}

		// Best agent per group
		// std::cout << "Groups: " << groups.size() << std::endl;
		// for (Group& g : groups)
		// {
		// 	std::cout << "Group's best agent fitness: " << g.getBestAgent()->fitness << std::endl;
		// 	Agent* a = g.getBestAgent();
		// 	std::cout << a->fitness << std::endl;
		// 	for (int i = 0; i < 4; i++)
		// 	{
		// 		a->brain.feedforward(inputs[i], a->output);
		// 		std::cout << inputs[i][0] << " " << inputs[i][1] << " => " << a->output[0] << std::endl;
		// 	}
		// }



		// All agents in best group
		// Group& best_group = groups[0];
		// for (int i = 0; i < groups.size(); i++)
		// 	if (groups[i].getBestAgent() == best_agent)
		// 		best_group = groups[i];
		// std::cout << "Best group: " << std::endl;
		// for (Agent* a : best_group.agents)
		// {
		// 	std::cout << a->fitness << std::endl;
		// 	for (int i = 0; i < 4; i++)
		// 	{
		// 		a->brain.feedforward(inputs[i], a->output);
		// 		std::cout << inputs[i][0] << " " << inputs[i][1] << " => " << a->output[0] << std::endl;
		// 	}
		// }
	}
};
