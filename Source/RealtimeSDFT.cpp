#include "utils.h"
#include <iostream>



/* RealtimeSDFT */
RealtimeSDFT::RealtimeSDFT(int fmin, int fmax, int nfft, int sampleRate) : nfft(nfft), rfs(sampleRate), fmin(fmin), fmax(fmax) {
	type = SdftType::RECTANGLE;
	init(fmin, fmax, nfft, sampleRate);
}

void RealtimeSDFT::init(int fmin, int fmax, int nfft, int sampleRate)
{
	this->nfft = nfft;
	this->rfs = sampleRate;
	this->fmin = fmin;
	this->fmax = fmax;
	rfs = sampleRate;
	max_idx = std::min((fmax * nfft) / (int)rfs, nfft);
	min_idx = std::max((fmin * nfft) / (int)rfs, 0);
	n = max_idx - min_idx + 1;
	n_out = std::min(n, 5);
	steps.resize(n_out, n/n_out);
	for (int i = 0; i < n_out; i++) steps[i] = n / n_out;
	for (int i = 0; i < n%n_out; i++) {
		steps[i] = steps[i] + 1;
	}

	coefs.resize(nfft);
	for (int i = 0; i < nfft; i++) {
		coefs[i] = std::complex<double>(std::cos(2 * M_PI * i / nfft), std::sin(2 * M_PI * i / nfft));
	}
	index = 0;
	sample.resize(nfft, std::complex<double>(0));
	fftout.resize(nfft, std::complex<double>(0));
}

void RealtimeSDFT::setNfft(int SDFT_nfft)
{
	init(fmin, fmax, SDFT_nfft, rfs);
}

void RealtimeSDFT::setFreqs(int FreqMin, int FreqMax)
{
	init(FreqMin, FreqMax, nfft, rfs);
}

void RealtimeSDFT::setSampleRate(int samplerate)
{
	init(fmin, fmax, nfft, samplerate);
}

void RealtimeSDFT::clear() {
	index = 0;
	sample.assign(nfft, std::complex<double>(0));
	fftout.assign(nfft, std::complex<double>(0));
}

void RealtimeSDFT::addSample(double in_sample) {
	std::complex<double> delta = in_sample - sample[index];
	sample[index] = in_sample;
	for (int i = min_idx; i <= max_idx; i++) {
		fftout[i] = (fftout[i] + delta) * coefs[i];
	}
	index = (index + 1) % nfft;
}

double RealtimeSDFT::getBandPower() {
	// int imin = fmin * nfft / rfs + 1;
	// int imax = fmax * nfft / rfs;
	double power = 0;
	for (int i = min_idx; i <= max_idx; i++) {
		power += std::abs(fftout[i]);
	}
	return power / (max_idx - min_idx + 1) / nfft;
}

std::vector<double> RealtimeSDFT::getBandPowerList()
{
	/*
	std::vector<double> powers(n, 0);
	for (int i = min_idx; i <= max_idx; i++) {
		std::complex<double> v = fftout[i];
		powers[i-min_idx] = (std::pow(v.real(), 2) + std::pow(v.imag(), 2)) / nfft;
	}
	return powers;
	*/
	std::vector<double> powers(n_out, 0);
	int index = min_idx;
	for (int i = 0; i < n_out; i++) {
		double out = 0;
		for (int j = 0; j < steps[i]; j++) {
			out += std::abs(fftout[index]);
			index++;
		}
		powers[i] = out / nfft / steps[i];
	}
	return powers;
}

int RealtimeSDFT::get_n()
{
	return n_out;
}
