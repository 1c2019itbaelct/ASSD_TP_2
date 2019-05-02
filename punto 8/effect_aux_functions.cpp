//
// Created by mlste on 5/1/2019.
//

#include "effect_aux_functions.h"

using namespace std;

void window_stream(const std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out)
{
    int N = in.size();
    for (int i = 0; i < N; ++i)
    {
        out[i].real(in[i].real() * hanning_window_factor(i, N));
        out[i].imag(in[i].imag() * hanning_window_factor(i, N));
    }
}

/**
* @brief Devuelve el valor de la ventana de Hanning en un punto
*
* @param i numero de muestra dentro de la ventana (0 <= i < n_fft)
* @param n_fft cantidad de muestras de la ventana
*/
float hanning_window_factor( int i, int n_fft)
{
//	return 0.5;
    return 0.5 - 0.5 * cos(2*M_PI * (float)i/((float)n_fft-1));
}
