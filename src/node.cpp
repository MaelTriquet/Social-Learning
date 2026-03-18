#include "node.hpp"

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
		value = std::tanh(TANH_SCALE * value);
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

void Node::propagate()
{
	for (auto connection : connections)
	{
		value += connection->weight * connection->from->value;
	}
	activate();
}

float Node::relu(float x)
{
	return x > 0 ? x : 0;
}

float Node::sigmoid(float x)
{
	return 1.0f / (1.0f + exp(-SIGMOID_SCALE * x));
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
	propagate();
}

void Node::remove_connection(Connection* connection)
{
	for (size_t i = 0; i < connections.size(); i++)
	{
		if (connections[i] == connection)
		{
			// remove connection from this node
			std::swap(connections[i], connections.back());
			connections.pop_back();
			break;
		}
	}
}

float Node::d_relu(float x)
{
	return x > 0 ? 1 : 0;
}

float Node::d_sigmoid(float x)
{
	return SIGMOID_SCALE * x * (1 - x);
}

float Node::d_tanh(float x)
{
	return TANH_SCALE * (1 - x * x);
}

float Node::d_identity(float x)
{
	return 1;
}

float Node::derivative()
{
	switch (m_act_fun_idx)
	{
	case 0: // RELU
 		return d_relu(value);
	case 1: // SIGMOID
		return d_sigmoid(value);
	case 2: // TANH
		return d_tanh(value);
	case 3: // IDENTITY
		return d_identity(value);
	case 4: // BIAS
		return 0;
	default:
		return 0;
	}
}

void Node::update_encoding()
{
	if (depth_index == 0)
	{
		depth_index = 0;
		return;
	}
	encoding *= 0;
	depth_index = 0;
	for (Connection* c : connections)
	{
		encoding += c->weight * c->from->encoding.normalized();
		if (c->from->depth_index + 1 > depth_index)
		{
			depth_index = c->from->depth_index + 1;
		}
	}
}
