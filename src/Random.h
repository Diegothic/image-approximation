#pragma once

#include <random>

class Random {

public:

	Random() = default;
	virtual ~Random() = default;

	uint16_t rangedInt(const uint16_t& min, const uint16_t& max);
	double rangedDouble(const double& min, const double& max);

private:

	std::random_device m_device;
};
