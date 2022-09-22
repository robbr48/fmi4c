#ifndef FMIC_TEST_FMI3_H
#define FMIC_TEST_FMI3_H

#include "fmi4c_types_fmi3.h"

void loggerFmi3(fmi3InstanceEnvironment instanceEnvironment,
                fmi3Status status,
                fmi3String category,
                fmi3String message);

int testFMI3(fmiHandle *fmu, bool forceModelExchange, bool forceCosimulation, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride);

#endif //FMIC_TEST_FMI3_H
