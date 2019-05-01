#ifndef TEST_SET_FLANGER_H
#define TEST_SET_FLANGER_H

#include "portaudio.h"
extern int gNumNoInputs;
PaError set_flanger(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //TEST_SET_FLANGER_H
