#include "brain.hpp"

Brain::Brain()
{
	// Form a basic brain, empty with only input and output nodes
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		m_nodes.push_back(new Node(0, IDENTITY));
		m_nodes.back()->encoding[i] = 1;
	}
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		m_nodes.push_back(new Node(1, OUTPUT_ACTIVATION));
	}
	// Add bias node
	m_nodes.push_back(new Node(0, 4));
	m_bias = m_nodes.back();
	m_bias->encoding[INPUT_SIZE] = 1;
	
	// Fully connect the input-to-output for everyone
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		for (int j = 0; j < OUTPUT_SIZE; j++)
		{
			add_connection(m_nodes[i], m_nodes[INPUT_SIZE + j], Random::get().rand(-1, 1));
		}
	}
	
	// Add bias-to-output connections
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		add_connection(m_nodes[INPUT_SIZE + OUTPUT_SIZE], m_nodes[INPUT_SIZE + i], Random::get().rand(-1, 1));
	}
	sort_nodes();
	update_encoding();
}

Brain::~Brain()
{
	for (auto connection : m_connections)
		delete connection;
	for (auto node : m_nodes)
	{
		// ConceptArchive::get().remove(node, node->depth_index);
		delete node;
	}
}

void Brain::add_node(Connection* connection)
{
	if (m_nodes.size() >= max_size)
		return weight_exploration();
	Node* candidate = new Node(connection->from->depth_index + 1, HIDDEN_ACTIVATION);

	// if necessary, add a depth_index
	if (candidate->depth_index == connection->to->depth_index)
	{
		m_layers++;
		for (auto node : m_nodes)
		{
			if (node->depth_index >= candidate->depth_index)
			{
				node->depth_index++;
			}
		}
	}

	add_connection(connection->from, candidate, connection->weight);
	add_connection(candidate, connection->to, 1);
	add_connection(m_bias, candidate, 0); // connect bias node to candidate
	
	connection->to->remove_connection(connection);

	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i] == connection)
		{
			// remove connection from this node
			std::swap(m_connections[i], m_connections.back());
			m_connections.pop_back();
			break;
		}
	}
	delete connection;
	m_nodes.push_back(candidate);
	sort_nodes();
}

void Brain::add_connection(Node* from, Node* to, float weight)
{
	m_connections.push_back(new Connection(from, to, weight));
	to->connections.push_back(m_connections.back());
}

bool Brain::fully_connected()
{
	int before = 0;
	int total = 0;
	for (int depth_index = 0; depth_index < m_layers; depth_index++)
	{
		int depth_index_size = 0;
		for (Node* node : m_nodes)
		{
			if (node->depth_index == depth_index)
			{
				total += before;
				depth_index_size++;
			}
		}
		before += depth_index_size;
	}
	return total == (int)m_connections.size();
}

void Brain::print()
{
	for (auto connection : m_connections)
	{
		std::cout << '[' << connection->from->encoding.transpose() << ']' << " is connected to " << '[' << connection->to->encoding.transpose() << ']' << " with a weight of " << connection->weight << std::endl;
	}

	std::cout << "Output: " << '[' << m_nodes[INPUT_SIZE]->encoding.transpose() << ']' << std::endl;
}

void Brain::feedforward(float input[INPUT_SIZE], float output[OUTPUT_SIZE])
{
	reset_nodes();
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		m_nodes[i]->value = input[i];
	}
	for (Node* node : m_ordered_nodes)
	{
		node->forward();
	}
	for (int i = 0; i < OUTPUT_SIZE; i++)
	{
		output[i] = m_nodes[i + INPUT_SIZE]->value;
	}
}

void Brain::sort_nodes()
{
	m_ordered_nodes.clear();
	for (int i = 0; i < m_layers; i++)
	{
		for (auto node : m_nodes)
		{
			if (node->depth_index == i)
			{
				m_ordered_nodes.push_back(node);
			}
		}
	}
}

void Brain::reset_nodes()
{
	for (auto node : m_nodes)
	{
		node->value = 0;
	}
}

void Brain::backpropagate(float input[INPUT_SIZE], float target[OUTPUT_SIZE])
{
    // --- 1. Forward pass ---
    float _[OUTPUT_SIZE];
    feedforward(input, _);

    // --- 2. Reset all deltas ---
    for (Node* n : m_ordered_nodes)
        n->delta = 0.0f;

    // --- 3. Output node deltas (MSE loss) ---
    for (int i = 0; i < OUTPUT_SIZE; i++)
    {
        Node* n = m_nodes[i + INPUT_SIZE];
        float error = n->value - target[i];
        n->delta = error * n->derivative();
    }

    // --- 4. Backpropagate deltas through hidden nodes (reverse order) ---
    for (int i = (int)m_ordered_nodes.size() - 1; i >= 0; i--)
    {
        Node* n = m_ordered_nodes[i];
        if (n->depth_index == 0) continue; // skip input/bias layer
        if (n->delta == 0.0f) continue;    // no signal to propagate

        for (Connection* c : n->connections)
        {
            if (c->from->depth_index == 0) continue; // don't propagate to input/bias
            c->from->delta += c->weight * n->delta * c->from->derivative();
        }
    }

    // --- 5. Accumulate gradients ---
    for (Connection* c : m_connections)
        c->accumulated_gradient += c->to->delta * c->from->value;
}

void Brain::update_weights(float learning_rate, int batch_size)
{
    for (Connection* c : m_connections)
	{
        c->weight -= learning_rate * c->accumulated_gradient / (float)batch_size;
		if (CLAMP_WEIGHTS)
			c->weight = std::clamp(c->weight, -CLAMP, CLAMP);
		c->accumulated_gradient = 0.0f;
	}
	update_encoding();
}

void Brain::update_encoding()
{
	for (Node* n : m_ordered_nodes)
	{
		n->update_encoding();
	}

	for (int i = m_ordered_nodes.size() - 1; i >= 0; i--)
	{
		Node* n = m_ordered_nodes[i];
		if (n->depth_index == m_layers - 1)
		{
			n->utility_score = 1;
		}
		for (Connection* c : n->connections)
			c->from->utility_score += std::abs(c->weight) * n->utility_score;
	}
}




void Brain::add_node_random()
{
	Connection* connection;
	do
	{
		connection = m_connections[Random::get().rand(0, m_connections.size() - 1)];
	} while (connection->from == m_bias);
	add_node(connection);
}

// Returns {p_weight_alignment, p_add_connection, p_add_node_target, p_add_node_random}


void Brain::weight_exploration()
{
	float exploration_magnitude = std::exp(-age / 30.0f);
	exploration_magnitude = 1.0f;
	// for (Connection* c : m_connections)
	// {
	// 	c->weight += Random::get().rand(-0.1f, 0.1f) * exploration_magnitude;
	// 	if (CLAMP_WEIGHTS)
	// 		c->weight = std::clamp(c->weight, -CLAMP, CLAMP);
	// }
	int index = Random::get().randint(0, m_connections.size());
	m_connections[index]->weight += Random::get().rand(-0.1f, 0.1f) * exploration_magnitude;
	if (CLAMP_WEIGHTS)
		m_connections[index]->weight = std::clamp(m_connections[index]->weight, -CLAMP, CLAMP);
}

void Brain::distance_score(ENCODING chain[CHAIN_SIZE], int initial_depth, std::vector<float>& distances)
{
	for (Node* n : m_ordered_nodes)
	{
		if (n->depth_index < initial_depth) continue;
		if (n->depth_index >= initial_depth + CHAIN_SIZE) return;
		int index = n->depth_index - initial_depth;
		float distance = chain[index].dot(n->encoding);
		if (distances.size() <= index)
			distances.push_back(distance);
		else
			if (distance < distances[index])
				distances[index] = distance;
	}
}

void Brain::interpret_chain(const ENCODING chain[CHAIN_SIZE], int initial_depth)
{
    // --- Clear resonance array ---
    for (int i = 0; i < CHAIN_SIZE; i++)
        m_resonance[i] = ResonanceResult();

    Node* previous_anchor = nullptr;

    for (int i = 0; i < CHAIN_SIZE; i++)
    {
        int expected_depth = initial_depth + i;
        float chain_norm = chain[i].norm();
        if (chain_norm == 0.0f) continue;

        ENCODING normalized_chain = chain[i] / chain_norm;

        // --- Find best anchor at exact expected depth ---
        float best_score = -1.0f;
        Node* best_anchor = nullptr;

        for (Node* n : m_ordered_nodes)
        {
            if (n->depth_index < expected_depth) continue;
            if (n->depth_index > expected_depth) break;

            float n_norm = n->encoding.norm();
            if (n_norm == 0.0f) continue;

            // Base similarity — cosine between node encoding and chain element
            float similarity = std::abs(n->encoding.dot(chain[i]))
                             / (n_norm * chain_norm);

            // Continuity bonus — prefer descendants of previous anchor
			for (auto connection : n->connections)
			{
				if (connection->from == previous_anchor)
				{
					similarity += 0.2f;
					break;
				}
			}

            if (similarity > best_score)
            {
                best_score  = similarity;
                best_anchor = n;
            }
        }

        // --- No node at this depth — leave result empty ---
        if (!best_anchor) continue;

        // --- Compute residual ---
        // residual = chain[i] - projection of chain[i] onto anchor
        float anchor_norm_sq = best_anchor->encoding.dot(best_anchor->encoding);
        ENCODING residual =
            ENCODING::Zero();

        if (anchor_norm_sq > 0.0f)
        {
            float proj_scalar = chain[i].dot(best_anchor->encoding) / anchor_norm_sq;
            residual = chain[i] - best_anchor->encoding * proj_scalar;
        }
        else
            residual = chain[i];

		float conn_similarity = 0.0f;
		Connection* best_conn = nullptr;
		for (auto connection : best_anchor->connections)
		{
			float conn_sim = std::abs(connection->from->encoding.dot(residual));
			if (conn_sim > conn_similarity)
			{
				conn_similarity = conn_sim;
				best_conn = connection;
			}
		}

        // --- Fill result ---
        m_resonance[i].anchor          = best_anchor;
        m_resonance[i].resonance_score = std::min(1.0f, best_score); // clamp after bonus
        m_resonance[i].residual        = residual;
        m_resonance[i].residual_magnitude = std::min(1.0f, residual.norm());
        m_resonance[i].depth_exists    = true;
		m_resonance[i].best_conn_similarity = conn_similarity;

        previous_anchor = best_anchor;
    }
	decide_action();
}

void Brain::decide_action()
{

    // ================================================================
    // STEP 1: Select target from chain
    // Only elements where depth_exists is true are candidates
    // Score = resonance_score * ambition * personality_bias
    // ================================================================

    int   best_target_idx  = -1;
    float best_target_score = -1.0f;
	float scores[CHAIN_SIZE] = {};

    for (int i = 0; i < CHAIN_SIZE; i++)
    {
        if (!m_resonance[i].depth_exists) continue;

        // Ambition: reward targeting deeper elements
        float ambition = (float)(i + 1) / (float)CHAIN_SIZE;

        // Resonance: how well the learner already understands this element
        float resonance = m_resonance[i].resonance_score;

        // Residual: how much is still to learn
        float residual = m_resonance[i].residual_magnitude;

        // Target score: want elements that are ambitious, partially understood,
        // and have something left to learn
        // High resonance alone = boring (already know it)
        // High residual alone = too foreign (can't integrate it)
        // The sweet spot is medium resonance + medium residual
        float target_score = (0.5f + ambition/2.0f)
						   // * (1.5f - m_resonance[i].anchor->importance_score) // Is this node really important to the agent
                           * (resonance * 0.5f + residual * 0.5f)
                           * (1.0f - std::abs(resonance - residual)); // peak at resonance≈residual
		scores[i] = target_score;

        if (target_score > best_target_score)
        {
            best_target_score = target_score;
            best_target_idx   = i;
        }
    }

	// Select the best target randomly, but heavily favour the ones with the highest score
	float max_score = -1.f;
	int max_score_idx = -1.f;
	while (max_score != 0)
	{
		max_score = -1;
		for (int i = 0; i < CHAIN_SIZE; i++)
		{
			if (scores[i] > max_score)
			{
				max_score = scores[i];
				max_score_idx = i;
			}
		}
		float a = Random::get().rand(0, 1);
		if (a < max_score)
		{
			best_target_idx = max_score_idx;
			break;
		}
		scores[max_score_idx] = 0;
	}

    // No valid target found — fall back to add_node_random
    if (best_target_idx == -1)
		return add_node_random();


    const ResonanceResult& res = m_resonance[best_target_idx];


    // ================================================================
    // STEP 3: Compute raw scores for each action
    // ================================================================

    // --- Weight alignment ---
    // Good when: high resonance (anchor is close),
    //            high best_conn_similarity (existing connection can be nudged),
    //            low residual (small gap to close)
    float score_wa = res.resonance_score
                   * res.best_conn_similarity
                   * (1.0f - res.residual_magnitude);

    // --- Add connection ---
    // Good when: meaningful residual exists (something to add),
    //            but existing connections are saturated (wa won't help much),
    //            and best_conn_similarity is low (no existing conn covers residual)
    float score_ac = res.residual_magnitude
                   * (1.0f - res.best_conn_similarity)
                   * res.resonance_score; // still need anchor to exist

    // --- Add node from target ---
    // Good when: large residual (concept is foreign),
    float score_an_target = res.residual_magnitude
						   * (1.0f - res.resonance_score)
						   * (1.0f - res.best_conn_similarity);

    // --- Add node random (refusal) ---
    // Good when: low resonance overall,
    //            would change a high importance node
	// Should be rare overall
    float score_an_random = ((1.0f - res.resonance_score) + (1.0f - res.anchor->importance_score)) * 0.05f;

    // ================================================================
    // STEP 5: Softmax with temperature
    // ================================================================

    std::array<float, 4> raw = {
        score_wa,
        score_ac,
        score_an_target,
        score_an_random
    };

    const float TEMPERATURE = 0.3f;
    float max_val = *std::max_element(raw.begin(), raw.end());
    float sum = 0.0f;
    for (float v : raw)
        sum += std::exp((v - max_val) / TEMPERATURE);

    for (int i = 0; i < 4; i++)
        raw[i] = std::exp((raw[i] - max_val) / TEMPERATURE) / sum;

    // ================================================================
    // STEP 6: Sample action
    // ================================================================

    float roll = Random::get().rand(0.0f, 1.0f);
    float cumulative = 0.0f;
	int action = 3; // default to add_node_random
    for (int i = 0; i < 4; i++)
    {
        cumulative += raw[i];
        if (roll < cumulative)
        {
            action = i;
            break;
        }
    }
	Global::get().interactions++;
	switch (action)
	{
	case 0:
		Global::get().weight_alignment++;
		return weight_alignment(res.anchor, res.residual);
	case 1:
		Global::get().add_connection++;
		return add_connection(res.anchor, res.residual);
	case 2:
		Global::get().add_node++;
		return add_node(res.anchor, res.residual);
	case 3:
		if (Random::get().rand(0, 1) < 0.99)
		{
			Global::get().weight_exploration++;
			weight_exploration();
		} else
			Global::get().add_node_random++;
		return add_node_random();
	}
	update_encoding();
}

void Brain::weight_alignment(Node* anchor, ENCODING residual)
{
    // --- 1. Find incoming connection best aligned with residual ---
    float max_pred_scalar = -1.0f;
    Connection* max_connection = nullptr;
    for (Connection* c : anchor->connections)
    {
        float scalar = std::abs(c->from->encoding.dot(residual));
        if (scalar > max_pred_scalar)
        {
            max_pred_scalar = scalar;
            max_connection = c;
        }
    }
    if (!max_connection) return;

    // --- 2. Closed-form optimal weight ---
    // goal = anchor->encoding + residual
    // new_v = anchor->encoding + (x - old_w) * pred
    // => (x - old_w) * pred = residual
    // => x = old_w + pred^T * residual / ||pred||²
    ENCODING pred_vec = max_connection->from->encoding;
    float denom = pred_vec.dot(pred_vec);
    if (denom == 0.0f) return;

    float x_optimal = max_connection->weight + pred_vec.dot(residual) / denom;

    // --- 3. Age-scaled partial step toward optimal ---
    // Young agents take bigger steps, old agents nudge carefully
    float step = std::exp(-(float)age / 30.0f); // 1.0 when young, approaches 0 when old
    float noise = Random::get().rand(-0.1f, 0.1f) * step;
    max_connection->weight += step * (x_optimal - max_connection->weight) + noise;

    if (CLAMP_WEIGHTS)
        max_connection->weight = std::clamp(max_connection->weight, -CLAMP, CLAMP);

}

void Brain::add_connection(Node* anchor, ENCODING residual)
{
    // --- 1. Build set of already connected predecessors ---
    std::unordered_set<Node*> already_connected;
    for (Connection* c : anchor->connections)
        already_connected.insert(c->from);

    // --- 2. Find best unconnected candidate aligned with residual ---
    float max_pred_scalar = -1.0f;
    Node* best_candidate = nullptr;
    for (Node* n : m_ordered_nodes)
    {
        if (n->depth_index >= anchor->depth_index) continue;
        if (already_connected.count(n)) continue;
        if (n == m_bias) continue;

        float n_norm = n->encoding.norm();
        float r_norm = residual.norm();
        if (n_norm == 0.0f || r_norm == 0.0f) continue;

        float scalar = std::abs(n->encoding.dot(residual)) / (n_norm * r_norm);
        if (scalar > max_pred_scalar)
        {
            max_pred_scalar = scalar;
            best_candidate  = n;
        }
    }
    if (!best_candidate) return;

    // --- 3. Closed-form optimal weight for new connection ---
    // goal = anchor->encoding + residual
    // new_v = anchor->encoding + x * pred  (new connection, old_w = 0)
    // => x * pred = residual
    // => x = pred^T * residual / ||pred||²
    ENCODING pred_vec = best_candidate->encoding;
    float denom = pred_vec.dot(pred_vec);
    if (denom == 0.0f) return;

    float x_optimal = pred_vec.dot(residual) / denom;

    // --- 4. Age-scaled partial step toward optimal ---
    float step  = std::exp(-(float)age / 30.0f);
    float noise = Random::get().rand(-0.1f, 0.1f) * step;
    float w     = step * x_optimal + noise;

    if (CLAMP_WEIGHTS)
        w = std::clamp(w, -CLAMP, CLAMP);

    add_connection(best_candidate, anchor, w);
}

void Brain::add_node(Node* anchor, ENCODING residual)
{
    if (anchor->depth_index <= 0) return;

    // --- 1. Find best predecessor ---
    float best_scalar  = -1.0f;
    Node* best_pred    = nullptr;
    float residual_norm = residual.norm();
    if (residual_norm == 0.0f) return;

    for (Node* n : m_ordered_nodes)
    {
        if (n->depth_index >= anchor->depth_index) break;
		if (n->depth_index < anchor->depth_index - 1) continue;
        if (n == m_bias) continue;

        float n_norm = n->encoding.norm();
        if (n_norm == 0.0f) continue;

        float scalar = std::abs(n->encoding.dot(residual)) / (n_norm * residual_norm);
        if (scalar > best_scalar)
        {
            best_scalar = scalar;
            best_pred   = n;
        }
    }
    if (!best_pred) return;

	bool connected = false;
	Connection* conn = nullptr;
	for (auto connection : anchor->connections)
		if (connection->from == best_pred)
		{
			connected = true;
			conn = connection;
			break;
		}
	if (connected)
	{
		add_node(conn);
		return;
	}

    // --- 2. Optimal weight for single connection ---
    // new_node->encoding = w * pred->encoding
    // minimise ||residual - w * pred||²
    // => w = pred^T * residual / ||pred||²
    ENCODING pred_vec = best_pred->encoding;
    float denom = pred_vec.dot(pred_vec);
    if (denom == 0.0f) return;

    float x_optimal = pred_vec.dot(residual) / denom;

    // --- 3. Age-scaled partial step + noise ---
    float noise = Random::get().rand(-0.1f, 0.1f);
    float w     = x_optimal + noise;

    if (CLAMP_WEIGHTS)
        w = std::clamp(w, -CLAMP, CLAMP);
	
	add_connection(best_pred, anchor, w);
	add_node(m_connections[m_connections.size() - 1]);
}
