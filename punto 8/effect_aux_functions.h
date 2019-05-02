//
// Created by mlste on 5/1/2019.
//

#ifndef ROBOT_EFFECT_AUX_FUNCTIONS_H
#define ROBOT_EFFECT_AUX_FUNCTIONS_H

#include <vector>
#include <complex>

void window_stream(const std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out);
float hanning_window_factor( int i, int n_fft);


#endif //ROBOT_EFFECT_AUX_FUNCTIONS_H
