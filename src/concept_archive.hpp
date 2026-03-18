#pragma once
#include "const.hpp"
#include <vector>
#include <Eigen/Dense>
#include <cmath>
#include "node.hpp"

class ConceptCluster {
private:
	std::vector<Node*> m_nodes = {};
	int nb_transmitions = 0;
	float transmission_success = 0.0f;

	ENCODING get_centroid() {
		ENCODING centroid = ENCODING::Zero();
		int count = 0;
		for (Node* node : m_nodes)
		{
			centroid += node->encoding;
			count++;
			if (count > 20) break;
		}
		if (centroid.norm() == 0.0f)
			return ENCODING::Zero();
		return centroid / centroid.norm();
	}

	bool inside(Node* node)
	{
		ENCODING centroid = get_centroid();
		ENCODING normed = node->encoding / node->encoding.norm();
		float dot = std::abs(centroid.dot(normed));
		return dot > CLUSTER_THRESHOLD;
	}


public:

	ConceptCluster(Node* node)
	{
		m_nodes.push_back(node);
	}

	bool contains(Node* node)
	{
		for (Node* n : m_nodes)
		{
			if (n == node)
			{
				if (inside(node))
					return true;
				std::swap(m_nodes[0], m_nodes.back());
				m_nodes.pop_back();
				return false;
			}
		}
		if (!inside(node))
			return false;
		m_nodes.push_back(node);
		return true;
	}

	void add_transmission(Node* result)
	{
		for (Node* node : m_nodes)
		{
			if (node == result)
			{
				transmission_success++;
				return;
			}
		}
	}

	float originality_score(Node* node)
	{
		int i;
		for (i = 0; i < m_nodes.size(); i++)
		{
			if (m_nodes[i] == node)
				break;
		}

		if (i == m_nodes.size())
			// not supposed to happen, the node should be in the cluster
			return 0.0f;
		return 1.0f / std::log(i + 1);
	}

	float transmission_score()
	{
		return transmission_success / (float)nb_transmitions;
	}

};

class ConceptArchive {
private:
	std::vector<std::vector<ConceptCluster>> m_clusters = std::vector<std::vector<ConceptCluster>>();
	Node* cached_node = nullptr;
	float cached_originality_score = 0.0f;
	float cached_transmission_score = 0.0f;
public:

    ConceptArchive(const ConceptArchive&) = delete;
    ConceptArchive& operator=(const ConceptArchive&) = delete;
	ConceptArchive() = default;
	~ConceptArchive() = default;

    static ConceptArchive& get()
    {
        static ConceptArchive instance;
        return instance;
    }

	ConceptCluster& find_cluster(Node* node, int depth)
	{
		bool new_depth = false;
		if (m_clusters.size() <= depth)
		{
			m_clusters.push_back(std::vector<ConceptCluster>());
			new_depth = true;
		}
		for (int i = 0; i < m_clusters[depth].size() && !new_depth; i++)
		{
			if (m_clusters[depth][i].contains(node))
			{
				return m_clusters[depth][i];
			}
		}
		m_clusters[depth].push_back(ConceptCluster(node));
		cached_node = node;
		cached_originality_score = m_clusters[depth].back().originality_score(node);
		cached_transmission_score = m_clusters[depth].back().transmission_score();
		return m_clusters[depth].back();
	}

	float get_originality_score(Node* node, int depth)
	{
		if (cached_node == node)
			return cached_originality_score;
		cached_node = node;
		ConceptCluster& cluster = find_cluster(node, depth);
		cached_originality_score = cluster.originality_score(node);
		cached_transmission_score = cluster.transmission_score();
		return cached_originality_score;
	}

	float get_transmission_score(Node* node, int depth)
	{
		if (cached_node == node)
			return cached_transmission_score;
		cached_node = node;
		ConceptCluster& cluster = find_cluster(node, depth);
		cached_originality_score = cluster.originality_score(node);
		cached_transmission_score = cluster.transmission_score();
		return cached_transmission_score;
	}

	void add_transmission(Node* target, Node* result, int depth)
	{
		find_cluster(target, depth).add_transmission(result);
	}
};
