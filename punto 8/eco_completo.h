//
// Created by User on 3/5/2019.
//

#ifndef TEST_ECO_COMPLETO_H
#define TEST_ECO_COMPLETO_H


#include "portaudio.h"
extern int gNumNoInputs;
PaError set_eco_completo(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);


#endif //TEST_ECO_COMPLETO_H
