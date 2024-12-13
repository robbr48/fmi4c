#include <stdarg.h>

#include "fmi4c.h"
#include "fmi4c_common.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi1.h"

void loggerFmi1(fmi1Component_t component,
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

    if(status == fmi1OK && logLevel < 4 ||
        status == fmi1Pending && logLevel < 4 ||
        status == fmi1Warning && logLevel < 3 ||
        status == fmi1Discard && logLevel < 3 ||
        status == fmi1Error && logLevel < 2 ||
        status == fmi1Fatal && logLevel < 1) {
        return;
    }

    va_list args;
    va_start(args, message);
    char msgstr[1024];
    sprintf(msgstr, "%s: %s\n", category, message);
    printf(msgstr, args);
    va_end(args);
}

int testFMI1ME(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride) {
    //Instantiate FMU
    if(!fmi1_instantiateModel(fmu, loggerFmi1, calloc, free, fmi1True)) {
        printf("  fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    double stopTime = 1;
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }

    if(fmi1_defaultStartTimeDefined(fmu)) {
        startTime = fmi1_getDefaultStartTime(fmu);
    }
    if(fmi1_defaultStopTimeDefined(fmu)) {
        stopTime = fmi1_getDefaultStopTime(fmu);
    }

    fmi1_setTime(fmu, startTime);


    fmi1Status status;
    fmi1EventInfo eventInfo;
    eventInfo.upcomingTimeEvent = fmi1False;
    eventInfo.stateValuesChanged = fmi1False;
    eventInfo.terminateSimulation = fmi1False;
    eventInfo.iterationConverged = fmi1True;
    eventInfo.nextEventTime = 0.0;
    size_t nStates;
    fmi1Real* states;
    fmi1Real* derivatives;
    size_t nEventIndicators;
    fmi1Real* eventIndicators;
    fmi1Real *eventIndicatorsPrev;
    double actualStepSize;
    fmi1Boolean callEventUpdate = fmi1False;
    fmi1Boolean intermediateResults = fmi1False;

    fmi1_initialize(fmu, fmi1False, 0, &eventInfo);
    printf("  FMU successfully initialized.\n");

    nStates = fmi1_getNumberOfContinuousStates(fmu);
    nEventIndicators = fmi1_getNumberOfEventIndicators(fmu);

    states = malloc(nStates*sizeof(double));
    derivatives = malloc(nStates*sizeof(double));
    eventIndicators = malloc(nEventIndicators*sizeof(double));
    eventIndicatorsPrev = calloc(nEventIndicators, sizeof(double));

    status = fmi1_getContinuousStates(fmu, states, nStates);
    if(status != fmi1OK) {
        printf("fmi1Sefmi1_getContinuousStatestTime() failed\n");
        exit(1);
    }
    status = fmi1_getEventIndicators(fmu, eventIndicators, nEventIndicators);
    if(status != fmi1OK) {
        printf("fmi1_getEventIndicators() failed\n");
        exit(1);
    }

    outputFile = fopen(outputCsvPath, "w");
    if(outputFile != NULL) {
        fprintf(outputFile,"time");
        for(int i=0; i<numOutputs; ++i) {
            fprintf(outputFile,",%s",fmi1_getVariableName(fmi1_getVariableByValueReference(fmu, outputRefs[i])));
        }
        fprintf(outputFile,"\n");
    }

    printf("  Simulating from %f to %f...\n",startTime, stopTime);

    double time = startTime;
    for(; time < stopTime; ) {
        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi1VariableHandle *var = fmi1_getVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                free(derivatives);
                free(eventIndicatorsPrev);
                return 1;
            }
            fmi1Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi1ValueReference vr = fmi1_getVariableValueReference(var);
            fmi1_setReal(fmu, &vr, 1, &value);
        }

        size_t k;
        int zeroCrossingEvent = 0;

        status = fmi1_setTime(fmu, time);
        if(status != fmi1OK) {
            printf("fmi1_setTime() failed\n");
            exit(1);
        }

        status = fmi1_getEventIndicators(fmu, eventIndicators, nEventIndicators);
        if(status != fmi1OK) {
            printf("fmi1_getEventIndicators() failed\n");
            exit(1);
        }
        for (k = 0; k < nEventIndicators; k++) {
            if ((eventIndicators[k] > 0) != (eventIndicatorsPrev[k] > 0)) {
                zeroCrossingEvent = 1;
                break;
            }
        }

        //Handle events
        if (callEventUpdate || zeroCrossingEvent ||
                (eventInfo.upcomingTimeEvent && time == eventInfo.nextEventTime)) {
            status = fmi1_eventUpdate(fmu, intermediateResults, &eventInfo);
            if(status != fmi1OK) {
                printf("fmi1_eventUpdate() failed\n");
                exit(1);
            }
            status = fmi1_getContinuousStates(fmu, states, nStates);
            if(status != fmi1OK) {
                printf("fmi1_getContinuousStates() failed\n");
                exit(1);
            }
            status = fmi1_getEventIndicators(fmu, eventIndicators, nEventIndicators);
            if(status != fmi1OK) {
                printf("fmi1_getEventIndicators() failed\n");
                exit(1);
            }
            status = fmi1_getEventIndicators(fmu, eventIndicatorsPrev, nEventIndicators);
            if(status != fmi1OK) {
                printf("fmi1_getEventIndicators() failed\n");
                exit(1);
            }
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
        status = fmi1_getDerivatives(fmu, derivatives, nStates);
        if(status != fmi1OK) {
            printf("fmi1_getDerivatives() failed\n");
            exit(1);
        }
        for (k = 0; k < nStates; k++) {
            states[k] = states[k] + actualStepSize*derivatives[k];
        }

        status = fmi1_setContinuousStates(fmu, states, nStates);
        if(status != fmi1OK) {
            printf("fmi1_setContinuousStates() failed\n");
            exit(1);
        }
        status = fmi1_completedIntegratorStep(fmu, &callEventUpdate);
        if(status != fmi1OK) {
            printf("fmi1_completedIntegratorStep() failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        if(outputFile != NULL) {
            fprintf(outputFile,"%f",time);
            for(int i=0; i<numOutputs; ++i) {
                fmi1_getReal(fmu, &outputRefs[i], 1, &value);
                fprintf(outputFile,",%f",value);
            }
            fprintf(outputFile,"\n");
        }
    }
    if(outputFile != NULL) {
        fclose(outputFile);
    }
    free(derivatives);
    free(eventIndicatorsPrev);
    free(states);
    free(eventIndicators);

    printf("  Simulation finished.\n");

    fmi1_terminate(fmu);

    printf("  FMU successfully terminated.\n");

    fmi1_freeModelInstance(fmu);

    return 0;
}


int testfmi1_cS(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Instantiate FMU
    if(!fmi1_instantiateSlave(fmu, "application/x-fmu-sharedlibrary", 1000, fmi1False, fmi1False, loggerFmi1, calloc, free, NULL, fmi1True)) {
        printf("fmi1_instantiateSlave() failed\n");
        exit(1);
    }
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    double stopTime = 1;
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }

    if(fmi1_defaultStartTimeDefined(fmu)) {
        startTime = fmi1_getDefaultStartTime(fmu);
    }
    if(fmi1_defaultStopTimeDefined(fmu)) {
        stopTime = fmi1_getDefaultStopTime(fmu);
    }

    fmi1Status status = fmi1_initializeSlave(fmu,startTime,fmi1False,0);
    if(status != fmi1OK) {

        printf("  Initialization failed.\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    outputFile = fopen(outputCsvPath, "w");
    if(outputFile != NULL) {
        fprintf(outputFile,"time");
        for(int i=0; i<numOutputs; ++i) {
            fprintf(outputFile,",%s",fmi1_getVariableName(fmi1_getVariableByValueReference(fmu, outputRefs[i])));
        }
        fprintf(outputFile,"\n");
    }

    double time = startTime;
    while(time <= stopTime) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi1VariableHandle *var = fmi1_getVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi1Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi1ValueReference vr = fmi1_getVariableValueReference(var);
            fmi1_setReal(fmu, &vr, 1, &value);
        }

        //Take a step
        status = fmi1_doStep(fmu, time, stepSize, fmi1True);
        if(status != fmi1OK) {
            printf("fmi1_doStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        if(outputFile != NULL) {
            fprintf(outputFile,"%f",time);
            for(int i=0; i<numOutputs; ++i) {
                fmi1_getReal(fmu, &outputRefs[i], 1, &value);
                fprintf(outputFile,",%f",value);
            }
            fprintf(outputFile,"\n");
        }

        time+=stepSize;
    }
    if(outputFile != NULL) {
        fclose(outputFile);
    }

    printf("  Simulation finished.\n");

    fmi1_terminateSlave(fmu);

    printf("  FMU successfully terminated.\n");

    fmi1_freeSlaveInstance(fmu);

    return 0;
}


int testFMI1(fmiHandle *fmu, bool forceModelExchange, bool forceCosimulation, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Loop through variables in FMU
    for(size_t i=0; i<fmi1_getNumberOfVariables(fmu); ++i)
    {
        fmi1VariableHandle* var = fmi1_getVariableByIndex(fmu, i+1);
        fmi1Causality causality = fmi1_getVariableCausality(var);
        long vr = fmi1_getVariableValueReference(var);

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

    if(fmi1_getType(fmu) == fmi1ModelExchange) {
        if(forceCosimulation) {
            printf("Co-simulation mode not supported by FMU. Aborting.");
            exit(1);
        }
        return testFMI1ME(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else {
        if(forceModelExchange) {
            printf("Mode3l exchange mode not supported by FMU. Aborting.");
            exit(1);
        }
        return testfmi1_cS(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
}
