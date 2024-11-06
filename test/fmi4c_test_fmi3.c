#include "fmi4c.h"
#include "fmi4c_common.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi3.h"

void loggerFmi3(fmi3InstanceEnvironment instanceEnvironment,
                 fmi3Status status,
                 fmi3String category,
                 fmi3String message)
{
    UNUSED(instanceEnvironment)
    UNUSED(category)

    if(status == fmi3OK && logLevel < 4 ||
        status == fmi3Warning && logLevel < 3 ||
        status == fmi3Discard && logLevel < 3 ||
        status == fmi3Error && logLevel < 2 ||
        status == fmi3Fatal && logLevel < 1) {
        return;
    }

    printf("%s: %s\n",category, message);
}


void intermediateUpdate(
        fmi3InstanceEnvironment instanceEnvironment,
        fmi3Float64  intermediateUpdateTime,
        fmi3Boolean  intermediateVariableSetRequested,
        fmi3Boolean  intermediateVariableGetAllowed,
        fmi3Boolean  intermediateStepFinished,
        fmi3Boolean  canReturnEarly,
        fmi3Boolean* earlyReturnRequested,
        fmi3Float64* earlyReturnTime)
{
    UNUSED(intermediateVariableSetRequested);
    UNUSED(canReturnEarly);
    UNUSED(earlyReturnRequested);
    UNUSED(earlyReturnTime);

    printf("Intermediate update at %f\n", intermediateUpdateTime);

    if(intermediateVariableGetAllowed && intermediateStepFinished) {
        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",intermediateUpdateTime);
        for(int i=0; i<numOutputs; ++i) {
            fmi3_getFloat64((fmiHandle *)instanceEnvironment, &outputRefs[i], 1, &value, 1);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
}


int testFMI3CS(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    fmi3Status status;

    int nRequiredIntermediateVariables = 0;
    if(!fmi3_instantiateCoSimulation(fmu, fmi3False, fmi3True, fmi3False, fmi3False, NULL, nRequiredIntermediateVariables, fmu, loggerFmi3, intermediateUpdate)) {
        printf("fmi3InstantiateCoSimulation() failed\n");
        exit(1);
    }
    printf("  FMU successfully instantiated!\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi3_defaultStartTimeDefined(fmu)) {
        startTime = fmi3_getDefaultStartTime(fmu);
    }
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    else if(fmi3_defaultStepSizeDefined(fmu)) {
        stepSize = fmi3_getDefaultStepSize(fmu);
    }
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }
    else if(fmi3_defaultStopTimeDefined(fmu)) {
        stopTime = fmi3_getDefaultStopTime(fmu);
    }

    //Enter initialization mode
    double tstop = 10;
    status = fmi3_enterInitializationMode(fmu, fmi3False, 0, startTime, fmi3True, tstop);
    if(status != fmi3OK) {
        printf("  fmi3EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi3_exitInitializationMode(fmu);
    if(status != fmi3OK) {
        printf("  fmi3ExitInitializationMode() failed\n");
        exit(1);
    }

    printf("  FMU successfully initialized!\n");

    printf("  Simulating from %f to %f with a step size of %f...\n",startTime, stopTime, stepSize);
    outputFile = fopen(outputCsvPath, "w");
    if(outputFile != NULL) {
        fprintf(outputFile,"time");
        for(int i=0; i<numOutputs; ++i) {
            fprintf(outputFile,",%s",fmi3_getVariableName(fmi3_getVariableByValueReference(fmu, outputRefs[i])));
        }
        fprintf(outputFile,"\n");
    }
    double time=startTime;
    while(time <= stopTime) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi3VariableHandle *var = fmi3_getVariableByName(fmu, interpolationData[i].name);
            fmi3Float64 value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi3ValueReference vr = fmi3_getVariableValueReference(var);
            fmi3_setFloat64(fmu, &vr, 1, &value, 1);
        }

        //Take a step
        bool eventEncountered, terminateSimulation, earlyReturn;
        double lastT;
        status = fmi3_doStep(fmu,  time, stepSize, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastT);
        if(status != fmi3OK) {
            printf("fmi3DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        if(outputFile != NULL) {
            fprintf(outputFile,"%f",time);
            for(int i=0; i<numOutputs; ++i) {
                fmi3_getFloat64(fmu, &outputRefs[i], 1, &value, 1);
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

    fmi3_terminate(fmu);

    printf("  FMU successfully terminated.\n");

    fmi3_freeInstance(fmu);

    return 0;
}


int testFMI3ME(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride) {
    //Instantiate FMU
    if(!fmi3_iInstantiateModelExchange(fmu, fmi2False, fmi2True, NULL, loggerFmi3)) {
        printf("  fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;
    double tolerance = 1e-4;

    if(fmi3_defaultStartTimeDefined(fmu)) {
        startTime = fmi3_getDefaultStartTime(fmu);
    }
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    else if(fmi3_defaultStepSizeDefined(fmu)) {
        stepSize = fmi3_getDefaultStepSize(fmu);
    }
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }
    else if(fmi3_defaultStopTimeDefined(fmu)) {
        stopTime = fmi3_getDefaultStopTime(fmu);
    }
    if(fmi3_defaultToleranceDefined(fmu)) {
        tolerance = fmi3_getDefaultTolerance(fmu);
    }

    fmi3Status status;

    //Enter initialization mode
    status = fmi3_enterInitializationMode(fmu,
                                         fmi3_defaultToleranceDefined(fmu), tolerance,
                                         startTime,
                                         fmi3_defaultStopTimeDefined(fmu), stopTime);
    if(status != fmi3OK) {
        printf("  fmi3EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi3_exitInitializationMode(fmu);
    if(status != fmi3OK) {
        printf("  fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    fmi3Boolean terminateSimulation = fmi3False;
    size_t nStates;
    fmi3Float64* states;
    fmi3Float64* nominalStates = NULL;
    fmi3Float64* derivatives;
    size_t nEventIndicators;
    fmi3Float64* eventIndicators;
    fmi3Float64* eventIndicatorsPrev;
    fmi3Boolean discreteStatesNeedUpdate = fmi3False;
    fmi3Boolean nominalsOfContinuousStatesChanged;
    fmi3Boolean valuesOfContinuousStatesChanged;
    fmi3Boolean nextEventTimeDefined = fmi3False;
    fmi3Float64 nextEventTime;
    fmi3Boolean timeEvent;
    fmi3Boolean stateEvent = fmi3False;
    fmi3Boolean stepEvent = fmi3False;
    fmi3Int32* rootsFound;

    status = fmi3_getNumberOfContinuousStates(fmu, &nStates);
    if(status != fmi3OK) {
        printf("  fmi3GetNumberOfContinuousStates() failed\n");
        exit(1);
    }
    status = fmi3_getNumberOfEventIndicators(fmu, &nEventIndicators);
    if(status != fmi3OK) {
        printf("  fmi3GetNumberOfEventIndicators() failed\n");
        exit(1);
    }

    states = malloc(nStates*sizeof(fmi3Float64));
    derivatives = malloc(nStates*sizeof(fmi3Float64));
    eventIndicators = malloc(nEventIndicators*sizeof(fmi3Float64));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(fmi3Float64));
    rootsFound = malloc(nEventIndicators*sizeof(fmi3Int32));

    for(int i=0; i<nEventIndicators; ++i) {
        eventIndicatorsPrev[i] = 0;
        rootsFound[i] = 0;
    }

    while (discreteStatesNeedUpdate) {
        fmi3_updateDiscreteStates(fmu,
                                 &discreteStatesNeedUpdate,
                                 &terminateSimulation,
                                 &nominalsOfContinuousStatesChanged,
                                 &valuesOfContinuousStatesChanged,
                                 &nextEventTimeDefined,
                                 &nextEventTime);

        if (terminateSimulation) {
            return 1;
        }
    }

    fmi3_enterContinuousTimeMode(fmu);

    status = fmi3_getContinuousStates(fmu, states, nStates);
    if(status != fmi3OK) {
        printf("  fmi3GetContinuousStates() failed\n");
        exit(1);
    }
    status = fmi3_getNominalsOfContinuousStates(fmu, nominalStates, nStates);
    if(status != fmi3OK) {
        printf("  fmi3GetNominalsOfContinuousStates() failed\n");
        exit(1);
    }
    status = fmi3_getEventIndicators(fmu, eventIndicators, nEventIndicators);
    if(status != fmi3OK) {
        printf("  fmi3GetEventIndicators() failed\n");
        exit(1);
    }

    outputFile = fopen(outputCsvPath, "w");
    if(outputFile != NULL) {
        fprintf(outputFile,"time");
        for(int i=0; i<numOutputs; ++i) {
            fprintf(outputFile,",%s",fmi3_getVariableName(fmi3_getVariableByValueReference(fmu, outputRefs[i])));
        }
        fprintf(outputFile,"\n");
    }

    printf("  Simulating from %f to %f with a step size of %f...\n",startTime, stopTime, stepSize);

    double time=startTime;
    while(time <= stopTime) {
        if(terminateSimulation || terminateSimulation) {
            printf("Terminating simulation at time = %f\n", time);
            break;
        }

        fmi3_getContinuousStateDerivatives(fmu, derivatives, nStates);

        fmi3_setTime(fmu, time);

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi3VariableHandle *var = fmi3_getVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                free(derivatives);
                free(eventIndicatorsPrev);
                free(rootsFound);
                free(states);
                free(eventIndicators);
                return 1;
            }
            fmi3Float64 value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi3ValueReference vr = fmi3_getVariableValueReference(var);
            fmi3_setFloat64(fmu, &vr, 1, &value, 1);
        }

        //Integrate one step
        for(size_t i=0; i<nStates; ++i) {
            states[i] += stepSize*derivatives[i]; // forward Euler method
        }

        fmi3_setContinuousStates(fmu, states, nStates);

        // get event indicators at t = time
        fmi3_getEventIndicators(fmu, eventIndicators, nEventIndicators);

        stateEvent = fmi3False;

        for (size_t i = 0; i < nEventIndicators; ++i) {

            // check for zero crossings
            if(eventIndicatorsPrev[i] < 0 && eventIndicators[i] >= 0) {
                rootsFound[i] = 1;
            }
            else if (eventIndicatorsPrev[i] > 0 && eventIndicators[i] <= 0) {
                rootsFound[i] = -1;
            } else {
                rootsFound[i] = 0;
            }

            stateEvent |= rootsFound[i];

            eventIndicatorsPrev[i] = eventIndicators[i];
        }

        //Print all output variables to CSV file
        double value;
        if(outputFile != NULL) {
            fprintf(outputFile,"%f",time);
            for(int i=0; i<numOutputs; ++i) {
                fmi3_getFloat64(fmu, &outputRefs[i], 1, &value, 1 );
                fprintf(outputFile,",%f",value);
            }
            fprintf(outputFile,"\n");
        }

        fmi3_completedIntegratorStep(fmu, fmi3True, &stepEvent, &terminateSimulation);

        time+=stepSize;
    }
    free(derivatives);
    free(eventIndicatorsPrev);
    free(rootsFound);
    free(states);
    free(eventIndicators);
    printf("  Simulation finished.\n");

    fmi3_terminate(fmu);

    printf("  FMU successfully terminated.\n");

    fmi3_freeInstance(fmu);

    return 0;
}


int testFMI3(fmiHandle *fmu, bool forceModelExchange, bool forceCosimulation, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Loop through variables in FMU
    for(size_t i=0; i<fmi3_getNumberOfVariables(fmu); ++i)
    {
        fmi3VariableHandle* var = fmi3_getVariableByIndex(fmu, i);
        fmi3Causality causality = fmi3_getVariableCausality(var);
        unsigned int vr = fmi3_getVariableValueReference(var);

        if(causality == fmi3CausalityOutput)
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

    if(forceModelExchange && !fmi3_supportsModelExchange(fmu)) {
        printf("Model exchange mode not supported by FMU. Aborting.");
        exit(1);
    }
    else if(forceCosimulation && !fmi3_supportsCoSimulation(fmu)) {
        printf("Co-simulation mode not supported by FMU. Aborting.");
        exit(1);
    }

    if(fmi3_supportsCoSimulation(fmu) && !forceModelExchange) {
        return testFMI3CS(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else if(fmi3_supportsModelExchange(fmu)) {
        return testFMI3ME(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else {
        printf("Requested FMU type cannot be simulated.\n");
        return 1;
    }
}
