#include "OcsController.h"


OcsController::OcsController() :
    freqLow(8),
    freqHigh(12),
    fix_threshold(0.5),
    isDelayEnabled(false),
    USE_STFT(true),
    USE_Auto_TH(true),
    USE_Bandpassfilter(true),
    USE_Smooth(false),
    bandpassLow(0.6),
    bandpassHigh(150),
    sdft_type(SdftType::ZeroPaddingExp),
    sdft_window_size(1),
    STD_TH(2),
    USE_Minus_Average(true),
    smooth_theta_k(0.9),
    fs(30000),
    rfs(300),
    twindow(0.08),
    lightDur(0.2),
    ignoreDur(0.5),
    holdDur(0.2),
    clearDur(2),
    delayMin(0.5),
    delayMax(1.0),
    random_seed(72),
    RMS_nsamp(1000),
    SDFT_nfft(300),
    threshold(0.5),
    tmplightOn(false),
    tmpthetaCrossingOn(false),
    tsBuffer (0),
    sdft(nullptr)
{
    init();
}

OcsController::~OcsController()
{
}


void OcsController::parameterValueChange(std::string name, double value) {
    std::cout << "[Output Test] name: " << name << "    value: " << value << std::endl;
	if (name == "freq_low") {
		freqLow = value;
	}
	else if (name == "freq_high") {
		freqHigh = value;
	}
    else if (name == "threshold_type") {
        USE_Auto_TH = (ThresholdType::AUTO == (int)value);
    }
    else if (name == "fix_threshold") {
        fix_threshold = value;
    }
    else if (name == "Auto_STD_TH") {
        STD_TH = value;
    }
    else if (name == "use_bandpassfilter") {
        USE_Bandpassfilter = (value > 0.5);
    }
    else if (name == "low_cut") {
        bandpassLow = value;
    }
    else if (name == "high_cut") {
        bandpassHigh = value;
    }
    else if (name == "use_minus_Average") {
        USE_Minus_Average = (value > 0.5);
    }
    else if (name == "use_smooth") {
        USE_Smooth = (value > 0.5);
    }
    else if (name == "smooth_K") {
        smooth_theta_k = value;
    }
    else if (name == "rfs") {
        rfs = (int)value;
    }
    else if (name == "sdft_window_type") {
        sdft_type = static_cast<SdftType>(static_cast<int>(value));
    }
    else if (name == "sdft_window_size") {
        SDFT_nfft = (int)(rfs * value);
    }
    else if (name == "use_delay") {
        isDelayEnabled = (value > 0.5);
    }
    else if (name == "delay_min") {
        delayMin = value;
    }
    else if (name == "delay_max") {
        delayMax = value;
    }
    else if (name == "duration_time") {
        twindow = value;
    }
    else if (name == "light_duration_time") {
        lightDur = value;
    }
    else if (name == "ignore_duration_time") {
        ignoreDur = value;
    }
    else {
        std::cout << "[Error] UnKnow Param: " << name << "    " << value << std::endl;
    }
    init();
}

// int OcsController::process(int nSamples, const float ptrT)
std::tuple<int, double> OcsController::process(float sample)
{
    // auto [theta, ref] = monitor.checkHold(tsBuffer, ptrT, ptrR);
    if (USE_Bandpassfilter) {
        sample = bandpass.filter(sample);
    }

    if (USE_Minus_Average) {
        sample = slidingWindow.addSample(sample);
    }

    sdft->addSample(sample);
    std::vector<double> power = sdft->getBandPowerList();

    if (USE_Smooth) {
        smooth_power.addSamples(power);
        power = smooth_power.getRes();
    }

    if (USE_Auto_TH) {
        std_power.addSamples(power);
        switchController.setTH(std_power.getResN(STD_TH));
    }
    auto [lightOn, thetaCrossingOn, is_over] = \
        switchController.checkTH(power, tsBuffer, sample);
    int res = 0;
    if (tmpthetaCrossingOn ^ thetaCrossingOn) res |= 0b1000;
    if (thetaCrossingOn) res |= 0b0100;
    if (tmplightOn ^ lightOn) res |= 0b0010;
    if (lightOn) res |= 0b0001;
    
    tmplightOn = lightOn;
    tmpthetaCrossingOn = thetaCrossingOn;
    tsBuffer++;
    return std::make_tuple(res, power[0]);
}

std::vector<double> OcsController::process2(float sample)
{
    std::vector<double> res(21, 0);
    int n = sdft->get_n();
    res[0] = sample;
    
    if (USE_Bandpassfilter) {
        sample = bandpass.filter(sample);
    }
    res[1] = sample;

    if (USE_Minus_Average) {
        sample = slidingWindow.addSample(sample);
    }
    res[2] = sample;

    sdft->addSample(sample);
    std::vector<double> power = sdft->getBandPowerList();
    for (int i = 0; i < n; i++)
        res[i + 3] = power[i];

    if (USE_Smooth) {
        smooth_power.addSamples(power);
        power = smooth_power.getRes();
    }
    for (int i = 0; i < n; i++)
        res[i + 8] = power[i];

    if (USE_Auto_TH) {
        std_power.addSamples(power);
        switchController.setTH(std_power.getResN(STD_TH));
    }
    for (int i = 0; i < n; i++)
        res[i + 13] = std_power.getResN(STD_TH)[i];

    auto [lightOn, thetaCrossingOn, is_over] = \
        switchController.checkTH(power, tsBuffer, sample);
    res[18] = lightOn;
    res[19] = thetaCrossingOn;
    res[20] = is_over;

    tmplightOn = lightOn;
    tmpthetaCrossingOn = thetaCrossingOn;
    tsBuffer++;
    return res;
}

void OcsController::clear_all()
{
    tsBuffer = 0;
    tmplightOn = false;
    tmpthetaCrossingOn = false;

    std_power.clear();
    smooth_power.clear(); 
    bandpass.reset();
    slidingWindow.clear();
	sdft->clear();
	switchController.clear(tsBuffer);
}

void OcsController::init()
{
    switchController.twindow = twindow;
    switchController.lightDur = lightDur;
    switchController.ignoreDur = ignoreDur;
    switchController.holdDur = holdDur;
    switchController.clearDur = clearDur;
    switchController.random_seed = random_seed;
    switchController.random_delay.first = delayMin;
    switchController.random_delay.second = delayMax;
    switchController.isDelayEnabled = isDelayEnabled;
    switchController.sampleRate = fs;
    switchController.rfs = rfs;

    threshold = fix_threshold;
    switchController.setTH(threshold);

    smooth_power.setK(smooth_theta_k);

    // RMS
    // rms_theta.setSize(RMS_nsamp);

    // SDFT
    if (sdft == nullptr || sdft->type != sdft_type) {
        sdft = createSDFTInstance(sdft_type);
    }
    sdft->setSampleRate(rfs);
    sdft->setNfft(SDFT_nfft);
    if (freqLow < freqHigh) {
        sdft->setFreqs(freqLow, freqHigh);
    }

    slidingWindow.setWindowSize(SDFT_nfft);

    int n = sdft->get_n();
    switchController.setN(n);
    std_power.setN(n);
    smooth_power.setN(n);

    if (USE_Bandpassfilter && (bandpassLow < bandpassHigh)) {
        bandpass.setup(rfs, (bandpassLow + bandpassHigh) / 2.0, bandpassHigh - bandpassLow);
        bandpass.reset();
    }
}

std::unique_ptr<RealtimeSDFT> OcsController::createSDFTInstance(SdftType type) {
    if (SdftType::RECTANGLE == type) {
        return std::make_unique<RealtimeSDFT>();
    }
    else if (SdftType::EXP == type) {
        return std::make_unique<RealtimeExpSDFT>();
    }
    else if (SdftType::ZeroPaddingExp == type) {
        return std::make_unique<RealtimeZeroPaddingExpSDFT>();
    }
    else if (SdftType::MirrorExp == type) {
        return std::make_unique<RealtimeMirrorExpSDFT>();
    }
    else {
        throw std::invalid_argument("Invalid choice");
    }
}
