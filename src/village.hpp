#pragma once
#include "agent.hpp"
#include <vector>
#include "const.hpp"

class Village
{
private:
	std::vector<Agent*> m_agents;
	Agent* chief;
	std::vector<Agent*> sorted_agents;
	void sort_agents();

public:
	Village();
	~Village();

	void add_agent(Agent* agent);
	void remove_agent(int index);
	Agent* get_agent(int index);
	int get_size() const;
	
	void step();
	void evaluate();
	void debate();
	void debate_group(std::vector<Agent*> group);
	Agent* select_teacher(std::vector<Agent*> group);
};
