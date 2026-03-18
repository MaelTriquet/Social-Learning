#pragma once
#include <Eigen/Dense>

class Encoding
{
private:
	Eigen::VectorXf m_data;
public:
	Encoding(int size);
	~Encoding();
	float& operator[](int index);
	float operator[](int index) const;
};
