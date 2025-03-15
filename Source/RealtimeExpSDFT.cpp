#include "utils.h"
#include <iostream>

RealtimeExpSDFT::RealtimeExpSDFT(int fmin, int fmax, int nfft, int sampleRate)
{
	type = SdftType::EXP;
	init(fmin, fmax, nfft, sampleRate);
}

void RealtimeExpSDFT::init(int fmin, int fmax, int nfft, int sampleRate)
{
	RealtimeSDFT::init(fmin, fmax, nfft, sampleRate);

	fftout1.resize(nfft, std::complex<double>(0));
	fftout2.resize(nfft, std::complex<double>(0));

	int D = 20;
	double tau = nfft / 2.0 * 8.69 / D;
	downTau = exp(-1 / tau);
	upTau = exp(1 / tau);
	W_1 = exp(-(1.0 + nfft / 2.0) / tau);
	W_2 = exp(-abs(nfft / 2.0 - 1.0) / tau);
	reset_label = true;
	change_label = true;	// 1
	reset_count = 0;
	if (reset_label) {
		sample_2.resize(nfft, std::complex<double>(0));
		fftout1_2.resize(nfft, std::complex<double>(0));
		fftout2_2.resize(nfft, std::complex<double>(0));
	}
}

void RealtimeExpSDFT::addSample(double in_sample) {
	if (reset_label) {
		addSampleReset(in_sample);
		return;
	}

	std::complex<double> x_a1, x_aN_1;
	x_a1 = sample[index];
	x_aN_1 = in_sample;
	std::complex<double> x_aN2 = sample[(index + nfft / 2 + 1) % nfft];
	sample[index] = x_aN_1;

	for (int i = min_idx; i <= max_idx; i++) {
		fftout1[i] = coefs[i] * (downTau * fftout1[i] - x_a1 * W_1);
		fftout2[i] = coefs[i] * (upTau * fftout2[i] + x_aN_1 * W_2);

		fftout[i] = fftout1[i] + fftout2[i];
		if (i % 2) { // ÆæÊý
			fftout1[i] -= x_aN2;
			fftout2[i] += x_aN2;
		}
		else {
			fftout1[i] += x_aN2;
			fftout2[i] -= x_aN2;
		}
		fftout[i] = fftout1[i] + fftout2[i];
	}
	index = (index + 1) % nfft;
}

void RealtimeExpSDFT::addSampleReset(std::complex<double> in_sample) {
	reset_count++;
	if (reset_count > nfft * 3) {
		// std::cout << "change\n";
		if (change_label) { // 1 -> 2
			// fftout1_2.clear();
			// fftout1_2.resize(nfft, std::complex<double>(0));
			fftout2_2.clear();
			fftout2_2.resize(nfft, std::complex<double>(0));
			sample_2.clear();
			sample_2.resize(nfft, std::complex<double>(0));
		}
		else { // 2 -> 1
			// fftout1.clear();
			// fftout1.resize(nfft, std::complex<double>(0));
			fftout2.clear();
			fftout2.resize(nfft, std::complex<double>(0));
			sample.clear();
			sample.resize(nfft, std::complex<double>(0));
		}
		change_label = !change_label;
		reset_count = 0;
	}

	std::complex<double> x_a1, x_a1_2;
	x_a1 = sample[index];
	x_a1_2 = sample_2[index];

	std::complex<double> x_aN2 = sample[(index + nfft / 2 + 1) % nfft];
	std::complex<double> x_aN2_2 = sample_2[(index + nfft / 2 + 1) % nfft];
	sample[index] = in_sample;
	sample_2[index] = in_sample;

	for (int i = min_idx; i <= max_idx; i++) {
		fftout1[i] = coefs[i] * (downTau * fftout1[i] - x_a1 * W_1);
		fftout1_2[i] = coefs[i] * (downTau * fftout1_2[i] - x_a1_2 * W_1);
		fftout2[i] = coefs[i] * (upTau * fftout2[i] + in_sample * W_2);
		fftout2_2[i] = coefs[i] * (upTau * fftout2_2[i] + in_sample * W_2);

		if (i % 2) { // ÆæÊý
			fftout1[i] -= x_aN2;
			fftout1_2[i] -= x_aN2_2;
			fftout2[i] += x_aN2;
			fftout2_2[i] += x_aN2_2;
		}
		else {
			fftout1[i] += x_aN2;
			fftout1_2[i] += x_aN2_2;
			fftout2[i] -= x_aN2;
			fftout2_2[i] -= x_aN2_2;
		}

		if (change_label) {
			fftout[i] = fftout1_2[i] + fftout2_2[i];
		}
		else {
			fftout[i] = fftout1[i] + fftout2[i];
		}
	}
	index = (index + 1) % nfft;
}

double RealtimeExpSDFT::getFFTout1() {
	// int imin = fmin * nfft / rfs + 1;
	// int imax = fmax * nfft / rfs;
	double power = 0;
	for (int i = min_idx; i <= max_idx; i++) {
		std::complex<double> v = fftout1[i];
		power += std::pow(v.real(), 2) + std::pow(v.imag(), 2);
	}
	return power / (max_idx - min_idx + 1) / nfft;
}

double RealtimeExpSDFT::getFFTout2() {
	// int imin = fmin * nfft / rfs + 1;
	// int imax = fmax * nfft / rfs;
	double power = 0;
	for (int i = min_idx; i <= max_idx; i++) {
		std::complex<double> v;
		if (reset_label && !change_label) {
			v = fftout2_2[i];
		}
		else {
			v = fftout2[i];
		}
		power += std::pow(v.real(), 2) + std::pow(v.imag(), 2);
	}
	return power / (max_idx - min_idx + 1) / nfft;
}

void RealtimeExpSDFT::setNfft(int SDFT_nfft)
{
	init(fmin, fmax, SDFT_nfft, rfs);
}

void RealtimeExpSDFT::setFreqs(int FreqMin, int FreqMax)
{
	init(FreqMin, FreqMax, nfft, rfs);
}

void RealtimeExpSDFT::setSampleRate(int samplerate)
{
	init(fmin, fmax, nfft, samplerate);
}

void RealtimeExpSDFT::clear()
{
	init(fmin, fmax, nfft, rfs);
}
