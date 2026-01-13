#include "village.hpp"

Village::Village()
{
}

Village::~Village()
{
	for (Agent* agent : m_agents)
	{
		delete agent;
	}
	m_agents.clear();
}

void Village::add_agent(Agent* agent)
{
	m_agents.push_back(agent);
}

void Village::remove_agent(int index)
{
	if (index >= 0 && index < static_cast<int>(m_agents.size()))
	{
		delete m_agents[index];
		m_agents.erase(m_agents.begin() + index);
	}
}

Agent* Village::get_agent(int index)
{
	if (index >= 0 && index < static_cast<int>(m_agents.size()))
	{
		return m_agents[index];
	}
	return nullptr;
}

int Village::get_size() const
{
	return static_cast<int>(m_agents.size());
}

void Village::step()
{
	for (Agent* agent : m_agents)
	{
		agent->step();
	}
}

void Village::sort_agents()
{
	for (Agent* agent : m_agents)
	{
		sorted_agents.push_back(agent);
	}
	std::sort(sorted_agents.begin(), sorted_agents.end(), [](Agent* a, Agent* b) {
		return a->score > b->score;
	});
}

void Village::evaluate()
{
	for (Agent* agent : m_agents)
	{
		agent->score_evaluation();
	}

	// Sort agents by score
	sort_agents();
}

void Village::debate()
{
	// Update the teacher
	chief = sorted_agents[0];
	std::vector<Agent*> group;

	while (sorted_agents.size() > 0)
	{
		int group_size = Random::get().randint(GROUP_SIZE_MIN, GROUP_SIZE_MAX + 1);
		if (group_size > sorted_agents.size())
		{
			group_size = sorted_agents.size();
		}
		for (int i = 0; i < group_size; i++)
		{
			int index = Random::get().randint(0, sorted_agents.size() - 1);
			group.push_back(sorted_agents[index]);
			sorted_agents.erase(sorted_agents.begin() + index);
		}
	
		debate_group(group);
	}
	sort_agents();
}

void Village::debate_group(std::vector<Agent*> group)
{
	Agent* teacher = nullptr;
	for (Agent* agent : group)
	{
		if (agent == chief)
		{
			teacher = agent;
			break;
		}
	}

	if (teacher != chief)
	{
		teacher = select_teacher(group);
	}

	for (Agent* agent : group)
	{
		if (agent != teacher)
			teacher->teach(agent);
	}
}
