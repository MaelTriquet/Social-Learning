#pragma once
#include "const.hpp"
#include <vector>
#include <Eigen/Dense>
#include <cmath>
#include "node.hpp"
#include "index_vector.hpp"
#include <iostream>

class ConceptCluster {
private:


	bool inside(Node* node)
	{
		return std::abs(m_centroid.dot(node->encoding.normalized())) > CLUSTER_THRESHOLD;
	}
	bool modified = false;

public:

	ENCODING m_centroid;
	siv::ID id = -1;
	siv::Vector<Node*> m_nodes = {};
	int depth;
	void remove(Node* node)
	{
		m_nodes.erase(node->id);
		node->id = -1;
		node->cluster_id = -1;
		modified = true;
	}

	void compute_centroid() {
		if (!modified) return;
		modified = false;
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


	ConceptCluster(Node* node, int depth_):
		m_centroid(node->encoding.normalized())
	{
		node->id = m_nodes.push_back(node);
		depth = depth_;
	}

	bool contains(Node* node, int node_depth)
	{
		if (depth != node_depth) return false;
		if (inside(node))
		{
			node->id = m_nodes.push_back(node);
			modified = true;
			return true;
		}
		return false;
	}

	bool still_contains(Node* node, int node_depth)
	{
		if (depth == node_depth && inside(node))
			return true;
		m_nodes.erase(node->id);
		for (Node* n : m_nodes)
			if (node == n)
				std::cout << "Fuck\n";
		node->id = -1;
		modified = true;
		return false;
	}

	bool empty()
	{
		return m_nodes.empty();
	}
};

class ConceptArchive {
public:
	siv::Vector<ConceptCluster> m_clusters;
	std::vector<std::vector<siv::ID>> depths;
	ConceptArchive() = default;
	~ConceptArchive() = default;
	ConceptArchive(const ConceptArchive&) = delete;
	ConceptArchive& operator=(const ConceptArchive&) = delete;
	static ConceptArchive& get()
	{
		static ConceptArchive instance;
		return instance;
	}

	void update_cluster(Node* node, int depth)
	{
		if (node->cluster_id != -1 && m_clusters[node->cluster_id].still_contains(node, depth))
			// Node is already in the right cluster
			return;
		if (node->cluster_id != -1)
		{
			ConceptCluster& cluster = m_clusters[node->cluster_id];
		}

		// Find the right cluster
		while (depth >= depths.size())
			depths.push_back(std::vector<siv::ID>());
		for (auto id : depths[depth])
		{
			ConceptCluster& cluster = m_clusters[id];
			if (cluster.contains(node, depth))
			{
				node->cluster_id = cluster.id;
				return;
			}
		}
		// No cluster found, create a new one
		siv::ID cluster_id = m_clusters.emplace_back(node, depth);
		m_clusters[cluster_id].id = cluster_id;
		node->cluster_id = cluster_id;
		depths[depth].push_back(cluster_id);
	}

	void compute_centroid()
	{
		for (ConceptCluster& cluster : m_clusters)
		{
			cluster.compute_centroid();
		}
	}

	void remove(Node* node)
	{
		if (node->cluster_id == -1)
			return;
		m_clusters[node->cluster_id].remove(node);
		for (ConceptCluster& cluster : m_clusters)
		{
			for (Node* n : cluster.m_nodes)
				if (n == node)
					std::cout << "Node " << n->id << " is in cluster " << cluster.id << std::endl;
		}
	}
};
