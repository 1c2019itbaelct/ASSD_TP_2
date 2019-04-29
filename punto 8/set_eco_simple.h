#ifndef EJ8_SET_ECO_SIMPLE_H
#define EJ8_SET_ECO_SIMPLE_H

#include "portaudio.h"
static int gNumNoInputs = 0;
PaError set_eco_simple(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //EJ8_SET_ECO_SIMPLE_H
