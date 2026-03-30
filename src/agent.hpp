#pragma once
#include "brain.hpp"
#include "random.hpp"
#include <vector>
#include <iostream>
#include "const.hpp"
#include <Eigen/Dense>
#include "concept_archive.hpp"

class Agent
{
public:
	Brain brain;
	float fitness = 0;
	bool book = false;
	const Agent* writer;
	float input[INPUT_SIZE] = {0};
	float output[OUTPUT_SIZE] = {0};
	float best_fitness = 0;
	int staleness = 0;
	float score = 0;
	float originality_personality = 0.f;
	float community_personality = 0.f;
	float novelty_personality = 0.f;
	float utility_personality = 0.f;
	float life_expectancy = 0;
	float age = 0;
	bool witness_improvement = false;
	float personality = Random::get().rand();
	ENCODING chain[CHAIN_SIZE];
	
	Agent(int life_expectancy_):
		brain(),
		writer(this),
		life_expectancy(life_expectancy_)
	{
		for (int j = 0; j < CHAIN_SIZE; j++)
			chain[j] = ENCODING::Zero();
		originality_personality = Random::get().rand();
		community_personality = Random::get().rand();
		novelty_personality = Random::get().rand();
		utility_personality = Random::get().rand();
	}

	Agent(const Agent& copy):
		brain(copy.brain),
		writer(&copy)
	{
		fitness = copy.fitness;
		book = true;
		best_fitness = copy.best_fitness;
	}

	void fitness_function()
	{
		int n_cases = 1 << INPUT_SIZE; // 2^INPUT_SIZE

		float error = 0;
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

			brain.feedforward(input, output);
			error += std::abs(output[0] - target);
		}
		float fitness_float = 1 / (1 + error * error);
		int correct = 0;
		for (int i = 0; i < n_cases; i++)
		{
			int ones = 0;
			for (int j = 0; j < INPUT_SIZE; j++)
			{
				input[j] = (i >> j) & 1;
				ones += input[j];
			}
			float target = ones % 2 == 1 ? 1.0f : 0.0f;

			brain.feedforward(input, output);
			int prediction = output[0] > 0.5f ? 1 : 0;
			if (prediction == (int)target) correct++;
		}
		fitness = (float)correct / (float)n_cases;
		fitness += fitness_float * 1.0f / (float)n_cases;
		stale();
		for (Node* n : brain.m_nodes)
			n->update_usefulness(fitness, fitness, n->utility_score);
	}

	void stale()
	{
		if (fitness > best_fitness)
		{
			best_fitness = fitness;
			staleness = 0;
		}
		else
			staleness++;
	}

	void debate(Agent* other)
	{
		Node* this_node = nullptr;
		Node* other_node = nullptr;
		float highest_difference = 0;
		for (Node* n : brain.m_ordered_nodes)
		{
			if (n->depth_index == 0) continue;
			Node* same = nullptr;
			for (Node* o : other->brain.m_ordered_nodes)
			{
				if (o->depth_index == 0) continue;
				if (o->cluster_id == n->cluster_id)
				// If o and n represent the same concept
				{
					same = o;
					break;
				}
			}
			if (same == nullptr) continue;
			float diff = 1 - std::abs(n->encoding.dot(same->encoding));
			if (diff > highest_difference)
			{
				highest_difference = diff;
				this_node = n;
				other_node = same;
			}
		}
		if (this_node == nullptr || other_node == nullptr) return; // No shared concept to debate about, skip for now
		if (!book)
			brain.weight_alignment(this_node, other_node->encoding - this_node->encoding, debate_alignment());
		if (!other->book)
			other->brain.weight_alignment(other_node, this_node->encoding - other_node->encoding, other->debate_alignment());
		this_node->update_usefulness(fitness, other->fitness, other_node->utility_score);
		other_node->update_usefulness(other->fitness, fitness, this_node->utility_score);
	}

	float debate_alignment() 
	{
		// return 0.5f * std::exp(-age * age / life_expectancy / life_expectancy);
		return personality;
	}

	void teach(Agent* other)
	{
		update_score();
		brain.update_encoding();
		Node* n = select_transmission_node(other->brain.m_layers);
		std::vector<ENCODING> chain_vectors;
		chain_vectors.push_back(n->encoding);
		int initial_depth = n->depth_index;
		while (n->depth_index > 0 && chain_vectors.size() < CHAIN_SIZE - 1)
		{
			int index = Random::get().randint(0, n->connections.size());
			n = n->connections[index]->from;
			chain_vectors.push_back(n->encoding);
			initial_depth = n->depth_index;
		}
		while (!chain_vectors.empty())
		{
			chain[chain_vectors.size() - 1] = chain_vectors.back();
			chain_vectors.pop_back();
		}
		other->brain.interpret_chain(chain, initial_depth);
	}

	void show(Agent* other)
	{
		for (int _ = 0; _ < EXAMPLE_SIZE; _++)
		{
			for (int i = 0; i < INPUT_SIZE; i++)
			{
				input[i] = Random::get().rand(0, 1);
			}
			brain.feedforward(input, output);
			other->brain.backpropagate(input, output);
		}
		other->brain.update_weights(0.1, EXAMPLE_SIZE);
	}

	void update_score()
	{
		for (Node* n : brain.m_ordered_nodes)
		{
			ConceptArchive::get().update_cluster(n, n->depth_index);
		}
	}

	Node* select_transmission_node(int max_depth)
	{
		// Compute importance score for each non-input, non-bias node
		std::vector<float> scores;
		std::vector<Node*> candidates;

		for (Node* n : brain.m_nodes)
		{
			if (n->depth_index == 0) continue;
			if (n->depth_index > max_depth) continue;
			candidates.push_back(n);
			scores.push_back(n->usefulness);
		}

		if (candidates.empty()) return nullptr;

		// Softmax over importance scores
		const float T = 0.5f; // temperature: low = greedy, high = uniform
		float max_score = *std::max_element(scores.begin(), scores.end());
		float sum = 0.0f;
		for (float& s : scores)
		{
			s = std::exp((s - max_score) / T);
			sum += s;
		}

		// Sample
		float roll = Random::get().rand(0.0f, sum);
		float cumulative = 0.0f;
		for (size_t i = 0; i < candidates.size(); i++)
		{
			cumulative += scores[i];
			if (roll < cumulative)
				return candidates[i];
		}
		return candidates.back();
	}
};
