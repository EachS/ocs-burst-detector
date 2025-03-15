#include "utils.h"
#include <iostream>


RealtimeZeroPaddingExpSDFT::RealtimeZeroPaddingExpSDFT(int fmin, int fmax, int nfft, int sampleRate)
{
	type = SdftType::ZeroPaddingExp;
	init(fmin, fmax, nfft, sampleRate);
}

void RealtimeZeroPaddingExpSDFT::init(int fmin, int fmax, int nfft, int sampleRate)
{
	RealtimeSDFT::init(fmin, fmax, nfft, sampleRate);

	int D = 20;
	double tau = nfft / 2.0 * 8.69 / D;

	tau_down = exp(-1 / tau);
	N2tau = exp(-(nfft / 2.0 / tau));

	PoolLength = nfft / 2 + 1;
}

void RealtimeZeroPaddingExpSDFT::addSample(double in_sample) {
	std::complex<double> sampleOldPrev;

	sampleOldPrev = sample[index];	// x(a)

	double AltSign = (min_idx % 2 == 0) ? 1 : -1;
	for (int i = min_idx; i <= max_idx; i++) {
		fftout[i] = (fftout[i] - sampleOldPrev * N2tau) * tau_down * coefs[i] + in_sample * AltSign;
		AltSign = -AltSign;
	}

	sample[index] = in_sample;
	index = (index + 1) % PoolLength;
}
