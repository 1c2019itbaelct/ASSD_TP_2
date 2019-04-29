#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include "set_eco_simple.h"
using namespace std;
#define PA_SAMPLE_TYPE      paFloat32
enum {ECO_SIMPLE,ECO_PLANO,ECO_LP,ECO_COMPLETO,ECO_CONV,FLANGER,ROBOT,GIRO_3D,VIBRATO}effect_type;

int main(void);
int main(void)
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto error;
    }
    inputParameters.channelCount = 2;       /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    switch(effect_type)
    {
        case ECO_SIMPLE:
            err = set_eco_simple(stream,inputParameters, outputParameters,err);
            break;
        case ECO_PLANO:
            break;
        case ECO_COMPLETO:
            break;
        case ECO_LP:
            break;
        case ECO_CONV:
            break;
        case FLANGER:
            break;
        case ROBOT:
            break;
        case GIRO_3D:
            break;
        case VIBRATO:
            break;
        default:
            break;

    }



    err = Pa_CloseStream( stream );
    if( err == paNoError )
    {
        printf("Finished. gNumNoInputs = %d\n", gNumNoInputs );
        Pa_Terminate();
        return 0;
    }
    else
    {
        error:
        Pa_Terminate();
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        return -1;
    }

}