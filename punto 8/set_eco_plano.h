#ifndef EJ8_SET_ECO_PLANO_H
#define EJ8_SET_ECO_PLANO_H

#include "portaudio.h"
extern int gNumNoInputs;
PaError set_eco_plano(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //EJ8_SET_ECO_PLANO_H
