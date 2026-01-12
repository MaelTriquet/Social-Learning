# AGENTS.md

This file contains guidelines for agentic coding agents working on the NEAT (NeuroEvolution of Augmenting Topologies) C++ project.

## Build Commands

### Building the Project
```bash
make                    # Build the entire project
make clean             # Remove all build artifacts (bin/ and obj/ directories)
```

### Running the Project
```bash
./bin/neat             # Run the compiled NEAT executable
```

### Build System Details
- Uses GNU Make with g++ compiler
- Target executable: `bin/neat`
- Object files stored in: `obj/`
- Compiler flags: `-Wall -Wextra -std=c++17 -O2`
- Source files located in: `src/`

## Code Style Guidelines

### File Organization
- Header files (`.hpp`) use `#pragma once` for include guards
- Source files (`.cpp`) include corresponding headers first
- Constants defined in `const.hpp`
- Class declarations in headers, implementations in source files

### Naming Conventions
- **Classes**: PascalCase (e.g., `Brain`, `Node`, `Connection`)
- **Variables**: snake_case with member prefix `m_` (e.g., `m_nodes`, `m_connections`)
- **Functions**: snake_case (e.g., `add_node`, `feedforward`, `back_propagate`)
- **Constants**: UPPER_CASE (e.g., `INPUT_SIZE`, `OUTPUT_SIZE`, `RELU`)
- **Private members**: prefixed with `m_` (e.g., `m_act_fun_idx`, `m_layers`)

### Import/Include Style
- System includes: `#include <iostream>`, `#include <vector>`, etc.
- Local includes: `#include "brain.hpp"`, `#include "node.hpp"`, etc.
- Include order: system headers first, then local headers
- Use forward declarations when possible to reduce compile dependencies

### Formatting
- Indentation: Tabs (consistent across codebase)
- Brace style: Allman style (opening brace on new line)
- Spacing: Single spaces around operators
- Line endings: Unix-style (\n)

### Type System
- Use standard C++ types: `int`, `float`, `bool`
- Prefer `float` for neural network computations
- Use `std::vector<T*>` for collections of pointers
- Use `const` for member functions that don't modify state
- Use references for parameters when appropriate, pointers for ownership

### Error Handling
- No exceptions used in current codebase
- Use assertions for debugging (not currently present but recommended)
- Return values for error conditions (consider adding error codes)
- Memory management: Manual new/delete with RAII in destructors

### Memory Management
- Manual memory management with `new`/`delete`
- Destructors clean up all allocated memory
- Use raw pointers for object relationships
- Be careful with circular references (Node ↔ Connection)

### Class Design Patterns
- **Brain**: Main neural network container, manages nodes and connections
- **Node**: Represents neurons with activation functions
- **Connection**: Represents synaptic connections between nodes
- **History**: Tracks innovation numbers for NEAT algorithm
- **Random**: Singleton for random number generation

### Constants and Configuration
- Network topology constants in `const.hpp`
- Activation function indices: `RELU=0`, `SIGMOID=1`, `TANH=2`, `IDENTITY=3`
- Input/output sizes configurable via macros

### Testing
- Test methods prefixed with `test_` (e.g., `test_add_node`, `test_add_connection`)
- Main function contains integration test for XOR-like problem
- No formal testing framework - use simple assertions and output

### Documentation
- Minimal inline comments
- Class member documentation through naming conventions
- Consider adding Doxygen-style comments for public APIs

### Performance Considerations
- Optimization flag `-O2` enabled in release builds
- Use `std::vector` for dynamic arrays with good cache locality
- Avoid unnecessary allocations in tight loops
- Consider using object pools for frequent Node/Connection creation

### Git Workflow
- Main development branch: `main`
- Feature branch: `PoC` (Proof of Concept)
- Conventional commits not enforced but recommended

## Project-Specific Guidelines

### NEAT Algorithm Implementation
- Maintain innovation numbers through History class
- Track node and connection genealogy
- Implement proper crossover and mutation operators
- Preserve species diversity (planned feature)

### Neural Network Architecture
- Layer-based organization with explicit layer numbers
- Support for multiple activation functions per node
- Bias nodes handled as special activation function type
- Feedforward propagation with topological sorting

### Code Evolution
- This is a research/prototype project
- Focus on correctness over optimization initially
- Modular design allows easy experimentation with different NEAT variants
- Keep the core algorithm clean and well-separated from test code