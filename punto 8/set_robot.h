#ifndef ROBOT_SET_ROBOT_H
#define ROBOT_SET_ROBOT_H

#include "portaudio.h"
extern int gNumNoInputs;
PaError set_robot(PaStream*& stream, PaStreamParameters& inputParameters, PaStreamParameters& outputParameters, PaError& err);

#endif //ROBOT_SET_ROBOT_H
