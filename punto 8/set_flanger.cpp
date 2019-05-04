#include "circular_buffer.h"
#include "set_flanger.h"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "set_flanger.h"
#define SAMPLE_RATE        (44100)
#define FRAMES_PER_BUFFER   (64)
#define PI 3.14159265
typedef float SAMPLE;

using namespace std;

typedef  struct {

    circular_buffer<float>* buff_;
    int count;
    float range_;
    float delay_;
    float f;
    float fs;
    int n;
    float a;
} flanger_user_data_t;

static int flanger_Callback( const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;
    flanger_user_data_t * UD = ((flanger_user_data_t *) userData);
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) userData;
    float nmax=(UD->fs)/(UD->f);
    float sin_;
    int D;



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
        for (i = 0; i < 2 * framesPerBuffer; i++,(UD->n)++) {
            while((UD->n)>=nmax)
            {
                (UD->n)=(UD->n)-nmax;
            }
            if ((UD->count) >= ((UD->range_ + UD->delay_) / FRAMES_PER_BUFFER)) {
                UD->buff_->push_back(in[i]);
                sin_ = sin(2 * PI * (UD->n) * ((UD->f) / (UD->fs)));
                D = (UD->delay_) + ((UD->range_) * sin_);
                *out++ = in[i] + (UD->a)*(*(UD->buff_->end() - 2*( D + 1)));
                UD->buff_->pop_front();
            }
            else{
                  *out++ = in[i];
                  UD->buff_->push_back(in[i]);
            }
        }
        (UD->count)++;
    }
    return paContinue;
}



PaError set_flanger(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err)
{
    float s=5;
    float range_=500;
    float delay_=1000;
    float a=1;
    float f=1/s;
    float fs=SAMPLE_RATE;
    int count_=0;
    int n=0;
    circular_buffer<float> buff_(2*(range_+delay_));

    flanger_user_data_t flanger_user_data = {&buff_, count_, range_, delay_, f, fs,n, a};


    err = Pa_OpenStream(
            &stream,
            &inputParameters,
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
            flanger_Callback,
            (void *) &flanger_user_data );
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

