#pragma once
#include <iostream>
#include "agent.hpp"
#include "const.hpp"
#include <vector>

class Group {

public:
	std::vector<Agent*> agents = std::vector<Agent*>();
	std::vector<std::vector<ENCODING>> representative = std::vector<std::vector<ENCODING>>();
	Group(Agent* founder)
	{
		agents.push_back(founder);
		for (Node* n : founder->brain.m_ordered_nodes)
		{
			if (n->depth_index >= representative.size())
				representative.push_back(std::vector<ENCODING>());
			representative[n->depth_index].push_back(n->encoding);
		}
	}
	~Group() = default;

	bool isInGroup(Agent* agent)
	{
		float distance = 0;
		std::vector<float> distances = std::vector<float>();
		for (Node* n : agent->brain.m_ordered_nodes)
		{
			if (n->depth_index == 0) continue;
			float d = minDist(n->encoding, n->depth_index);
			distances.push_back(1-d);
			distance += d / agent->brain.m_ordered_nodes.size();
			if (distance > GROUP_THRESHOLD)
				return false;
		}
		for (int i = 0; i < distances.size(); i++)
			agent->brain.m_ordered_nodes[i]->community_score = distances[i];
		return true;
	}

	float minDist(ENCODING& encoding, int depth)
	{
		float enc_norm_sq = encoding.dot(encoding);
		if (enc_norm_sq == 0.0f)
			return 1.0f;
		if (depth >= (int)representative.size())
			return 1.0f;
		if (representative[depth].empty())
			return 1.0f;

		float enc_norm_inv = 1.0f / std::sqrt(enc_norm_sq);
		float distance = 1.0f;

		for (const ENCODING& e : representative[depth])
		{
			// Representatives are pre-normalised so e.norm() == 1.0
			float d = 1.0f - std::abs(encoding.dot(e)) * enc_norm_inv;
			if (d < distance)
			{
				distance = d;
				if (distance == 0.0f)
					return 0.0f; // perfect match, can't do better
			}
		}
		return distance;
	}

	Agent* getBestAgent() const
	{
		float best_score = -1.0f;
		Agent* best_agent = nullptr;
		for (Agent* a : agents)
		{
			if (a->fitness > best_score)
			{
				best_score = a->fitness;
				best_agent = a;
			}
		}
		return best_agent;
	}
};
