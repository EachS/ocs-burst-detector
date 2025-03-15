#include "SwitchController.h"
#include <iostream>


SwitchController::SwitchController(int n, double threshold, int sampleRate, double twindow)
	: thetaCrossingOn(false), isLightOn(false),
	tsLightOff(-1), tsIgnore(-1), tsHoldDur(-1), tsDelayLight(-1), tsClear(-1), holdTheta(0),
	sampleRate(sampleRate), rfs(1000), twindow(twindow),
	lightDur(0.2), ignoreDur(1), holdDur(0.2), clearDur(2.0), random_delay(0.5, 1.0), random_seed(72),
	isDelayEnabled(false)
{
	random_engine.seed(random_seed);
	setN(n);
}

void SwitchController::lightOn(long long tsBuffer, double holdTheta)
{
	isLightOn = true;
	tsLightOff = tsBuffer + static_cast<int>(lightDur * rfs);
	tsIgnore = tsBuffer + static_cast<int>(ignoreDur * rfs);
	tsHoldDur = tsBuffer + static_cast<int>(holdDur * rfs);
	this->holdTheta = holdTheta;
	// std::cout << "Light ON\n";
}

void SwitchController::lightOff()
{
	isLightOn = false;
	tsLightOff = -1;
	// std::cout << "Light OFF\n";
}

void SwitchController::delayOn(long long tsBuffer)
{
	std::uniform_real_distribution<double> dist(random_delay.first, random_delay.second);
	double delay = dist(random_engine) * rfs;
	tsDelayLight = tsBuffer + static_cast<int>(delay);
}

void SwitchController::delayOff()
{
	tsDelayLight = -1;
}

void SwitchController::setDurAfterClear(long long tsBuffer)
{
	tsClear = tsBuffer + static_cast<int>(clearDur * rfs);
}

bool SwitchController::checkIgnoreThetaAfterLight(long long tsBuffer)
{
	return (tsBuffer >= tsIgnore) && (tsBuffer >= tsClear);
}

bool SwitchController::checkDelayFinish(long long tsBuffer)
{
	return (tsBuffer >= tsDelayLight) && (tsDelayLight > 0);
}

bool SwitchController::checkLightFinish(long long tsBuffer)
{
	return tsBuffer >= tsLightOff;
}

bool SwitchController::isCrossingOn()
{
	bool label = false;
	for (int i = 0; i < n; i++) {
		label = label || checkOverList[i].isup;
	}
	return label;
}

double SwitchController::checkHold(long long tsBuffer, double theta)
{
	if (tsBuffer >= tsHoldDur) {
		return theta;
	}
	else {
		return holdTheta;
	}
}

std::tuple<bool, bool, bool> SwitchController::checkTH(std::vector<double> out, long long tsBuffer, double theta)
{
	if (n < out.size()) {
		setN(out.size());
	}
	addSample(out, theta);
	bool crossing_on = isCrossingOn();

	if (isDelayEnabled) {
		if (crossing_on && checkIgnoreThetaAfterLight(tsBuffer) && !thetaCrossingOn) {
			thetaCrossingOn = true;
			delayOff();
		}
		else if (thetaCrossingOn && !crossing_on && checkIgnoreThetaAfterLight(tsBuffer)) {
			thetaCrossingOn = false;
			delayOn(tsBuffer);
		}
		else if (!crossing_on && checkDelayFinish(tsBuffer)) {
			lightOn(tsBuffer, theta);
			delayOff();
		}
	}
	else {
		if (crossing_on && checkIgnoreThetaAfterLight(tsBuffer) && !thetaCrossingOn) {
			thetaCrossingOn = true;
			// std::cout << "Theta Crossing On\n";
			lightOn(tsBuffer, theta);
		}
		else if (thetaCrossingOn && !crossing_on) {
			thetaCrossingOn = false;
			// std::cout << "Theta Crossing Off\n";
		}
	}

	if (isLightOn && checkLightFinish(tsBuffer)) {
		lightOff();
	}

	return std::tuple<bool, bool, bool>(isLightOn, thetaCrossingOn, crossing_on);
}

void SwitchController::addSample(std::vector<double> samples, double theta)
{
	for (int i = 0; i < samples.size(); i++) {
		checkOverList[i].addSample(samples[i], theta);
	}
}

void SwitchController::clear(long long tsBuffer)
{
	thetaCrossingOn = false;
	isLightOn = false;
	tsLightOff = -1;
	tsIgnore = -1;
	tsHoldDur = -1;
	tsDelayLight = -1;
	holdTheta = 0;
	setDurAfterClear(tsBuffer);

	for (int i = 0; i < n; i++) {
		checkOverList[i].clear();
	}
}

void SwitchController::setTH(std::vector<double> thresholds)
{
	for (int i = 0; i < thresholds.size(); i++) {
		checkOverList[i].threshold = thresholds[i];
	}
}

void SwitchController::setTH(double threshold)
{
	for (int i = 0; i < n; i++) {
		checkOverList[i].threshold = threshold;
	}
}

void SwitchController::setN(int n)
{
	this->n = n;
	checkOverList.resize(n);
	for (int i = 0; i < n; i++) {
		checkOverList[i] = checkOver(static_cast<int>(rfs * twindow));
	}
}


/* CheckOver */ 
checkOver::checkOver(int twindow): twindow(twindow)
{
	clear();
}

void checkOver::addSample(double sample, double theta)
{
	is_over = (sample >= threshold);
	if (is_over) {
		count++;
	}
	else {
		count = 0;
	}

	isprevup = isup;
	isup = (count > twindow);
}

void checkOver::clear()
{
	count = 0;
	isprevup = false;
	isup = false;
	is_over = false;
}

