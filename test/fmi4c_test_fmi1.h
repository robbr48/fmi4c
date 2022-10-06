#ifndef FMIC_TEST_FMI1_H
#define FMIC_TEST_FMI1_H

#include "fmi4c_types_fmi1.h"
#include <stdbool.h>

void loggerFmi1(fmi1Component_t component,
                fmi1String instanceName,
                fmi1Status status,
                fmi1String category,
                fmi1String message,
                ...);

int testFMI1(fmiHandle *fmu, bool forceModelExchange, bool forceCosimulation, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride);

#endif //FMIC_TEST_FMI1_H
