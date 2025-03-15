#include "utils.h"
#include <iostream>

RealtimeMirrorExpSDFT::RealtimeMirrorExpSDFT(int fmin, int fmax, int nfft, int sampleRate)
{
	type = SdftType::MirrorExp;
	init(fmin, fmax, nfft, sampleRate);
}

void RealtimeMirrorExpSDFT::init(int fmin, int fmax, int nfft, int sampleRate)
{
	RealtimeSDFT::init(fmin, fmax, nfft, sampleRate);

	fftout1.resize(nfft, std::complex<double>(0));
	fftout2.resize(nfft, std::complex<double>(0));

	int D = 20;
	double tau = nfft / 2.0 * 8.69 / D;

	tau_down = exp(-1 / tau);
	N2tau = exp(-(nfft / 2.0 / tau));

	coefs1.resize(nfft);
	coefs2.resize(nfft);
	for (int i = 0; i < nfft; i++) {
		coefs1[i] = std::complex<double>(std::cos(2 * M_PI * i / nfft), std::sin(2 * M_PI * i / nfft));
		coefs2[i] = std::complex<double>(std::cos(2 * M_PI * i / nfft), std::sin(-2 * M_PI * i / nfft));
	}
	PoolLength = nfft / 2 + 1;
}

void RealtimeMirrorExpSDFT::addSample(double in_sample) {
	std::complex<double> sample_Prev, sampleOld, sampleOldPrev;

	// asumming
	sample_Prev = sample[(index - 1 + PoolLength) % PoolLength];	// x(a+N/2)
	sampleOldPrev = sample[index];	// x(a)
	sampleOld = sample[(index + 1) % PoolLength];	// x(a+1)

	for (int i = min_idx; i <= max_idx; i++) {
		double AltSign = (i % 2 == 0) ? 1 : -1;
		fftout1[i] = (fftout1[i] + sample_Prev * AltSign - sampleOldPrev * N2tau) * tau_down * coefs1[i];
		fftout2[i] = fftout2[i] * tau_down * coefs2[i] + in_sample * AltSign - sampleOld * N2tau;
		fftout[i] = fftout1[i] + fftout2[i];
	}

	sample[index] = in_sample;
	index = (index + 1) % PoolLength;
}

void RealtimeMirrorExpSDFT::clear()
{
	init(fmin, fmax, nfft, rfs);
}

void RealtimeMirrorExpSDFT::setNfft(int SDFT_nfft)
{
	init(fmin, fmax, SDFT_nfft, rfs);
}

void RealtimeMirrorExpSDFT::setFreqs(int FreqMin, int FreqMax)
{
	init(FreqMin, FreqMax, nfft, rfs);
}

void RealtimeMirrorExpSDFT::setSampleRate(int samplerate)
{
	init(fmin, fmax, nfft, samplerate);
}