#pragma once

class Global
{
public:
	Global(const Global&) = delete;
	Global& operator=(const Global&) = delete;
	int interactions = 0;
	int weight_exploration = 0;
	int weight_alignment = 0;
	int add_connection = 0;
	int add_node = 0;
	int add_node_random = 0;
	Global() = default;
	~Global() = default;
	static Global& get()
	{
		static Global instance;
		return instance;
	}
};

