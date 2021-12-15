#ifndef FMIC_TEST_FMI3_H
#define FMIC_TEST_FMI3_H

#include "fmic_types_fmi3.h"

void loggerFmi3(fmi3InstanceEnvironment instanceEnvironment,
                 fmi3String instanceName,
                 fmi3Status status,
                 fmi3String category,
                 fmi3String message);

int testFMI3(fmiHandle *fmu, bool forceModelExchange);

#endif //FMIC_TEST_FMI3_H
