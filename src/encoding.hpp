#pragma once
#include <array>
#include <cmath>

template <int N>
class Encoding
{
	std::array<float, N> m_values;

public:
	Encoding()
	{
		for (int i = 0; i < N; i++)
			m_values[i] = 0;
	}

	Encoding(const Encoding& other)
	{
		for (int i = 0; i < N; i++)
			m_values[i] = other.m_values[i];
	}

	float& operator[](int index)
	{
		return m_values[index];
	}

	const float& operator[](int index) const
	{
		return m_values[index];
	}

	float& operator()(int index)
	{
		return m_values[index];
	}

	bool operator==(const Encoding& other) const
	{
		for (int i = 0; i < N; i++)
			if (m_values[i] != other.m_values[i])
				return false;
		return true;
	}

	bool operator!=(const Encoding& other) const
	{
		return not this == other;
	}

	Encoding& operator+=(const Encoding& other)
	{
		for (int i = 0; i < N; i++)
			m_values[i] += other.m_values[i];
		return *this;
	}
	
	Encoding& operator-=(const Encoding& other)
	{
		for (int i = 0; i < N; i++)
			m_values[i] -= other.m_values[i];
		return *this;
	}

	Encoding& operator*=(float scalar)
	{
		for (int i = 0; i < N; i++)
			m_values[i] *= scalar;
		return *this;
	}

	Encoding& operator/=(float scalar)
	{
		for (int i = 0; i < N; i++)
			m_values[i] /= scalar;
		return *this;
	}

	float norm() const
	{
		float sum = 0;
		for (int i = 0; i < N; i++)
			sum += m_values[i] * m_values[i];
		return std::sqrt(sum);
	}

	void normalize()
	{
		float norm = this->norm();
		if (norm == 0)
			return;
		this->operator/=(norm);
	}

	// Current norm() computes sqrt — add a norm_sq for cases where you don't need sqrt
	float norm_sq() const
	{
		float sum = 0;
		for (int i = 0; i < N; i++)
			sum += m_values[i] * m_values[i];
		return sum;
	}

	Encoding normalized() const
	{
		float n = norm();
		if (n == 0.0f) return Encoding();
		return *this / n; // uses existing operator/
	}

	float dot(const Encoding& other) const
	{
		float sum = 0;
		for (int i = 0; i < N; i++)
			sum += m_values[i] * other.m_values[i];
		return sum;
	}

	void operator=(const Encoding& other)
	{
		for (int i = 0; i < N; i++)
			m_values[i] = other.m_values[i];
	}

	Encoding operator+(const Encoding& other) const
	{
		Encoding result;
		for (int i = 0; i < N; i++)
			result.m_values[i] = m_values[i] + other.m_values[i];
		return result;
	}

	Encoding operator-(const Encoding& other) const
	{
		Encoding result;
		for (int i = 0; i < N; i++)
			result.m_values[i] = m_values[i] - other.m_values[i];
		return result;
	}

	Encoding operator*(float scalar) const
	{
		Encoding result;
		for (int i = 0; i < N; i++)
			result.m_values[i] = m_values[i] * scalar;
		return result;
	}

	Encoding operator/(float scalar) const
	{
		Encoding result;
		for (int i = 0; i < N; i++)
			result.m_values[i] = m_values[i] / scalar;
		return result;
	}

	static Encoding Zero()
	{
		Encoding result;
		return result;
	}

	int transpose()
	{
		return 0;
	}
};
