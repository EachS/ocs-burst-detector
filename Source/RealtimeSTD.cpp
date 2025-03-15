#include "utils.h"
#include <complex>


/* RealtimeSTD */
RealtimeSTD::RealtimeSTD() : n(0), mean(0.0), M2(0.0) {}

void RealtimeSTD::addSample(double sample) {
	n++;
	this->sample = sample;
	double delta = sample - mean;
	mean += delta / n;
	M2 += delta * (sample - mean);
}

double RealtimeSTD::getSTD() {
	return std::sqrt(getVariance());
}

double RealtimeSTD::getResN(float n) {
	return mean + n * getSTD();
}

void RealtimeSTD::clear() {
	n = 0;
	mean = 0.0;
	M2 = 0.0;
}

double RealtimeSTD::getMean() {
	return mean;
}

double RealtimeSTD::getVariance()
{
	if (n < 2) {
		return 0.0001;
	}
	else {
		return M2 / (n - 1);
	}
}

double RealtimeSTD::get_z_score()
{
	return (sample - mean) / getSTD();
}
