#pragma once
#include "node.hpp"

class Node;
struct Connection
{
public:
	Node* from;
	Node* to;
	float weight;

	Connection(Node* from_, Node* to_, float weight_):
		from(from_),
		to(to_),
		weight(weight_)
	{
	}
};
