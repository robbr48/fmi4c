#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "fmi4c.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi1.h"
#include "fmi4c_test_fmi2.h"
#include "fmi4c_test_fmi3.h"
#include "fmi4c_test_tlm.h"

int numOutputs = 0;
FILE* outputFile = NULL;
unsigned int outputRefs[VAR_MAX];

//Default experiment settings (may be overwritten by default experiment in FMU)
static double startTime = 0;
static double stepSize = 0.001;
static double stopTime = 1;
static double tolerance = 0;

const char* outputCsvPath = "\0";
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

int main(int argc, char *argv[])
{
    //Parse flags
    bool forceModelExchange = false;
    bool testTLM = false;
    int i=1;
    int nFlags = 0;
    const char* inputCsvPath = "\0";
    while(argv[i]) {
        if(!strcmp(argv[i],"--me")) {
            printf("Forcing model exchange mode.\n");
            forceModelExchange = true;
            ++nFlags;
        }
        else if(!strcmp(argv[i],"--tlm")) {
            printf("Running TLM test with intermediate update.\n");
            testTLM = true;
        }
        ++i;
    }
    if(argc < 3+nFlags) {
        printf("Usage: %s <flags> <fmu_file> <csv_output>\n", argv[0]);
        printf("Usage: %s <flags> <fmu_file> <csv_input> <csv_output>\n", argv[0]);
        exit(1);
    }

    const char* fmuPath;
    const char* fmuPath2;
    if(testTLM) {
        if(argc != 5) {
            printf("Wrong number of arguments for testing TLM.\n");
            printf("Usage: %s --tlm <fmu_file_1> <fmu_file_2> <csv_output>\n", argv[0]);
            exit(1);
        }
        fmuPath = argv[2];
        fmuPath2 = argv[3];
        outputCsvPath = argv[4];
    }
    else {
        fmuPath = argv[1+nFlags];
        if(argc > 3+nFlags) {
            inputCsvPath = argv[2+nFlags];
            outputCsvPath = argv[3+nFlags];
        }
        else {
            outputCsvPath = argv[2+nFlags];
        }
    }

    printf("--- Arguments ---\n");
    printf("  FMU path:        %s\n", fmuPath);
    if(testTLM) {
        printf("  FMU path (2): %s\n", fmuPath2);
    }
    if(inputCsvPath != "\0") {
        printf("  Input CSV path:  %s\n", inputCsvPath);
    }
    printf("  Output CSV path: %s\n", outputCsvPath);
    if(testTLM) {
        printf("  Running a TLM test.\n");
    }
    if(forceModelExchange) {
        printf("  Forcing model exchange mode.\n");
    }

    fmiHandle *fmu = loadFmu(fmuPath, "testfmu");

    if(fmu == NULL) {
        printf("Failed to load FMU\n");
        exit(1);
    }

    fmiVersion_t version = getFmiVersion(fmu);
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
        printf("Aborting due to unknown FMI verison.\n");
        exit(1);
    }

    char *cwd = (char*)malloc(sizeof(char)*FILENAME_MAX);
#ifdef _WIN32
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
#else
    getcwd(cwd, sizeof(char)*FILENAME_MAX);
#endif

    if(strcmp(inputCsvPath, "\0")) {
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
        fmiHandle *fmu2 = loadFmu(fmuPath2, "testfmu2");

        if(fmu2 == NULL) {
            printf("Failed to load second FMU\n");
            exit(1);
        }

        fmiVersion_t version2 = getFmiVersion(fmu2);
        if(version2 != fmiVersion3) {
            printf("Can only test TLM with FMI 3. Second FMU is of wrong version.\n");
            return 1;
        }

#ifdef FMI4CTEST_NO_TLM
        printf("FMI TLM support not available\n");
        return 1;
#else
        return testFMI3TLM(fmu, fmu2);
#endif
    }

    if(version == fmiVersion1) {
        return testFMI1(fmu);
    }
    else if(version == fmiVersion2) {
        return testFMI2(fmu, forceModelExchange);
    }
    else {
        return testFMI3(fmu, forceModelExchange);
    }

    return 0;
}


