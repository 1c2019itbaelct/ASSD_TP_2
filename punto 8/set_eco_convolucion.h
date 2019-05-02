//
// Created by mlste on 5/1/2019.
//

#ifndef ROBOT_SET_ECO_CONVOLUCION_H
#define ROBOT_SET_ECO_CONVOLUCION_H

#include "portaudio.h"
extern int gNumNoInputs;
PaError set_eco_convolucion(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //ROBOT_SET_ECO_CONVOLUCION_H
