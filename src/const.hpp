#include "encoding.hpp"

#define MUT_ADD_NODE 0.05
#define MUT_ADD_CONNECTION 0.05
#define MUT_WEIGHT 0.05

#define INPUT_SIZE 3
#define OUTPUT_SIZE 1

#define RELU 0
#define SIGMOID 1
#define TANH 2
#define IDENTITY 3
#define BIAS 4
#define SIGMOID_SCALE 4.0f
#define TANH_SCALE 2.0f

#define HIDDEN_ACTIVATION TANH
#define OUTPUT_ACTIVATION SIGMOID

#define CLAMP_WEIGHTS true
#define CLAMP 1.f

#define CHAIN_SIZE 3
#define EXAMPLE_SIZE 1
#define GROUP_SIZE 2

#define ENCODING Encoding<INPUT_SIZE + 1>
#define CLUSTER_THRESHOLD 0.75f
#define GROUP_THRESHOLD 0.3f

#define INSIDE_GROUP_PROBABILITY 0.95f
#define DEBATE_ALIGNMENT 0.6f
