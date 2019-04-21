#pragma once

#include <complex>
#include <vector>

#include "FFT_calculator.h"

#define N_FFT_ROBOT N_FFT_MAX

void robot_voice(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out, int n_fft = N_FFT_ROBOT);
