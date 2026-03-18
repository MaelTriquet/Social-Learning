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
	float input[INPUT_SIZE] = {0};
	float output[OUTPUT_SIZE] = {0};
	float best_fitness = 0;
	int staleness = 0;
	float score = 0;
	float originality_personality = 0.f;
	float transmission_personality = 0.f;
	ENCODING chain[CHAIN_SIZE];
	
	Agent():
		brain()
	{
		for (int j = 0; j < CHAIN_SIZE; j++)
			chain[j] = ENCODING::Zero();
		originality_personality = Random::get().rand();
		transmission_personality = 1 - originality_personality;

	}

	void fitness_function()
	{
		float inputs[4][INPUT_SIZE] = {{0,0},{0,1},{1,0},{1,1}};
		float targets[4][OUTPUT_SIZE] = {{0},{1},{1},{0}};
		float error = 0;
		for (int i = 0; i < 4; i++)
		{
			brain.feedforward(inputs[i], output);
			error += std::abs(output[0] - targets[i][0]);
		}
		fitness = 1 / (1 + error * error);
		if (fitness > best_fitness)
		{
			best_fitness = fitness;
			staleness = 0;
		}
		else
			staleness++;
		// int a = Random::get().randint(0, 4);
		// brain.feedforward(inputs[a], output);
		// fitness = 1 / (1 + std::abs(output[0] - targets[a][0]));
	}

	void teach(Agent* other)
	{
		brain.update_encoding();
		// int index = Random::get().randint(0, brain.m_nodes.size() - INPUT_SIZE - OUTPUT_SIZE);
		// Node* n = brain.m_nodes[index + INPUT_SIZE + OUTPUT_SIZE];
		Node* n = select_transmission_node();
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
				input[i] = Random::get().rand(-1, 1);
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
			n->originality_score = ConceptArchive::get().get_originality_score(n, n->depth_index);
			n->transmission_score = ConceptArchive::get().get_transmission_score(n, n->depth_index);
			n->importance_score = n->originality_score * originality_personality + n->transmission_score * transmission_personality;
		}
	}

	Node* select_transmission_node()
	{
		// Compute importance score for each non-input, non-bias node
		std::vector<float> scores;
		std::vector<Node*> candidates;

		for (Node* n : brain.m_nodes)
		{
			if (n->depth_index == 0) continue;
			candidates.push_back(n);
			scores.push_back(n->importance_score * n->depth_index);
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
	
