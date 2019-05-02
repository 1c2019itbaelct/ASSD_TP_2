//
// Created by mlste on 4/29/2019.
//

#include "set_robot.h"
#include <vector>
#include <complex>
#include <algorithm>
#include "FFT_calculator.h"
#include "effect_aux_functions.h"

#define SAMPLE_RATE         (60000)
#define FRAMES_PER_BUFFER   (512)
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

void robotize_stream(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out);
void window_stream(const std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out);
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
            float left_real = in[2*i];
            float right_real = in[2*i+1];
            (*current_samples_left)[i].real(left_real);
            (*current_samples_left)[i].imag(0);
            (*current_samples_right)[i].real(right_real);
            (*current_samples_right)[i].imag(0);
        }


        //En la salida de un callback, estan mezcladas tres ventanas superpuestas 50%:
        //  A) Ventana de muestras mitad del callback anterior mitad de este
        //  B) Ventana de muestras del callback anterior
        //  C) Ventana de muestras de este callback

        // // // A) Ventana de muestras mitad del callback anterior mitad de este

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

        //2)Aplico ventana
        window_stream(*aux_left, *aux_left);
        window_stream(*aux_right, *aux_right);

        //3)Robotizo
        robotize_stream(*aux_left,  *aux_left);
        robotize_stream(*aux_right, *aux_right);

        //4)Sumo a la salida
        for (int i = 0; i < framesPerBuffer; ++i)
        {
            out[2 * i] = (*aux_left)[i].real();
            out[2 * i + 1] = (*aux_right)[i].real();
        }

        // // // B) Ventana de muestras del callback anterior

        // El efecto robot ya esta hecho en el callback anterior. Solamente sumo a la salida
        for (int i = 0; i < framesPerBuffer / 2; i++)
        {
            SAMPLE left_sample = (*past_output_left)[framesPerBuffer / 2 + i].real();
            SAMPLE right_sample = (*past_output_right)[framesPerBuffer / 2 + i].real();
            out[2 * i] +=  left_sample;
            out[2 * i + 1] += right_sample;
        }

        // // // C) Ventana de muestras de este callback

        //1)cargo las muestras (ya fue hecho en el anteriormente)

        //2)Aplico ventana
        window_stream(*current_samples_left,  *current_output_left);
        window_stream(*current_samples_right, *current_output_right);

        //3)Robotizo
        robotize_stream(*current_output_left,  *current_output_left);
        robotize_stream(*current_output_right, *current_output_right);

        //4)Sumo a la salida
        for (int i = framesPerBuffer / 2; i < framesPerBuffer; ++i)
        {
            SAMPLE left_sample = (*current_output_left)[i].real();
            SAMPLE right_sample = (*current_output_right)[i].real();
            out[2 * i] +=  left_sample;
            out[2 * i + 1] += right_sample;
        }

        // Guardo las samples de entrada y su output para el proximo callback:
        swap(UD->current_samples_left, UD->past_samples_left);
        swap(UD->current_samples_right, UD->past_samples_right);
        swap(UD->current_output_left, UD->past_output_left);
        swap(UD->current_output_right, UD->past_output_right);

    }
    return paContinue;
}

/**
 * @brief Aplica el efecto de robot a un stream con ventana hanning
 *
 * @param in Buffer de entrada. Para que no sea recortado, su tamanio debe ser potencia de 2.
 * @param out Buffer de salida. Importante: debe ser de igual o mayor tamanio que el buffer de entrada.
 */
void robotize_stream(std::vector<std::complex<float>>& in, std::vector<std::complex<float>>& out)
{

    unsigned int N = in.size();

    //1) FFteo
    FFT_calculator::fft(in, out);

    //2) Pongo la fase en 0
    for (int i = 0; i < N; i++)
    {
        (out)[i].real(abs((out)[i]));
        (out)[i].imag(0);
    }

    //3)iFFTeo
    FFT_calculator::ifft(out, out);
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



