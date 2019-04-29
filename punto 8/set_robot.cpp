//
// Created by mlste on 4/29/2019.
//

#include "set_robot.h"
#include <vector>
#include <complex>
#include "FFT_calculator.h"
#include <algorithm>

#define SAMPLE_RATE         (60100)
#define FRAMES_PER_BUFFER   (1024)
typedef float SAMPLE;

using namespace std;

typedef  struct {
    vector<complex<float>> * past_samples_left;
    vector<complex<float>> * past_samples_right;
    vector<complex<float>> * past_output_left;
    vector<complex<float>> * past_output_right;
    vector<complex<float>> * current_samples_left;
    vector<complex<float>> * current_samples_right;
    vector<complex<float>> * current_output_left;
    vector<complex<float>> * current_output_right;
    vector<complex<float>> * aux_left;
    vector<complex<float>> * aux_right;
} robot_user_data_t;

float hanning_window_factor( int i, int n_fft);

static int robot_Callback( const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData )
{
    // Para evitar castear constantemente:
    auto out = (SAMPLE *) outputBuffer;
    auto in = (const SAMPLE *) inputBuffer;
    auto UD = (robot_user_data_t *) userData;

    if( inputBuffer == NULL )
    {
        for(int i = 0; i < framesPerBuffer; i++ )
        {
            *out++ = 0;
            *out++ = 0;
        }
        gNumNoInputs += 1;
    }
    else
    {
        //Para evitar castear constantemente
        auto past_samples_left      = (vector<complex<float>> *) UD->past_samples_left;
        auto past_samples_right     = (vector<complex<float>> *) UD->past_samples_right;
        auto past_output_left       = (vector<complex<float>> *) UD->past_output_left;
        auto past_output_right      = (vector<complex<float>> *) UD->past_output_right;
        auto current_samples_left   = (vector<complex<float>> *) UD->current_samples_left;
        auto current_samples_right  = (vector<complex<float>> *) UD->current_samples_right;
        auto current_output_left    = (vector<complex<float>> *) UD->current_output_left;
        auto current_output_right   = (vector<complex<float>> *) UD->current_output_right;
        auto aux_left               = (vector<complex<float>> *) UD->aux_left;
        auto aux_right              = (vector<complex<float>> *) UD->aux_right;

        (void) timeInfo;
        (void) statusFlags;

        //Cargo las samples del buffer de entrada en los vectores correspondientes
        for (int i = 0; i < framesPerBuffer; i++)
        {
            (*current_samples_left)[i].real(in[2 * i]);
            (*current_samples_left)[i].imag(0);
            (*current_samples_right)[i].real(in[2 * i + 1]);
            (*current_samples_right)[i].imag(0);
        }

        //En la salida de un callback, estan mezcladas tres ventanas superpuestas 50%:
        //  A) Ventana de muestras mitad del callback anterior mitad de este
        //  B) Ventana de muestras del callback anterior
        //  C) Ventana de muestras de este callback

        {
            //  A) Ventana de muestras mitad del callback anterior mitad de este

            //1)cargo las muestras

            aux_left->clear();
            aux_right->clear();

            aux_left->insert(aux_left->begin(),
                             past_samples_left->begin() + framesPerBuffer / 2,
                             past_samples_left->end());
            aux_left->insert(aux_left->begin() + framesPerBuffer / 2,
                             current_samples_left->begin(),
                             current_samples_left->begin() + framesPerBuffer / 2);

            aux_right->insert(aux_right->begin(),
                              past_samples_right->begin() + framesPerBuffer / 2,
                              past_samples_right->end());
            aux_right->insert(aux_right->begin() + framesPerBuffer / 2,
                              current_samples_right->begin(),
                              current_samples_right->begin() + framesPerBuffer / 2);

            //2)aplico ventana
            for (int i = 0; i < framesPerBuffer; ++i) {
                (*aux_left)[i].real((*aux_left)[i].real() * hanning_window_factor(i, framesPerBuffer));
                (*aux_right)[i].real((*aux_right)[i].real() * hanning_window_factor(i, framesPerBuffer));
            }

            //3) FFteo
            FFT_calculator::fft(*aux_left, *aux_left);
            FFT_calculator::fft(*aux_right, *aux_right);

            //4) Pongo la fase en 0
            for (int i = 0; i < framesPerBuffer; i++) {
                (*aux_left)[i].real(abs((*aux_left)[i]));
                (*aux_left)[i].imag(0);
                (*aux_right)[i].real(abs((*aux_right)[i]));
                (*aux_right)[i].imag(0);
            }

            //5)iFFTeo
            FFT_calculator::ifft(*aux_left, *aux_left);
            FFT_calculator::ifft(*aux_right, *aux_right);

            //6)Sumo a la salida
            for (int i = 0; i < framesPerBuffer; ++i)
            {
                out[2 * i] = (*aux_left)[i].real();
                out[2 * i + 1] = (*aux_right)[i].real();
            }

        }

        //  B) Ventana de muestras del callback anterior

        for (int i = 0; i < framesPerBuffer / 2; ++i)
        {
            out[2 * i] += (*past_samples_left)[framesPerBuffer / 2 + i].real();
            out[2 * i + 1] += (*past_samples_right)[framesPerBuffer / 2 + i].real();
        }

        //  C) Ventana de muestras de este callback
        {
            //1)cargo las muestras (ya fue hecho en el anteriormente)
            //2)aplico ventana
            for (int i = 0; i < framesPerBuffer; ++i)
            {
                (*current_samples_left)[i].real((*current_samples_left)[i].real() * hanning_window_factor(i, framesPerBuffer));
                (*current_samples_right)[i].real((*current_samples_right)[i].real() * hanning_window_factor(i, framesPerBuffer));
            }

            //3) FFteo. En este caso no sobreescribo el vector de entrada como hice con la ventana
            // A) porque necesito preservar los valores hasta el proximo callback
            FFT_calculator::fft(*current_samples_left, *current_output_left);
            FFT_calculator::fft(*current_samples_right, *current_output_right);

            //4) Pongo la fase en 0
            for (int i = 0; i < framesPerBuffer; i++) {
                (*current_output_left)[i].real(abs((*current_output_left)[i]));
                (*current_output_left)[i].imag(0);
                (*current_output_right)[i].real(abs((*current_output_right)[i]));
                (*current_output_right)[i].imag(0);
            }

            //5)iFFTeo
            FFT_calculator::ifft(*current_output_left, *current_output_left);
            FFT_calculator::ifft(*current_output_right, *current_output_right);

            //6)Sumo a la salida
            for (int i = framesPerBuffer / 2; i < framesPerBuffer; ++i)
            {
                out[2 * i] += (*current_output_left)[i].real();
                out[2 * i + 1] += (*current_output_right)[i].real();
            }

        }

        // Guardo las samples de entrada y su output para el proximo callback:
        swap(current_samples_left, past_samples_left);
        swap(current_samples_right, past_samples_right);
        swap(current_output_left, past_output_left);
        swap(current_output_right, past_output_right);

    }

    return paContinue;

}

PaError set_robot(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err)
{
    FFT_calculator::fft_init(); // Calculo de constantes de la FFT

    vector<complex<float>>  past_samples_left(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  past_samples_right(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  past_output_left(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  past_output_right(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  current_samples_left(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  current_samples_right(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  current_output_left(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  current_output_right(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  aux_left(FRAMES_PER_BUFFER, complex<float>(0,0));
    vector<complex<float>>  aux_right(FRAMES_PER_BUFFER, complex<float>(0,0));



    robot_user_data_t robot_user_data = {
            &past_samples_left,
            &past_samples_right,
            &past_output_left,
            &past_output_right,
            &current_samples_left,
            &current_samples_right,
            &current_output_left,
            &current_output_right,
            &aux_left,
            &aux_right};


    err = Pa_OpenStream(
            &stream,
            &inputParameters,
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
            robot_Callback,
            (void *) &robot_user_data );
    if( err == paNoError )
    {
        err = Pa_StartStream( stream );
        if(err == paNoError)
        {
            printf("Hit ENTER to stop program.\n");
            getchar();
        }
    }
    return err;
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

