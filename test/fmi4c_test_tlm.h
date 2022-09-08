#ifndef FMIC_TEST_TLM_H
#define FMIC_TEST_TLM_H

#include "fmi4c.h"

int testFMI3TLM(fmiHandle *fmua, fmiHandle *fmub, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride);

#endif //FMIC_TEST_TLM_H
