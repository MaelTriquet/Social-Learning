#pragma once
#include "node.hpp"

class Node;
struct Connection
{
public:
	Node* from;
	Node* to;
	float weight;
	int history_idx;
	float delta = 0.f;

	Connection(Node* from_, Node* to_, float weight_, int history_idx_):
		from(from_),
		to(to_),
		weight(weight_),
		history_idx(history_idx_)
	{
	}
};
