#pragma once
#include <algorithm>
#include "node.hpp"
#include "connection.hpp"
#include <vector>
#include <iostream>
#include "random.hpp"
#include "const.hpp"
#include <Eigen/Dense>
#include <unordered_set>


struct ResonanceResult
{
    Node* anchor                = nullptr;
    float resonance_score       = 0.0f;
    ENCODING residual = ENCODING::Zero();
    float residual_magnitude    = 1.0f; // normalised residual norm, 1 = fully unknown
    bool  depth_exists          = false;
	float best_conn_similarity  = 0.0f;
};

class Brain
{
private:
	int m_layers = 2;
	Node* m_bias;

	bool fully_connected();
public:
	std::vector<Node*> m_ordered_nodes = {};
	std::vector<Connection*> m_connections = {};
	std::vector<Node*> m_nodes = {};
	ResonanceResult m_resonance[CHAIN_SIZE] = {};
	int max_size = 0;
	int age = 0;
	Brain();
	~Brain();

	void add_node(Connection* connection);
	void add_connection(Node* from, Node* to, float weight);
	void print();
	void update_encoding();
	void feedforward(float input[INPUT_SIZE], float output[OUTPUT_SIZE]);
	void sort_nodes();
	void reset_nodes();
	void backpropagate(float input[INPUT_SIZE], float target[OUTPUT_SIZE]);
	void update_weights(float learning_rate, int batch_size);
	void weight_alignment(ENCODING target, int target_depth);
	void add_connection(ENCODING target, int target_depth);
	void add_node_random();
	void add_node(ENCODING target, int target_depth);
	void compute_mutation_probability(ENCODING chain[CHAIN_SIZE], int initial_depth);
	void distance_score(ENCODING chain[CHAIN_SIZE], int initial_depth, std::vector<float>& distances);
	void reset();
	std::array<float, 5> compute_probabilities(ENCODING chain[CHAIN_SIZE], int initial_depth);
	void weight_exploration();
	void interpret_chain(const ENCODING chain[CHAIN_SIZE], int initial_depth);
	void decide_action();
	void weight_alignment(Node* anchor, ENCODING residual);
	void add_connection(Node* anchor, ENCODING residual);
	void add_node(Node* anchor, ENCODING residual);
};
