#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#ifndef OCSCONTROLLER_H
#define OCSCONTROLLER_H

#include "iir/Butterworth.h"
#include "SwitchController.h"
#include "utils.h"


class OcsController
{
public:
	OcsController();
	~OcsController();

	float getFreqLow();
	float getFreqHigh();
	float getBandpassLow();
	float getBandpassHigh();
	float getFixThreshold();
	float getDelayMin();
	float getDelayMax();
	int getThresholdType();
	float getThreshold();
	float getStdTH();
	float getSmoothK();
	int getSdftType();
	float getSdftWindowSize();
	float getDurTime();
	float getLightDur();
	float getIgnoreDur();

	bool get_use_delay();
	bool get_use_stft();
	bool get_use_auto_th();
	bool get_use_minus_average();
	bool get_use_bandpass_filter();
	bool get_use_smooth();

	int get_rfs_factor();
	int get_fs();
	int get_rfs();

	void parameterValueChange(std::string name, double value);

	std::tuple<int, double> process(float ptrT);
	std::vector<double> process2(float ptrT);

	void clear_all();
	void init();

	std::unique_ptr<RealtimeSDFT> createSDFTInstance(SdftType type);

private:
	float freqHigh, freqLow;

	bool USE_Auto_TH;
	float threshold, fix_threshold, STD_TH;

	bool USE_Bandpassfilter;
	float bandpassLow, bandpassHigh;
	
	bool USE_Minus_Average;
	bool USE_Smooth;
	float smooth_theta_k;

	bool USE_STFT;
	SdftType sdft_type;
	float sdft_window_size;

	bool isDelayEnabled;
	float delayMin, delayMax;

	Iir::Butterworth::BandPass<1> bandpass;

	// RealtimeSTD out_std;
	SmoothList  smooth_power;

	// Smooth smooth_theta;
	STDList std_power;
	int fs, rfs;

	RMS rms_ref, rms_theta;
	int RMS_nsamp;

	std::unique_ptr<RealtimeSDFT> sdft;
	int SDFT_nfft;

	SwitchController switchController;
	SlidingWindow slidingWindow;
	float twindow;
	float lightDur, ignoreDur, holdDur, clearDur;

	int random_seed;

	bool tmplightOn, tmpthetaCrossingOn;

	long long tsBuffer;
};

inline float OcsController::getFreqLow()
{
	return freqLow;
}

inline float OcsController::getFreqHigh()
{
	return freqHigh;
}

inline float OcsController::getBandpassLow()
{
	return bandpassLow;
}

inline float OcsController::getBandpassHigh()
{
	return bandpassHigh;
}

inline float OcsController::getFixThreshold()
{
	return fix_threshold;
}

inline float OcsController::getDelayMin()
{
	return delayMin;
}

inline float OcsController::getDelayMax()
{
	return delayMax;
}

inline int OcsController::getThresholdType()
{
	if (USE_Auto_TH)
		return ThresholdType::AUTO;
	return ThresholdType::CONSTANT;
}

inline float OcsController::getThreshold()
{
	return threshold;
}

inline float OcsController::getStdTH()
{
	return STD_TH;
}

inline float OcsController::getSmoothK()
{
	return smooth_theta_k;
}

inline int OcsController::getSdftType()
{
	return sdft_type;
}

inline float OcsController::getSdftWindowSize()
{
	return sdft_window_size;
}

inline float OcsController::getDurTime()
{
	return twindow;
}

inline float OcsController::getLightDur()
{
	return lightDur;
}

inline float OcsController::getIgnoreDur()
{
	return ignoreDur;
}

inline bool OcsController::get_use_delay()
{
	return isDelayEnabled;
}

inline bool OcsController::get_use_stft()
{
	return USE_STFT;
}

inline bool OcsController::get_use_auto_th()
{
	return USE_Auto_TH;
}

inline bool OcsController::get_use_minus_average()
{
	return USE_Minus_Average;
}

inline bool OcsController::get_use_bandpass_filter()
{
	return USE_Bandpassfilter;
}

inline bool OcsController::get_use_smooth()
{
	return USE_Smooth;
}

inline int OcsController::get_rfs_factor()
{
	return (int)(fs/rfs); 
}

inline int OcsController::get_fs()
{
	return fs;
}

inline int OcsController::get_rfs()
{
	return rfs;
}

#endif