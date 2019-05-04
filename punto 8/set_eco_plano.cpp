#include <stdio.h>
#include <math.h>
#include <iostream>
#include "circular_buffer.h"
#include "set_eco_plano.h"
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (64)
typedef float SAMPLE;

using namespace std;

typedef  struct {

    circular_buffer<float>* buff_;
    float a;
    float D;
    int count;
} ecoplano_user_data_t;

static int eco_plano_Callback( const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;

    ecoplano_user_data_t * UD = ((ecoplano_user_data_t *) userData);
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) userData;

    if( inputBuffer == NULL )
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = 0;  /* left - silent */
            *out++ = 0;  /* right - silent */
        }
        gNumNoInputs += 1;
    }
    else
    {
        if((UD->count)>=((2*(UD->D))/FRAMES_PER_BUFFER))
        {

            for( i=0; i<2*framesPerBuffer; i++ )
            {
                *out= in[i]+ ((UD->a) * (UD->buff_->front()));
                UD->buff_->pop_front();
                UD->buff_->push_back(*out);
                *out++;
            }



        }
        else
        {
            for( i=0; i<2*framesPerBuffer; i++ )
            {
                *out++= in[i];
                UD->buff_->push_back(in[i]);
            }
            (UD->count)++;
        }
    }
    return paContinue;
}



PaError set_eco_plano(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err)
{
    float a_=0.3;
    float D_=500;
    int count_=0;
    circular_buffer<float> buff_(2*D_);
    ecoplano_user_data_t ecoplano_user_data = { &buff_,a_, D_, count_};


    err = Pa_OpenStream(
            &stream,
            &inputParameters,
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
            eco_plano_Callback,
            (void *) &ecoplano_user_data );
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


