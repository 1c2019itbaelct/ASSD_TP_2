#pragma once
#include <complex>
#include <vector>

#define LOG_N_FFT_MAX 12
#define N_FFT_MAX (1<<LOG_N_FFT_MAX)


class FFT_calculator
{
public:
	FFT_calculator();
	~FFT_calculator();

	void fft(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out);
	void ifft(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out);

private:
	std::complex<float> twiddleFactors[N_FFT_MAX/2 + 1];
	void bitReverse(std::vector <std::complex<float>>& out);

};

