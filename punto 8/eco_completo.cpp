#include <stdio.h>
#include <math.h>
#include <iostream>
#include "circular_buffer.h"
#include "eco_completo.h"
#define SAMPLE_RATE         (44100)
#define FRAMES_PER_BUFFER   (64)
typedef float SAMPLE;

using namespace std;

typedef  struct {
    circular_buffer<float>* aux1;
    circular_buffer<float>* aux2;
    circular_buffer<float>* aux3;
    circular_buffer<float>* aux4;
    circular_buffer<float>* buff_in;
    circular_buffer<float>* entrada1_AP;

    float a;
    float b;
    float D1;
    float D2;
    float D3;
    float D4;

    int count1;
    int count2;
    int count3;
    int count4;

} ecocompleto_user_data_t;

static int eco_completo_Callback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData )
{
    SAMPLE *out = (SAMPLE*)outputBuffer;
    const SAMPLE *in = (const SAMPLE*)inputBuffer;
    unsigned int i;

    ecocompleto_user_data_t * UD = ((ecocompleto_user_data_t *) userData);
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

        for( i=0; i<2*framesPerBuffer; i++ )
        {
            if(UD->count4>=1)
            {
                //entrada de allpass1 == salida de la sumatoria de reverb planos == (UD->aux1->front())+ (UD->aux2->front())+ (UD->aux3->front());
                UD->entrada1_AP->push_back((UD->aux1->front())+ (UD->aux2->front())+ (UD->aux3->front()));
                //salida total = salida del allpass1
                (*out++) =UD->aux4->front();
                UD->aux1->pop_front();
                UD->aux2->pop_front();
                UD->aux3->pop_front();
                UD->aux4->pop_front();
            }
            ///////////////////////////////////////////////////////////////////
            //reverberador plano 1
            if ((UD->count1) >= ((2 * (UD->D1)) / FRAMES_PER_BUFFER))
            {
                    UD->aux1->push_back(in[i] + ((UD->a) * (UD->aux1->front())));
            }
            else
            {
                UD->aux1->push_back(in[i]);
                (UD->count1)++;
            }
            ////////////////////////////////////////////////////////////////
            //reverberador plano 2
            if ((UD->count2) >= ((2 * (UD->D2)) / FRAMES_PER_BUFFER))
            {
                UD->aux2->push_back(in[i] + ((UD->a) * (UD->aux2->front())));
            }
            else
            {
                UD->aux2->push_back(in[i]);
                (UD->count2)++;
            }
            ///////////////////////////////////////////////////////////////
            //reverberador plano 3
            if ((UD->count3) >= ((2 * (UD->D3)) / FRAMES_PER_BUFFER))
            {
                UD->aux3->push_back(in[i] + ((UD->a) * (UD->aux3->front())));
            }
            else
            {
                UD->aux3->push_back(in[i]);
                (UD->count3)++;
            }
            ////////////////////////////////////////////////////////////////
            //allpass1
            if((UD->count4)>=((2*(UD->D4))/FRAMES_PER_BUFFER))
            {
                UD->aux4->push_back( ((UD->b) * (UD->aux4->front())) - ((UD->b) * UD->entrada1_AP->front()) + (UD->buff_in->front()) );
                UD->buff_in->pop_front();
                UD->buff_in->push_back(UD->entrada1_AP->front());
                UD->entrada1_AP->pop_front();

            }
            else
            {
                UD->aux4->push_back( UD->entrada1_AP->front() );
                UD->buff_in->push_back(UD->entrada1_AP->front());
                UD->entrada1_AP->pop_front();
                (UD->count4)++;
            }
        }
}




    return paContinue;
}



PaError set_eco_completo(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err)
{
    float a_=0.5;
    float b_=0.5;
    float D1_=5000; // combs en paralelo
    float D2_=5500;
    float D3_=5900;
    float D4_=6000; // allpass en serie

    int count1_=0;
    int count2_=0;
    int count3_=0;
    int count4_=0;


    circular_buffer<float> aux1_(2*D1_);
    circular_buffer<float> aux2_(2*D2_);
    circular_buffer<float> aux3_(2*D3_);
    circular_buffer<float> aux4_(2*D4_);
    circular_buffer<float> buff_in(2*D4_);
    circular_buffer<float> entrada1_AP(2*D4_);



    ecocompleto_user_data_t ecocompleto_user_data = { &aux1_,&aux2_,&aux3_, &aux4_, &buff_in, &entrada1_AP,a_, b_, D1_, D2_, D3_, D4_ ,count1_, count2_, count3_, count4_ };
    err = Pa_OpenStream(
            &stream,
            &inputParameters,
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
            eco_completo_Callback,
            (void *) &ecocompleto_user_data );
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


