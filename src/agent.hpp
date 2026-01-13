#pragma once
#include "brain.hpp"

class Agent
{
protected:
	Brain m_brain;
	float input[INPUT_SIZE];
	float output[OUTPUT_SIZE];

public:
	float score;
	Agent();
	virtual ~Agent();

	virtual void see() {
	// This is where the agent perceives the world, aka fills the input vector for the brain
	// This function should end with input being filled with the correct values
	};

	virtual void act() {
		// This is where the agent acts, aka has actions on the world based on the output values
	};

	virtual void score_evaluation() {
		// This is where the agent evaluates its score
		// This function should end with score being filled with the correct value (higher score == better performance, not necessarely positive)
	};

	void think();
	void step();
	void teach(Agent* agent);
	bool accept_teaching(Agent* agent);
};
