#include "FFT_calculator.h"
#define _USE_MATH_DEFINES
#include "math.h"	//M_PI no anda si pongo cmath

#include <iostream>
#include <bitset>

using namespace std;

#define DEBUG_FFT

FFT_calculator::FFT_calculator()
{
	// Llenar todos los W_N. Hay un valor extra
	for (int i = 0; i < N_FFT_MAX/2 + 1; i++)
	{
		twiddleFactors[i] = complex<float>(cos(2*M_PI / N_FFT_MAX * i), -sin((2*M_PI / N_FFT_MAX * i)));
	}
}


FFT_calculator::~FFT_calculator()
{
	//Destruir los W
}

void FFT_calculator::fft(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out)
{
	out = in;
	if( !in.size() || in.size() == 1) { return; } // Si no hay minimo dos muestras, no hay nada mas que hacer.

	int logn = (int)log2(in.size()); // Se pasa por el logaritmo y se trunca para asegurar que se trabaja con una potencia de 2
	if(logn > LOG_N_FFT_MAX) { logn = LOG_N_FFT_MAX; } // Si hay mas muestras, truncar al maximo admitido.

	// butterflyWingSpan_0: distancia inicial entre wings (nodos) de una misma butterfly ( = N/2 = (2^logn)/2 = 2^(logn-1) )
	int butterflyWingSpan_0 = 1 << (logn - 1);
	
	// twiddleFactorIndexSep: separacion entre los indices de los twiddle factors correspondientes
	// a butterflies calculadas consecutivamente. Se duplica al aumentar la etapa.
	// Si N = N_FFT_MAX, es 1. Si se divide por dos, la separacion se duplica. => sep = N_FFT_MAX / N => sep = 2^(LOG_N_FFT_MAX - log(N))
	int twiddleFactorIndexSep = 1 << (LOG_N_FFT_MAX - logn);
	
	// twiddleFactorIndexMax: maximo indice de twiddle factors. No se modifica al aumentar la etapa.
	int twiddleFactorIndexMax = (butterflyWingSpan_0 - 1) * twiddleFactorIndexSep;
	
	//twiddleFactorIndex: indice del twiddle factor.
	int twiddleFactorIndex = 0;
	
	// Identifico las etapas por la distancia entre los nodos de la misma butterfly (butterflyWingSpan). 
	// Mientras avanzo de etapa, butterflyWingSpan se divide por dos.
	// En la ultima etapa, esta distancia es 1
	// Si hay 1 o 0 muestras, butterflyWingSpan inicia en 0 => se saltea el loop siguiente
	for (int butterflyWingSpan = butterflyWingSpan_0; 
		butterflyWingSpan; 
		butterflyWingSpan >>= 1, twiddleFactorIndexMax-=twiddleFactorIndexSep, twiddleFactorIndexSep <<= 1, twiddleFactorIndex = 0)
	{
		// Cada butterfly tiene dos wings: la top (butterflyTopWing), que tiene indice menor dentro 
		// del arreglo, y la bottom (butterflyBottomWing), que tiene indice mayor dentro del arreglo.
		// butterflyBottomWing = butterflyTopWing + butterflyWingSpan
		// Si se recorren todas las wings superiores en orden, la ultima esta en la posicion
		// 2*butterflyWingSpan_0 - butterflyWingSpan - 1

		// Busco la top wing de cada butterfly. Con eso mas el span obtengo la bottom wing.
		// Una vez que tengo las dos wings hago los calculos
		for (int butterflyTopWing = 0;  
			butterflyTopWing < (butterflyWingSpan_0 << 1) - butterflyWingSpan; 
			butterflyTopWing = (++butterflyTopWing) + (butterflyTopWing & butterflyWingSpan),
			twiddleFactorIndex = (twiddleFactorIndex + twiddleFactorIndexSep) & twiddleFactorIndexMax) //TODO: explicar
		{
#ifdef DEBUG_FFT
			cout << endl << "Nueva mariposa:" << endl;
			cout << "top: " << bitset<8>(butterflyTopWing) << "\t" << butterflyTopWing << endl;
			cout << "bot: " << bitset<8>(butterflyTopWing + butterflyWingSpan) << "\t" << butterflyTopWing + butterflyWingSpan << endl;
#endif
			complex<float> topWingContent = out[butterflyTopWing];	//hago backup para no pisar
			out[butterflyTopWing] = topWingContent + out[butterflyTopWing + butterflyWingSpan];
			out[butterflyTopWing + butterflyWingSpan] = twiddleFactors[twiddleFactorIndex] * (topWingContent - out[butterflyTopWing + butterflyWingSpan]);


#ifdef DEBUG_FFT
			cout << "Tfi: " << bitset<8>(twiddleFactorIndex) << "\t" << twiddleFactorIndex << endl << endl;
			cout << "Twiddle Factor:   " << twiddleFactors[twiddleFactorIndex];
			cout << " = " << abs(twiddleFactors[twiddleFactorIndex]) << " < " << arg(twiddleFactors[twiddleFactorIndex]) / M_PI << "*PI" << endl;
			cout << "Top Wing Output:  " << out[butterflyTopWing] << endl;
			cout << "Bot Wing Output:  " << out[butterflyTopWing+butterflyWingSpan] << endl;
#endif
		}
	}
	// Bit reverse de toda la secuencia
}


void FFT_calculator::ifft(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out)
{
	out = in;
	if (!in.size() || in.size() == 1) { return; } // Si no hay minimo dos muestras, no hay nada mas que hacer.

	int logn = (int)log2(in.size()); // Se pasa por el logaritmo y se trunca para asegurar que se trabaja con una potencia de 2
	if (logn > LOG_N_FFT_MAX) { logn = LOG_N_FFT_MAX; } // Si hay mas muestras, truncar al maximo admitido.

														// butterflyWingSpan_0: distancia inicial entre wings (nodos) de una misma butterfly ( = N/2 = (2^logn)/2 = 2^(logn-1) )
	int butterflyWingSpan_0 = 1 << (logn - 1);

	// twiddleFactorIndexSep: separacion entre los indices de los twiddle factors correspondientes
	// a butterflies calculadas consecutivamente. Se duplica al aumentar la etapa.
	// Si N = N_FFT_MAX, es 1. Si se divide por dos, la separacion se duplica. => sep = N_FFT_MAX / N => sep = 2^(LOG_N_FFT_MAX - log(N))
	int twiddleFactorIndexSep = 1 << (LOG_N_FFT_MAX - logn);

	// twiddleFactorIndexMax: maximo indice de twiddle factors. No se modifica al aumentar la etapa.
	int twiddleFactorIndexMax = (butterflyWingSpan_0 - 1) * twiddleFactorIndexSep;

	//twiddleFactorIndex: indice del twiddle factor.
	int twiddleFactorIndex = 0;

	// Identifico las etapas por la distancia entre los nodos de la misma butterfly (butterflyWingSpan). 
	// Mientras avanzo de etapa, butterflyWingSpan se divide por dos.
	// En la ultima etapa, esta distancia es 1
	// Si hay 1 o 0 muestras, butterflyWingSpan inicia en 0 => se saltea el loop siguiente
	for (int butterflyWingSpan = butterflyWingSpan_0;
		butterflyWingSpan;
		butterflyWingSpan >>= 1, twiddleFactorIndexMax -= twiddleFactorIndexSep, twiddleFactorIndexSep <<= 1, twiddleFactorIndex = 0)
	{
		// Cada butterfly tiene dos wings: la top (butterflyTopWing), que tiene indice menor dentro 
		// del arreglo, y la bottom (butterflyBottomWing), que tiene indice mayor dentro del arreglo.
		// butterflyBottomWing = butterflyTopWing + butterflyWingSpan
		// Si se recorren todas las wings superiores en orden, la ultima esta en la posicion
		// 2*butterflyWingSpan_0 - butterflyWingSpan - 1

		// Busco la top wing de cada butterfly. Con eso mas el span obtengo la bottom wing.
		// Una vez que tengo las dos wings hago los calculos
		for (int butterflyTopWing = 0;
			butterflyTopWing < (butterflyWingSpan_0 << 1) - butterflyWingSpan;
			butterflyTopWing = (++butterflyTopWing) + (butterflyTopWing & butterflyWingSpan),
			twiddleFactorIndex = (twiddleFactorIndex + twiddleFactorIndexSep) & twiddleFactorIndexMax) //TODO: explicar
		{
#ifdef DEBUG_FFT
			cout << endl << "Nueva mariposa:" << endl;
			cout << "top: " << bitset<8>(butterflyTopWing) << "\t" << butterflyTopWing << endl;
			cout << "bot: " << bitset<8>(butterflyTopWing + butterflyWingSpan) << "\t" << butterflyTopWing + butterflyWingSpan << endl;
#endif
			complex<float> topWingContent = out[butterflyTopWing];	//hago backup para no pisar
			out[butterflyTopWing] = topWingContent + out[butterflyTopWing + butterflyWingSpan];
			unsigned int index = (twiddleFactorIndexMax - twiddleFactorIndex + twiddleFactorIndexSep);
			out[butterflyTopWing + butterflyWingSpan] = -twiddleFactors[index] * (topWingContent - out[butterflyTopWing + butterflyWingSpan]);


#ifdef DEBUG_FFT
			cout << "Tfi: " << bitset<8>(index) << "\t" << index << endl << endl;
			cout << "Twiddle Factor:   " << -twiddleFactors[index];
			cout << " = " << abs(-twiddleFactors[index]) << " < " << arg(-twiddleFactors[index]) / M_PI << "*PI" << endl;

			cout << "Top Wing Output:  " << out[butterflyTopWing] << endl;
			cout << "Bot Wing Output:  " << out[butterflyTopWing + butterflyWingSpan] << endl;
#endif
		}
	}


	for (int i = 2 * butterflyWingSpan_0 - 1; i >= 0; i--)
	{
		out[i] /= (2 * butterflyWingSpan_0);
	}
	// Bit reverse de toda la secuencia



	/*
	out = in;

	int i = 1;	//TODO: borrar, DEBUG.

	int logn = (int)log2(N_FFT); //log en base 2 de la cantidad de muestras TODO: obtener apartir del input

	// butterflyWingSpan_0: distancia inicial entre wings (nodos) de una misma butterfly
	int butterflyWingSpan_0 = (logn ? (1 << (logn - 1)) : 0);	//Si hay una sola muestra, poner en 0

	// twiddleFactorIndexSep: separacion entre los indices de los twiddle factors correspondientes
	// a butterflies calculadas consecutivamente. Se duplica al aumentar la etapa.
	int twiddleFactorIndexSep = 1;

	// twiddleFactorIndexMax: maximo indice de twiddle factors. No se modifica al aumentar la etapa.
	int twiddleFactorIndexMax = (butterflyWingSpan_0 ? butterflyWingSpan_0 - 1 : 0);	//Si hay una sola muestra, poner en 0

	//twiddleFactorIndex: indice del twiddle factor.
	int twiddleFactorIndex = 0;

	// Identifico las etapas por la distancia entre los nodos de la misma butterfly (butterflyWingSpan). 
	// Mientras avanzo de etapa, butterflyWingSpan se divide por dos y kaleidoscopeCount se duplica.
	// En la ultima etapa, esta distancia es 1
	for (int butterflyWingSpan = butterflyWingSpan_0;
		butterflyWingSpan;
		butterflyWingSpan >>= 1, twiddleFactorIndexSep <<= 1, twiddleFactorIndex = 0)
	{
		cout << "***************" << endl;
		cout << "Etapa " << i << ":" << endl;
		cout << "wing span = " << butterflyWingSpan << endl << endl;

		// Cada butterfly tiene dos wings: la top (butterflyTopWing), que tiene indice menor dentro 
		// del arreglo, y la bottom (butterflyBottomWing), que tiene indice mayor dentro del arreglo.
		// butterflyBottomWing = butterflyTopWing + butterflyWingSpan
		// Si se recorren todas las wings superiores en orden, la ultima esta en la posicion
		// 2*butterflyWingSpan_0 - butterflyWingSpan - 1

		// Busco la top wing de cada butterfly. Con eso mas el span obtengo la bottom wing.
		// Una vez que tengo las dos wings hago los calculos
		for (int butterflyTopWing = 0;
			butterflyTopWing < (butterflyWingSpan_0 << 1) - butterflyWingSpan;
			butterflyTopWing = (++butterflyTopWing) + (butterflyTopWing & butterflyWingSpan),
			twiddleFactorIndex = (twiddleFactorIndex + twiddleFactorIndexSep) & twiddleFactorIndexMax) //TODO: explicar
		{
			cout << endl << "Nueva mariposa:" << endl;
			cout << "top: " << bitset<8>(butterflyTopWing) << "\t" << butterflyTopWing << endl;
			cout << "bot: " << bitset<8>(butterflyTopWing + butterflyWingSpan) << "\t" << butterflyTopWing + butterflyWingSpan << endl;
			cout << "Tfi: " << bitset<8>(twiddleFactorIndex) << "\t" << twiddleFactorIndex << endl << endl;

			cout << "Top Wing Input: " << out[butterflyTopWing] << endl;
			cout << "Bot Wing Input: " << out[butterflyTopWing + butterflyWingSpan] << endl;
			cout << "Twiddle Factor:   " << -twiddleFactors[twiddleFactorIndexMax - twiddleFactorIndex + 1];
			cout << " = " << abs(-twiddleFactors[twiddleFactorIndexMax - twiddleFactorIndex + 1]) << " < " << arg(-twiddleFactors[twiddleFactorIndexMax - twiddleFactorIndex + 1]) / M_PI << "*PI" << endl;
			
			complex<float> topWingContent = out[butterflyTopWing];	//hago backup para no pisar
			out[butterflyTopWing] = topWingContent + out[butterflyTopWing + butterflyWingSpan];
			out[butterflyTopWing + butterflyWingSpan] = -twiddleFactors[twiddleFactorIndexMax - twiddleFactorIndex + 1] * (topWingContent - out[butterflyTopWing + butterflyWingSpan]);

			cout << "Top Wing Output:  " << out[butterflyTopWing] << endl;
			cout << "Bot Wing Output:  " << out[butterflyTopWing + butterflyWingSpan] << endl;

		}
		i++; //TODO: borrar, DEBUG. Contador de etapas para hacer cout.
	}
	bitReverse(out);
	*/
}

void FFT_calculator::bitReverse(std::vector<std::complex<float>>& out)
{
	/*
	// Tabla y algoritmo de bitReverse obtenidos de:
	// http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
	static const unsigned char BitReverseTable256[] = 
	{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
	};

	int bitReverseIndex = 0;
	int index = 0;
	for ( ; index < N_FFT / 2; index++ )
	{
		// Obtener nuevo indice haciendo bit reverse del indice actual:
		bitReverseIndex = 	(BitReverseTable256[index & 0xff] << 24) | 
							(BitReverseTable256[(index >> 8) & 0xff] << 16) | 
							(BitReverseTable256[(index >> 16) & 0xff] << 8) |
							(BitReverseTable256[(index >> 24) & 0xff]);

		complex<float> indexContentBackup = out[index];
		out[index] = out[bitReverseIndex];
		out[bitReverseIndex] = indexContentBackup;
	}
	*/
}