#pragma once
#include <random>

class Random
{
private:
    std::mt19937 m_gen;
    std::uniform_real_distribution<float> m_dist;

    // Private constructor — only accessible through instance()
    Random()
        : m_gen(std::random_device{}()), m_dist(0.0f, 1.0f)
    {}

public:
    // Delete copy/move operations to enforce singleton
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    static Random& get()
    {
        static Random instance;
        return instance;
    }

    // --- Random methods ---
    float rand()
    {
        return m_dist(m_gen);
    }

    float rand(float max)
    {
        return m_dist(m_gen) * max;
    }

    float rand(float min, float max)
    {
        return m_dist(m_gen) * (max - min) + min;
    }

    int randint(int max)
    {
        return m_dist(m_gen) * max;
    }

    int randint(int min, int max)
    {
        return m_dist(m_gen) * (max - min) + min;
    }

	template<typename T>
	void shuffle(std::vector<T>& vec)
	{
		int index1;
		int index2;
		for (int _ = 0; _ < vec.size(); _++)
		{
			index1 = randint(0, vec.size() - 1);
			index2 = randint(0, vec.size() - 1);
			std::swap(vec[index1], vec[index2]);
		}

	}
};

