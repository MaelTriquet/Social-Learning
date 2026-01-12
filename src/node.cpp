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
	case 4:
		value = 1;
		break;
	default:
		break;
	}
}

void Node::calculate_gradient(float error)
{
	switch (m_act_fun_idx)
	{
	case 0: // ReLU
		gradient = (value > 0.0f ? 1.0f : 0.0f) * error;
		break;

	case 1: // Sigmoid
		gradient = value * (1.0f - value) * error;
		break;

	case 2: // Tanh
		gradient = (1.0f - value * value) * error;
		break;

	case 3: // Identity (linear)
		gradient = error;
		break;
	
	case 4: // Bias
		gradient = 0.0f;
		break;

	default:
		gradient = error;
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
	// Don't activate input layer (layer 0) - they should just pass through their values
	if (layer != 0) {
		activate();
	}
	propagate();
}

void Node::remove_connection(int history_idx_)
{
	for (size_t i = 0; i < connections.size(); i++)
	{
		if (connections[i]->history_idx == history_idx_)
		{
			// remove connection from this node
			std::swap(connections[i], connections.back());
			connections.pop_back();
			break;
		}
	}
}
