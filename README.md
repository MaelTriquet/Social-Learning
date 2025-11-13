# Social Learning

## Why

I am fascinated by intelligent algorithms based on life and our understanding of it. But Reinforcement Learning zooms the learning process in just one agents life, while Genetic Algorithms are too broad and take into account every form of life from the very beginning. I believe that humans are a lucky find from nature's randomness (in terms of adapting and understanding), but that a human alone is nothing. I believe that humanity's progress comes from the multiple generations, from talking with one another, and that is what I am trying to simulate.

## How

This project is only at a Proof of Concept stage, but here are my ideas:

- I really like the idea of a Neural Network, I think it is a good way to represent the human brain
- I also really like the NEAT approach, of starting with an empty brain, and filling it as we go
- This will be a multi-agent model, where agents will "teach" each other
- An agent teaching another can be represented in 2 ways:
  - The teacher shows what they would do in a certain situation, and the receiver tries to adapt to it (simple backward propagation)
  - The teacher tries to teach a rule instead of a specific action. The teacher will pick one of it's topological mutation that the receiver does not have and make him mutate that way. If it is not possible, we find the first compatible mutation by going up the mutation tree from the selected mutation.
- Two agents can have a baby (the probability of having a baby is 1 over the rank of the worst rank of the two potential parents)
- For now, the baby is crossover/mutation as per the NEAT approach
- To keep diversity, there will be multiple niches, or villages if you will, and travelling from one village to another will be possible, although not likely to keep the distinction from every village
- Each agent is limited to 2 children maximum, but the best performing one is not
- Each generation is limited to a certain number of agents
- When a child is born, one of its great-grandparents will randomly die (to prevent population explosion. That way, the population per village is at most 3 times the maximum number of agents per generation)
- The best performing agent will be imune to death
- Each iteration will go as follows:
  - A subset of the population is selected to be the talkers
  - The talkers are grouped in very small groups (2 to 5)
  - Each group selects the teacher (best performing agent among them)
  - The teacher will teach the group
  - The teacher will pick a random agent from the group and try to have a baby with it
- Each agent will have a stuborness value, that will start of low (when just born) and will end high (when old). This stuborness value will make them more gullible (prone to accept blindly the teaching material) or stuborn (reject the teaching)
