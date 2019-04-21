#include "audioEffects.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <algorithm>

#include "FFT_calculator.h"

// #define DEBUG_ROBOT
// #define ANDROID_VOICE
using namespace std;

float hanning_window_factor(int i, int n_fft);


/**
 * @brief Genera efecto robot en una voz suprimiendo la fase de su espectro.
 * 
 * @param in Vector que contiene las muestras de voz
 * @param out Vector donde se guarda las muestras de la voz robotizada. Debe ser distinto de @code in @endcode
 * @param n_fft Longitud de las ventana utilizadas. Si no es una potencia de 2, se trunca a la potencia de 2 inferior mas cercana
 */
void robot_voice(vector<complex<float>>& in, vector<complex<float>>& out, int n_fft)
{
	n_fft = 1 << ((int)log2(n_fft));	// Trunco a la potencia de 2 mas cercana.

	vector<complex<float>> aux(n_fft);
	out.clear();
	out.resize(in.size());

	// Recorro todas las ventanas:
	for (int first_window_sample = 0; first_window_sample + n_fft < in.size(); first_window_sample += n_fft / 2)
	{
#ifdef DEBUG_ROBOT
		cout << endl << "Ventana # " << first_window_sample /(n_fft / 2) << endl;
#endif //DEBUG_ROBOT
		//Recorro todas las muestras dentro de una ventana
		for(int sample_offset = 0; sample_offset < n_fft; sample_offset++)
		{
			aux[sample_offset] = in[first_window_sample + sample_offset] * hanning_window_factor(sample_offset, n_fft);
		}
		FFT_calculator::fft(aux, aux);
		aux.resize(n_fft / 2);	// Elimino las muestras de la mitad superior
		aux.resize(n_fft, complex<float>(0, 0));	// Las relleno con 0 para que el vector tenga longitud n_fft
		for_each(aux.begin(), aux.end(), [](complex<float> sample) { sample *= 2; });	// Duplico para "compensar" las muestras que puse en 0 y o tener perdida de potencia

		for(int sample = 0; sample < n_fft; sample++)
		{
#ifndef ANDROID_VOICE
			//Fuerzo a 0 la fase:
			aux[sample] = complex<float>(std::abs(aux[sample]), 0);
#endif // !ANDROID_VOICE
		}
		FFT_calculator::ifft(aux,aux);
#ifdef DEBUG_ROBOT
		cout << " Guardo en las posiciones: " << endl;
#endif // !DEBUG_ROBOT

		for(int sample_offset = 0; sample_offset < n_fft; sample_offset++)
		{
#ifdef DEBUG_ROBOT
		cout << first_window_sample + sample_offset << endl;
#endif //DEBUG_ROBOT
		out[first_window_sample + sample_offset] += aux[sample_offset] * hanning_window_factor(sample_offset, n_fft);
		}
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
//	return 1;
	return 0.5 - 0.5 * cos(2*M_PI * i/(float)n_fft);
}