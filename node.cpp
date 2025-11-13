#include "node.hpp"
#include <cmath>

void Node::activate()
{
	// check the m_act_fun_idx value and run the corresponding function
	switch (m_act_fun_idx)
	{
	case 0:
		value = relu(value);
		break;
	case 1:
		value = sigmoid(value);
		break;
	case 2:
		value = tanh(value);
		break;
	case 3:
		value = identity(value);
		break;
	default:
		break;
	}
}

void Node::propagate()
{
	for (auto connection : connections)
	{
		connection->to->value += connection->weight * value;
	}
}

void Node::set_value(float value_)
{
	value = value_;
}

float Node::relu(float x)
{
	return x > 0 ? x : 0;
}

float Node::sigmoid(float x)
{
	return 1 / (1 + exp(-x));
}

float Node::tanh(float x)
{
	return std::tanh(x);
}

float Node::identity(float x)
{
	return x;
}

void Node::forward()
{
	activate();
	propagate();
}
