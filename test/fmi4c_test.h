#ifndef FMIC_TEST_H
#define FMIC_TEST_H

#include <stdio.h>

#define VAR_MAX 1024

typedef struct {
    const char *name;
    double data[1024];
} namedData;

extern int logLevel;

extern FILE *outputFile;
extern int numOutputs;
extern unsigned int outputRefs[VAR_MAX];
extern const char* outputCsvPath;

extern size_t nInterpolators;
extern namedData interpolationData[20];
extern size_t dataSize;

double interpolate(namedData *timeData, namedData *data, double time, size_t nData);

#endif //FMIC_TEST.H
