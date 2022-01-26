#include <stdarg.h>

#include "fmi4c.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi1.h"

void loggerFmi1(fmi1Component component,
                fmi1String instanceName,
                fmi1Status status,
                fmi1String category,
                fmi1String message,
                ...)
{
    UNUSED(component)
            UNUSED(instanceName)
            UNUSED(status)
            UNUSED(category)

            va_list args;
    va_start(args, message);
    printf(message, args);
    va_end(args);
}

int testFMI1ME(fmi1Handle *fmu1) {
    //Instantiate FMU
    if(!fmi1InstantiateModel(fmu1, loggerFmi1, calloc, free, NULL, fmi1True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi1DefaultStartTimeDefined(fmu1)) {
        startTime = fmi1GetDefaultStartTime(fmu1);
    }
    if(fmi1DefaultStopTimeDefined(fmu1)) {
        stopTime = fmi1GetDefaultStopTime(fmu1);
    }

    fmi1SetTime(fmu1, startTime);


    fmi1Status status;
    fmi1EventInfo eventInfo;
    size_t nStates;
    fmi1Real* states;
    fmi1Real* derivatives;
    size_t nEventIndicators;
    fmi1Real* eventIndicators;
    fmi1Real *eventIndicatorsPrev;
    eventInfo.terminateSimulation = fmi2False;
    eventInfo.nextEventTime = 0.0;
    double actualStepSize = stepSize;
    fmi1Boolean callEventUpdate = fmi2False;
    fmi1Boolean intermediateResults = fmi1False;

    fmi1Initialize(fmu1, fmi1False, 0, &eventInfo);
    printf("FMU successfully initialized!\n");

    nStates = fmi1GetNumberOfContinuousStates(fmu1);
    nEventIndicators = fmi1GetNumberOfEventIndicators(fmu1);

    states = malloc(nStates*sizeof(double));
    derivatives = malloc(nStates*sizeof(double));
    eventIndicators = malloc(nEventIndicators*sizeof(double));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(double));

    status = fmi1GetContinuousStates(fmu1, states, nStates);
    status = fmi1GetEventIndicators(fmu1, eventIndicators, nEventIndicators);

    FILE *outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi1GetVariableName(fmi1GetVariableByValueReference(fmu1, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    double time = startTime;
    for(; time < stopTime; time+=actualStepSize) {
        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi1VariableHandle *var = fmi1GetVariableByName(fmu1, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi1Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi1ValueReference vr = fmi1GetVariableValueReference(var);
            fmi1SetReal(fmu1, &vr, 1, &value);
        }

        size_t k;
        int zeroCrossingEvent = 0;

        status = fmi1SetTime(fmu1, time);

        status = fmi1GetEventIndicators(fmu1, eventIndicators, nEventIndicators);
        for (k = 0; k < nEventIndicators; k++) {
            if ((eventIndicators[k] > 0) != (eventIndicatorsPrev[k] > 0)) {
                zeroCrossingEvent = 1;
                break;
            }
        }

        //Handle events
        if (callEventUpdate || zeroCrossingEvent ||
                (eventInfo.upcomingTimeEvent && time == eventInfo.nextEventTime)) {
            status = fmi1EventUpdate(fmu1, intermediateResults, &eventInfo);
            status = fmi1GetContinuousStates(fmu1, states, nStates);
            status = fmi1GetEventIndicators(fmu1, eventIndicators, nEventIndicators);
            status = fmi1GetEventIndicators(fmu1, eventIndicatorsPrev, nEventIndicators);
        }

        //Update actual time stpe
        if (eventInfo.upcomingTimeEvent) {
            if(time + stepSize < eventInfo.nextEventTime) {
                actualStepSize = stepSize;
            }
            else {
                actualStepSize = eventInfo.nextEventTime - time;
            }
        }
        else {
            actualStepSize = stepSize;
        }
        time += actualStepSize;
        if(time > stopTime) {
            time -= actualStepSize;
            actualStepSize = stopTime - time;
            time = stopTime;
        }

        //Perform integration
        status = fmi1GetDerivatives(fmu1, derivatives, nStates);
        for (k = 0; k < nStates; k++) {
            states[k] = states[k] + actualStepSize*derivatives[k];
        }

        status = fmi1SetContinuousStates(fmu1, states, nStates);
        status = fmi1CompletedIntegratorStep(fmu1, &callEventUpdate);

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi1GetReal(fmu1, &outputRefs[i], 1, &value);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);

    fmi1Terminate(fmu1);
    fmi1FreeModelInstance(fmu1);

    return 0;
}


int testFMI1CS(fmi1Handle *fmu1)
{
    //Instantiate FMU
    if(!fmi1InstantiateSlave(fmu1, "application/x-fmu-sharedlibrary", 1000, fmi1False, fmi1False, loggerFmi1, calloc, free, NULL, fmi1True)) {
        printf("fmi1InstantiateSlave() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi1DefaultStartTimeDefined(fmu1)) {
        startTime = fmi1GetDefaultStartTime(fmu1);
    }
    if(fmi1DefaultStopTimeDefined(fmu1)) {
        stopTime = fmi1GetDefaultStopTime(fmu1);
    }

    fmi1Status status = fmi1InitializeSlave(fmu1,startTime,fmi1False,0);
    if(status != fmi1OK) {

        printf("Initialization failed!\n");
        exit(1);
    }
    printf("FMU successfully initialized!\n");

    printf("Simulating from %f to %f...\n",startTime, stopTime);
    FILE *outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi1GetVariableName(fmi1GetVariableByValueReference(fmu1, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    for(double time=startTime; time <= stopTime; time+=stepSize) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi1VariableHandle *var = fmi1GetVariableByName(fmu1, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi1Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi1ValueReference vr = fmi1GetVariableValueReference(var);
            fmi1SetReal(fmu1, &vr, 1, &value);
        }

        //Take a step
        status = fmi1DoStep(fmu1, time, stepSize, fmi1True);
        if(status != fmi1OK) {
            printf("fmi1DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi1GetReal(fmu1, &outputRefs[i], 1, &value);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);

    fmi1TerminateSlave(fmu1);
    fmi1FreeSlaveInstance(fmu1);

    return 0;
}


int testFMI1(fmiHandle *fmu)
{
    fmi1Handle *fmu1 = loadFmu1(fmu);

    if(fmu1 == NULL) {
        printf("Failed to load FMU file\n");
        exit(1);
    }

    //Loop through variables in FMU
    for(size_t i=0; i<fmi1GetNumberOfVariables(fmu1); ++i)
    {
        fmi1VariableHandle* var = fmi1GetVariableByIndex(fmu1, i);
        const char* name = fmi1GetVariableName(var);
        fmi1DataType type = fmi1GetVariableDataType(var);
        fmi1Causality causality = fmi1GetVariableCausality(var);
        fmi1Variability variability = fmi1GetVariableVariability(var);
        long vr = fmi1GetVariableValueReference(var);

        if(causality == fmi1CausalityOutput)
        {
            if(numOutputs == VAR_MAX) {
                printf("Too many output variables, only printing the first %i\n", VAR_MAX);
            }
            else {
                outputRefs[numOutputs] = vr;
                numOutputs++;
            }
        }
    }

    if(fmi1GetType(fmu1) == fmi1ModelExchange) {
        return testFMI1ME(fmu1);
    }
    else {
        return testFMI1CS(fmu1);
    }
}
