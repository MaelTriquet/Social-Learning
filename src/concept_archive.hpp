#pragma once
#include "const.hpp"
#include <vector>
#include <Eigen/Dense>
#include <cmath>
#include "node.hpp"

class ConceptCluster {
private:
	int nb_transmitions = 0;
	float transmission_success = 0.0f;
	ENCODING m_centroid = ENCODING::Zero();


	bool inside(Node* node)
	{
		return std::abs(m_centroid.dot(node->encoding.normalized())) < CLUSTER_THRESHOLD;
	}


public:

	std::vector<Node*> m_nodes = {};
	bool remove(Node* node)
	{
		bool found = false;
		for (int i = 0; i < m_nodes.size(); i++)
		{
			Node* n = m_nodes[i];
			if (n == node)
			{
				m_nodes[i] = m_nodes.back();
				m_nodes.pop_back();
				found = true;
			}
		}
		return found;
	}

	void compute_centroid() {
		m_centroid *= 0;
		int count = 0;
		for (Node* node : m_nodes)
		{
			m_centroid += node->encoding;
			count++;
			if (count > 20) break;
		}
		if (m_centroid.norm() == 0.0f)
			return;
		m_centroid.normalize();
	}


	ConceptCluster(Node* node)
	{
		m_nodes.push_back(node);
		m_centroid = node->encoding.normalized();
	}

	bool contains(Node* node)
	{
		for (int i = 0; i < m_nodes.size(); i++)
		{
			Node* n = m_nodes[i];
			if (n == node)
			{
				if (inside(node))
					return true;
				m_nodes[i] = m_nodes.back();
				m_nodes.pop_back();
				return false;
			}
		}
		if (!inside(node))
			return false;
		node->novelty_score = 1;
		m_nodes.push_back(node);
		return true;
	}

	void add_transmission(Node* result)
	{
		nb_transmitions++;
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
		if (m_clusters.size() <= depth)
		{
			m_clusters.push_back(std::vector<ConceptCluster>());
		}
		int index = -1;
		for (int i = 0; i < m_clusters[depth].size(); i++)
		{
			if (m_clusters[depth][i].contains(node))
			{
				cached_node = node;
				cached_originality_score = m_clusters[depth][i].originality_score(node);
				cached_transmission_score = m_clusters[depth][i].transmission_score();
				index = i;
			}
		}
		if (index >= 0)
			return m_clusters[depth][index];
		node->novelty_score = 1;
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

	void compute_centroid()
	{
		for (auto& depth : m_clusters)
			for (ConceptCluster& cluster : depth)
				cluster.compute_centroid();
	}

	void remove(Node* node, int depth_index)
	{
		auto& depth = m_clusters[depth_index];
		for (ConceptCluster& cluster : depth)
			cluster.remove(node);
	}
};
