//#include <ProcessorHeaders.h>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <vector>
#include <complex>


enum ThresholdType { CONSTANT = 0, AUTO };
enum SdftType { RECTANGLE = 1, EXP, ZeroPaddingExp, MirrorExp};


class Smooth
{
public:
	Smooth(double k = 0.9);
	void setK(double k);
	void addSample(double sample);
	double getRes();
	void clear();
private:
	double smoothed_sample;
	double k;
};


class RMS
{
public:
	RMS(int nsamp = 500);
	void setSize(int nsamp);
	void addSample(double sample);
	double getRes();
	void clear();

private:
	//juce::Array<double> buffer;
	int size;
	int index;
	double ss;
};

class RealtimeSTD
{
public:
	RealtimeSTD();
	void addSample(double sample);
	double getSTD();
	double getResN(float n);
	void clear();
	double getMean();
	double getVariance();
	double get_z_score();

private:
	int n;
	double mean, M2, c;
	double sample;
};


class SmoothList
{
public:
	SmoothList(int num = 10, double k = 0.9);
	~SmoothList();
	void setN(int num);
	void addSamples(std::vector<double> data);
	void setK(double k);
	std::vector<double> getRes();
	void clear();
	int num;

	std::vector<Smooth> list;
private:
	double k;
};


class STDList
{
public:
	STDList(int num = 10);
	~STDList();
	void setN(int num);
	void addSamples(std::vector<double> data);
	std::vector<double> getResN(float n);
	void clear();
	int num;

	std::vector<RealtimeSTD> list;
};


class RealtimeSDFT
{
public:
	int nfft;
	double rfs;
	int fmin;
	int fmax;
	int max_idx;
	int min_idx;
	int n;
	int n_out;
	std::vector<int> steps;
	std::vector<std::complex<double>> coefs;
	int index;
	std::vector<std::complex<double>> sample;
	std::vector<std::complex<double>> fftout;
	virtual void init(int fmin, int fmax, int nfft, int sampleRate);


	RealtimeSDFT(int fmin = 4, int fmax = 8, int nfft = 512, int sampleRate = 1000);
	virtual ~RealtimeSDFT() = default;
	virtual void setNfft(int SDFT_nfft);
	virtual void setFreqs(int FreqMin, int FreqMax);
	virtual void setSampleRate(int samplerate);
	virtual void clear();
	virtual void addSample(double sample);
	double getBandPower();
	std::vector<double> getBandPowerList();
	int get_n();
	SdftType type;
};


class RealtimeExpSDFT : public RealtimeSDFT
{
public:
	std::vector<std::complex<double>> fftout1;
	std::vector<std::complex<double>> fftout2;
	double downTau, upTau, W_1, W_2;

	bool reset_label;
	std::vector<std::complex<double>> fftout1_2;
	std::vector<std::complex<double>> fftout2_2;
	std::vector<std::complex<double>> sample_2;
	bool change_label = false;
	int reset_count = 0;

	RealtimeExpSDFT(int fmin = 4, int fmax = 8, int nfft = 1000, int sampleRate = 1000);

	void init(int fmin, int fmax, int nfft, int sampleRate) override;

	void addSample(double in_sample) override;

	void addSampleReset(std::complex<double> in_sample);

	double getFFTout1();

	double getFFTout2();

	void setNfft(int SDFT_nfft) override;
	void setFreqs(int FreqMin, int FreqMax) override;
	void setSampleRate(int samplerate) override;
	void clear() override;
};


class RealtimeMirrorExpSDFT : public RealtimeSDFT {
public:
	std::vector<std::complex<double>> fftout1;
	std::vector<std::complex<double>> fftout2;
	double tau_down, N2tau;
	std::vector<std::complex<double>> coefs1;
	std::vector<std::complex<double>> coefs2;
	int PoolLength;

	RealtimeMirrorExpSDFT(int fmin = 4, int fmax = 8, int nfft = 1000, int sampleRate = 1000);
	void init(int fmin, int fmax, int nfft, int sampleRate) override;
	void setNfft(int SDFT_nfft) override;
	void setFreqs(int FreqMin, int FreqMax) override;
	void setSampleRate(int samplerate) override;
	void addSample(double in_sample) override;
	void clear() override;
};


class RealtimeZeroPaddingExpSDFT : public RealtimeSDFT {
public:
	double tau_down, N2tau;
	int PoolLength;

	RealtimeZeroPaddingExpSDFT(int fmin = 4, int fmax = 8, int nfft = 1000, int sampleRate = 1000);
	void init(int fmin, int fmax, int nfft, int sampleRate) override;
	void addSample(double in_sample) override;
};


class SlidingWindow {
public:
	SlidingWindow();
	double addSample(double in_sample);
	void setWindowSize(size_t window_size);
	void clear();

private:
	size_t window_size_;
	double sum_;
	size_t index_;
	std::vector<double> buffer_;
};

