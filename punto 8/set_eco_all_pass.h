//
// Created by User on 3/5/2019.
//

#ifndef TEST_SET_ECO_ALL_PASS_H
#define TEST_SET_ECO_ALL_PASS_H

#include "portaudio.h"
extern int gNumNoInputs;
PaError set_eco_all_pass(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //TEST_SET_ECO_ALL_PASS_H
