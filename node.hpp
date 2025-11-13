#pragma once
#include <vector>
#include "connection.hpp"

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
	std::vector<Connection*> connections = {};

	Node(int layer_, int act_fun_idx):
		m_act_fun_idx(act_fun_idx),
		layer(layer_)
	{
	}
	void set_value(float value_);
	void forward();
};
