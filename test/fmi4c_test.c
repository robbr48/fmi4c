#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "fmi4c.h"
#include "fmi4c_common.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi1.h"
#include "fmi4c_test_fmi2.h"
#include "fmi4c_test_fmi3.h"
#include "fmi4c_test_tlm.h"

int numOutputs = 0;
FILE* outputFile = NULL;
unsigned int outputRefs[VAR_MAX];
int logLevel = 0;

//Default experiment settings (may be overwritten by default experiment in FMU)
static double startTime = 0;
static double stepSize = 0.001;
static double stopTime = 1;
static double tolerance = 0;

const char* outputCsvPath = NULL;
size_t nInterpolators = 0;
namedData interpolationData[20];
size_t dataSize = 0;

double interpolate(namedData *timeData, namedData *data, double time, size_t nData) {
    size_t i1 = 0;
    size_t i2 = 1;
    while(i1 < nData && !(timeData->data[i1] <= time && timeData->data[i2] > time)) {
        ++i1;
        ++i2;
    }

    double x1 = data->data[i1];
    double x2 = data->data[i2];
    double t1 = timeData->data[i1];
    double t2 = timeData->data[i2];

    return x1 + (x2-x1)*(time-t1)/(t2-t1);
}


void printUsage() {
    printf("Usage: fmi4ctest <options> <fmu_file(s)>\n");
    printf("Options:                 Meaning:\n");
    printf("-i, --input              Path to input CSV file\n");
    printf("-o, --output             Path to output CSV file\n");
    printf("-m, --mode               Simulation mode: \n"
           "                         auto: use co-simulation if possible, else model excghange (defualt)\n"
           "                         me: force model excghange mode\n"
           "                         cs: force co-simulation mode\n");
    printf("-h, --stepsize=TIMESTEP  Specify communication step size\n");
    printf("-s, --stoptime=STOPTIME  Specify simulation stop time\n");
    printf("-l, --loglevel=LOGLEVEL  Specify log level: \n"
           "                         0: no logging\n"
           "                         1: fatal"
           "                         2: fatal & errors\n"
           "                         3: fatal, errors & warnings\n"
           "                         4: fatal, errors, warning & info\n"
           "                         5: fatal, errors, warnings, info & debug.\n");
    printf("-t, --tlm                Run a TLM test (requires two FMUs)\n");
}

int main(int argc, char *argv[])
{
    if(argc == 1) {
        printUsage();
        exit(0);
    }

    //Parse flags
    bool forceModelExchange = false;
    bool forceCosimulation = false;
    bool testTLM = false;
    bool overrideStopTime = false;
    double stopTimeOverride=0;
    bool overrideTimeStep = false;
    double timeStepOverride=0;
    int i=1;
    int nFlags = 0;
    const char* inputCsvPath = "";
    while(argv[i]) {
        if(!strcmp(argv[i],"-i") || !strcmp(argv[i],"--input")) {
            inputCsvPath = argv[i+1];
            nFlags += 2;
        }
        else if(!strcmp(argv[i],"-o") || !strcmp(argv[i],"--output")) {
            outputCsvPath = argv[i+1];
            nFlags += 2;
        }
        else if(!strcmp(argv[i],"-m") || !strcmp(argv[i],"--mode")) {
            if(!strcmp(argv[i+1], "me")) {
                forceModelExchange = true;
            }
            else if(!strcmp(argv[i+1], "cs")) {
                forceCosimulation = true;
            }
            else if(strcmp(argv[i+1], "auto")) {   
                printf("Error: Unknown mode: %s\n",argv[i+1]);
                printUsage();
                exit(1);
            }
            nFlags += 2;    
        }
        else if(!strcmp(argv[i],"-t") || !strcmp(argv[i],"--tlm")) {
            testTLM = true;
            ++nFlags;
        }
        else if(!strcmp(argv[i],"-s") || !strcmp(argv[i], "--stoptime")) {
            ++i;
            if(argc<=i || argv[i][0] == '-')   {
                printf("Error: Stop time flag requires a value.");
                printUsage();
                exit(1);
            }
            if((sscanf(argv[i], "%lf", &stopTimeOverride) != 1) || (stopTimeOverride <= 0)) {
                printf("Error: Stop time must be a positive number.");
                printUsage();
                exit(1);
            }
            overrideStopTime = true;
            nFlags += 2;
        }
        else if(!strcmp(argv[i],"-h") || !strcmp(argv[i], "--stepsize")) {
            ++i;
            if(argc<=i || argv[i][0] == '-')   {
                printf("Error: Time step flag requires a value.");
                printUsage();
                exit(1);
            }
            if((sscanf(argv[i], "%lf", &timeStepOverride) != 1) || (timeStepOverride <= 0)) {
                printf("Error: Time step must be a positive number.");
                printUsage();
                exit(1);
            }
            overrideTimeStep = true;
            nFlags += 2;
        }
        else if(!strcmp(argv[i],"-l") || !strcmp(argv[i], "--loglevel")) {
            ++i;
            if(argc<=i || argv[i][0] == '-')   {
                printf("Error: Time step flag requires a value.");
                printUsage();
                exit(1);
            }
            if((sscanf(argv[i], "%i", &logLevel) != 1) || (logLevel < 0) || (logLevel > 5)) {
                printf("Error: Log level must be an integer betweeon 0 and 5.");
                printUsage();
                exit(1);
            }
            nFlags+=2;
        }
        ++i;
    }
    if(argc < 2+nFlags) {
        printUsage();
        exit(1);
    }

    const char* fmuPath = NULL;
    const char* fmuPath2 = NULL;
    if(testTLM) {
        if(argc != nFlags+3) {
            printf("Wrong number of arguments for testing TLM.\n");
            printf("Usage: %s --tlm <fmu_file_1> <fmu_file_2> <csv_output>\n", argv[0]);
            exit(1);
        }
        fmuPath = argv[1+nFlags];
        fmuPath2 = argv[2+nFlags];
    }
    else {
        fmuPath = argv[1+nFlags];
    }

    printf("--- Arguments ---\n");
    if(testTLM) {
        printf("  First FMU to test: %s\n", fmuPath);
        printf("  Second FMU to test: %s\n", fmuPath2);
    }
    else {
        printf("  FMU to test: %s\n", fmuPath);
    }
    if(inputCsvPath != NULL && strcmp(inputCsvPath, "") != 0) {
        printf("  Will read from input file: %s\n", inputCsvPath);
    }
    if(outputCsvPath != NULL && strcmp(outputCsvPath, "") != 0) {
        printf("  Will write to output file: %s\n", outputCsvPath);
    }
    if(forceModelExchange) {
        printf("  Will use model exchange mode\n");
    }
    if(forceCosimulation) {
        printf("  Will use co-simulation mode\n");
    }
    if(testTLM) {
        printf("  Will run a TLM test with intermediate update\n");
    }
    if(overrideStopTime) {
        printf("  Will use stop time: %f\n", stopTimeOverride);
    }
    if(overrideTimeStep) {
        printf("  Will use time step: %f\n", timeStepOverride);
    }
    if(logLevel == 0) {
        printf("  Using log level 0 (no logging)\n");
    }
    else if(logLevel == 1) {
        printf("  Using log level 1 (fatal)\n");
    }
    else if(logLevel == 2) {
        printf("  Using log level 2 (fatal & errors)\n");
    }
    else if(logLevel == 3) {
        printf("  Using log level 3 (fatal, errors & warnings)\n");
    }
    else if(logLevel == 4) {
        printf("  Using log level 4 (fatal, errors, warning & info)\n");
    }
    else if(logLevel == 5) {
        printf("  Using log level 5 (fatal, errors, warnings, info & debug)\n");
    }

    fmiHandle *fmu = fmi4c_loadFmu(fmuPath, "testfmu");

    if(fmu == NULL) {
        printf("Failed to load FMU\n");
        exit(1);
    }

    fmiVersion_t version = fmi4c_getFmiVersion(fmu);
    printf("--- FMU data ---\n  FMI Version:        ");
    if(version == fmiVersion1) {
        printf("1\n");
    }
    else if(version == fmiVersion2) {
        printf("2\n");
    }
    else if(version == fmiVersion3) {
        printf("3\n");
    }
    else {
        printf("Unknown\n");
        printf("Error: Unknown FMI version. Aborting.\n");
        exit(1);
    }

    if(inputCsvPath != NULL && strcmp(inputCsvPath, "") != 0) {
        FILE *inputFile = fopen(inputCsvPath, "r");
        char lineStr[1024];
        size_t lineNumber = 0;
        while(fgets(lineStr , sizeof(lineStr) , inputFile) != NULL) {
            lineStr[strcspn(lineStr, "\r\n")] = 0;
            char* word = strtok(lineStr, ",");
            int j=0;
            while(word != NULL && strcmp(word, "")) {
                if(lineNumber == 0) {
                    namedData data;
                    data.name = _strdup(word);
                    interpolationData[nInterpolators] = data;
                    ++nInterpolators;
                }
                else {
                    interpolationData[j].data[lineNumber-1] = atof(word);
                }
                word = strtok(NULL, ",");

                ++j;
            }
            lineNumber++;
        }
        fclose(inputFile);
        dataSize = lineNumber-1;
    }

    if(testTLM) {
        if(version != fmiVersion3) {
            printf("Can only test TLM with FMI 3. First FMU is of wrong version.\n");
            return 1;
        }

        //Load second FMU
        fmiHandle *fmu2 = fmi4c_loadFmu(fmuPath2, "testfmu2");

        if(fmu2 == NULL) {
            printf("Failed to load second FMU\n");
            exit(1);
        }

        fmiVersion_t version2 = fmi4c_getFmiVersion(fmu2);
        if(version2 != fmiVersion3) {
            printf("Can only test TLM with FMI 3. Second FMU is of wrong version.\n");
            return 1;
        }

#ifdef FMI4CTEST_NO_TLM
        printf("FMI TLM support not available\n");
        return 1;
#else
        int retval = testFMI3TLM(fmu, fmu2, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
        fmi4c_freeFmu(fmu);
        fmi4c_freeFmu(fmu2);
        return retval;
#endif
    }

    int retval;
    if(version == fmiVersion1) {
        retval = testFMI1(fmu, forceModelExchange, forceCosimulation, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else if(version == fmiVersion2) {
        retval = testFMI2(fmu, forceModelExchange, forceCosimulation, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else {
        retval = testFMI3(fmu, forceModelExchange, forceCosimulation, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }

    fmi4c_freeFmu(fmu);

    return retval;
}


