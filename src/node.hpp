#pragma once
#include <vector>
#include "connection.hpp"
#include <iostream>

class Node
{
private:
	int m_act_fun_idx;
	void activate();
	void propagate();
	float relu(float x);
	float sigmoid(float x);
	float tanh(float x);
	float identity(float x);

public:
	int layer;
	float value = 0;
	int history_idx;
	float gradient = 0.f;
	std::vector<Connection*> connections = {};

	// activation functions:
	// 0: relu
	// 1: sigmoid
	// 2: tanh
	// 3: identity
	// 4: bias

	Node(int layer_, int act_fun_idx, int history_idx_):
		m_act_fun_idx(act_fun_idx),
		layer(layer_),
		history_idx(history_idx_)
	{
	}

	void set_value(float value_);
	void forward();
	void remove_connection(int history_idx_);
	void calculate_gradient(float error);
};
