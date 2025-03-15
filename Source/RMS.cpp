#include "utils.h"
#include <complex>


/* RMS */
RMS::RMS(int nsamp) {
	size = nsamp;
	clear();
}

void RMS::setSize(int nsamp) {
	size = nsamp;
	clear();
}

void RMS::addSample(double sample) {
	/*
	double sq = sample * sample;
	ss = ss - buffer[index] + sq;
	buffer.set(index, sq);
	index = (index + 1) % size;
	*/

}

double RMS::getRes() {
	return sqrt(ss / size);
}

void RMS::clear() {
	/*
	buffer.clear();
	buffer.insertMultiple(0, 0, size);
	index = 0;
	ss = 0;*/
}