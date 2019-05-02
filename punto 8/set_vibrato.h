#ifndef TEST_SET_VIBRATO_H
#define TEST_SET_VIBRATO_H

#include "portaudio.h"
extern int gNumNoInputs;
PaError set_vibrato(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //TEST_SET_VIBRATO_H
