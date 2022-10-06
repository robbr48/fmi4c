#ifndef FMIC_TEST_FMI2_H
#define FMIC_TEST_FMI2_H

#include "fmi4c_types_fmi2.h"
#include <stdbool.h>

void loggerFmi2(fmi2ComponentEnvironment componentEnvironment,
                fmi2String instanceName,
                fmi2Status status,
                fmi2String category,
                fmi2String message,
                ...);

int testFMI2(fmiHandle *fmu, bool forceModelExchange, bool forceCosimulation, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride);

#endif //FMIC_TEST_FMI2_H
