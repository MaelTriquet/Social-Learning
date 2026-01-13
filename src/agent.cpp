#include "agent.hpp"

Agent::Agent()
: score(0)
{
	m_brain = Brain();
}

Agent::~Agent()
{
}

void Agent::think() {
	m_brain.feedforward(input, output);
};

void Agent::step() {
	see();
	think();
	act();
}

void Agent::teach(Agent* student) {
	if (!student->accept_teaching(this))
		return;
	for (int i = 0; i < BATCH_SIZE; i++)
	{
		for (int j = 0; j < INPUT_SIZE; j++)
		{
			input[j] = Random::get().rand(0, 1);
		}
		m_brain.feedforward(input, output);
		student->m_brain.feedforward(input, student->output);
		student->m_brain.back_propagate(output, 0.1);
	}
	student->m_brain.update_weights();
}

bool Agent::accept_teaching(Agent* teacher) {
	return true;
}
