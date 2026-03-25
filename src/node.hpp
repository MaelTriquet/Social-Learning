#pragma once
#include <vector>
#include "connection.hpp"
#include <cmath>
#include "const.hpp"
#include <Eigen/Dense>
#include "index_vector.hpp"


class Node
{
private:
	void activate();
	void propagate();
	float relu(float x);
	float sigmoid(float x);
	float tanh(float x);
	float identity(float x);
	float d_relu(float x);
	float d_sigmoid(float x);
	float d_tanh(float x);
	float d_identity(float x);

public:
	int m_act_fun_idx;
	int depth_index;
	float value = 0;
	float delta = 0.0f;
	float novelty_score = 0;
	float utility_score = 0;
	float community_score = 0;
	float originality_score = 0;
	float importance_score = 0;
	siv::ID cluster_id = -1;
	siv::ID id = -1;
	ENCODING encoding = ENCODING::Zero();
	// Holds the connections that are incoming to this node
	std::vector<Connection*> connections{};

	// activation functions:
	// 0: relu
	// 1: sigmoid
	// 2: tanh
	// 3: identity
	// 4: bias

	Node(int depth_index_, int act_fun_idx = 0):
		m_act_fun_idx(act_fun_idx),
		depth_index(depth_index_)
	{}

	Node(const Node& copy);

	void forward();
	float derivative();
	void remove_connection(Connection* connection);
	void update_encoding();
};
