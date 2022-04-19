#include "Random.h"

Random::Random() : m_device{} {

}

uint16_t Random::rangedInt(const uint16_t& min, const uint16_t& max) {

	std::default_random_engine eng(m_device());
	std::uniform_int_distribution<> dist(min, max);
	return dist(eng);
}

double Random::rangedDouble(const double& min, const double& max) {

	std::default_random_engine eng(m_device());
	std::uniform_real_distribution<> dist(min, max);
	return dist(eng);
}
