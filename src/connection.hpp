#pragma once
#include "node.hpp"

class Node;
struct Connection
{
public:
	Node* from;
	Node* to;
	float weight;
	float accumulated_gradient = 0.0f;

	Connection(Node* from_, Node* to_, float weight_):
		from(from_),
		to(to_),
		weight(weight_)
	{
	}
};
