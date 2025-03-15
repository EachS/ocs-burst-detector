#include "utils.h"
#include <complex>
#include <iostream>


/* Smooth */
Smooth::Smooth(double k) : smoothed_sample(0.0), k(0.9)
{
	setK(k);
}

void Smooth::setK(double k)
{
	this->k = k;
}

void Smooth::addSample(double sample)
{
	smoothed_sample = k * sample + (1 - k) * smoothed_sample;
}

double Smooth::getRes()
{
	return smoothed_sample;
}

void Smooth::clear()
{
	smoothed_sample = 0.0;
}
