#ifndef FMIC_TEST_FMI1_H
#define FMIC_TEST_FMI1_H

#include "fmic_types_fmi1.h"

void loggerFmi1(fmi1Component component,
                fmi1String instanceName,
                fmi1Status status,
                fmi1String category,
                fmi1String message,
                ...);

int testFMI1(fmiHandle *fmu);

#endif //FMIC_TEST_FMI1_H
