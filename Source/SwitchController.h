#pragma once

#include <random>

class checkOver
{
public:
    checkOver(int twindow=0);

    void addSample(double sample, double theta);
    void clear();

    int count;
    bool isprevup;
    bool isup;
    bool is_over;
    int twindow;

    double threshold;
};

class SwitchController {
public:
	SwitchController(int n=10, double threshold = 0.5, int sampleRate = 1000, double twindow = 0.2);
    void lightOn(long long tsBuffer, double holdTheta);
    void lightOff();
    void delayOn(long long tsBuffer);
    void delayOff();
    void setDurAfterClear(long long tsBuffer);
    bool checkIgnoreThetaAfterLight(long long tsBuffer);
    bool checkDelayFinish(long long tsBuffer);
    bool checkLightFinish(long long tsBuffer);
    bool isCrossingOn();
    double checkHold(long long tsBuffer, double theta);

    std::tuple<bool, bool, bool> checkTH(std::vector<double> out, long long tsBuffer, double theta);
    void addSample(std::vector<double> samples, double theta);
    void clear(long long tsBuffer);
    void setTH(std::vector<double> thresholds);
    void setTH(double threshold);
    void setN(int n);

    int sampleRate;
    int rfs;
    double lightDur;
    double ignoreDur;
    double holdDur;
    double clearDur;
    std::pair<double, double> random_delay;
    int random_seed;
    bool isDelayEnabled;

    bool thetaCrossingOn;
    bool isLightOn;
    int tsLightOff;
    int tsIgnore;
    int tsHoldDur;
    int tsDelayLight;
    int tsClear;
    double holdTheta;
    double twindow;

    int n;
    std::vector<checkOver> checkOverList;

    std::default_random_engine random_engine;
};


