# include "brain.hpp"

int main()
{
	Brain brain;
	
	// Build XOR network: 2 inputs -> 2 hidden -> 1 output with bias
	// Add hidden layer nodes
	brain.test_add_node(0); // Add node on first input->output connection
	brain.test_add_node(1); // Add node on second input->output connection
	
	// Add connections to the hidden layer
	brain.test_add_connection(0, 5, Random::get().rand(-1, 1)); 
	brain.test_add_connection(1, 4, Random::get().rand(-1, 1)); 

	// The architecture is now:
	//
	// 0	4
	// 1	5	2
	// 3 (bias)
	
	brain.print();
	
	// XOR training data
	float xor_inputs[4][INPUT_SIZE] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
	float xor_targets[4][OUTPUT_SIZE] = {{0}, {1}, {1}, {0}};
	
	float output[OUTPUT_SIZE];
	float learning_rate = 0.1f;
	
	for (int epoch = 0; epoch < 10000; epoch++)
	{
		float total_error = 0.0f;
		
		// Batch training: iterate through all XOR examples
		for (int sample = 0; sample < 4; sample++)
		{
			// Set input
			float input[INPUT_SIZE];
			for (int i = 0; i < INPUT_SIZE; i++)
			{
				input[i] = xor_inputs[sample][i];
			}
			
			// Forward pass
			brain.feedforward(input, output);
			
			// Calculate error for this sample
			float sample_error = output[0] - xor_targets[sample][0];
			total_error += sample_error * sample_error;
			
			// Backward pass (accumulate gradients)
			brain.back_propagate(xor_targets[sample], learning_rate);
			
			// Print progress every 1000 epochs
			if (epoch % 1000 == 0 && sample == 0)
			{
				std::cout << "Epoch " << epoch << ": [" 
						  << input[0] << ", " << input[1] << "] -> " 
						  << output[0] << " (target: " << xor_targets[sample][0] << ")" << std::endl;
			}
		}
		
		// Update weights after processing all samples
		brain.update_weights();
		
		// Print error every 1000 epochs
		if (epoch % 1000 == 0)
		{
			std::cout << "Total Error: " << total_error << std::endl << std::endl;
		}
		
		// Stop if error is small enough
		if (total_error < 0.01f)
		{
			std::cout << "Converged after " << epoch << " epochs!" << std::endl;
			break;
		}
	}
	
	// Test the trained network
	std::cout << "\nFinal Test Results:" << std::endl;
	for (int sample = 0; sample < 4; sample++)
	{
		float input[INPUT_SIZE];
		for (int i = 0; i < INPUT_SIZE; i++)
		{
			input[i] = xor_inputs[sample][i];
		}
		
		brain.feedforward(input, output);
		std::cout << "[" << input[0] << ", " << input[1] << "] -> " 
				  << output[0] << " (target: " << xor_targets[sample][0] << ")" << std::endl;
	}
	
	brain.print();
	return 0;
}
