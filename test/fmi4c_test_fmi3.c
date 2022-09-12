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
    UNUSED(status)
    UNUSED(category)

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
            fmi3GetFloat64((fmiHandle *)instanceEnvironment, &outputRefs[i], 1, &value, 1);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
}


int testFMI3CS(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    fmi3Status status;

    int nRequiredIntermediateVariables = 0;
    if(!fmi3InstantiateCoSimulation(fmu, fmi3False, fmi3True, fmi3False, fmi3False, NULL, nRequiredIntermediateVariables, NULL, loggerFmi3, intermediateUpdate)) {
        printf("fmi3InstantiateCoSimulation() failed\n");
        exit(1);
    }
    printf("  FMU successfully instantiated!\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi3DefaultStartTimeDefined(fmu)) {
        startTime = fmi3GetDefaultStartTime(fmu);
    }
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    else if(fmi3DefaultStepSizeDefined(fmu)) {
        stepSize = fmi3GetDefaultStepSize(fmu);
    }
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }
    else if(fmi3DefaultStopTimeDefined(fmu)) {
        stopTime = fmi3GetDefaultStopTime(fmu);
    }

    //Enter initialization mode
    double tstop = 10;
    status = fmi3EnterInitializationMode(fmu, fmi3False, 0, startTime, fmi3True, tstop);
    if(status != fmi3OK) {
        printf("  fmi3EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi3ExitInitializationMode(fmu);
    if(status != fmi3OK) {
        printf("  fmi3ExitInitializationMode() failed\n");
        exit(1);
    }

    printf("  FMU successfully initialized!\n");

    printf("  Simulating from %f to %f with a step size of %f...\n",startTime, stopTime, stepSize);
    outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi3GetVariableName(fmi3GetVariableByValueReference(fmu, outputRefs[i])));
    }
    fprintf(outputFile,"\n");
    for(double time=startTime; time <= stopTime; time+=stepSize) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi3VariableHandle *var = fmi3GetVariableByName(fmu, interpolationData[i].name);
            fmi3Float64 value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi3ValueReference vr = fmi3GetVariableValueReference(var);
            fmi3SetFloat64(fmu, &vr, 1, &value, 1);
        }

        //Take a step
        bool eventEncountered, terminateSimulation, earlyReturn;
        double lastT;
        status = fmi3DoStep(fmu,  time, stepSize, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastT);
        if(status != fmi3OK) {
            printf("fmi3DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi3GetFloat64(fmu, &outputRefs[i], 1, &value, 1);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);

    printf("  Simulation finished.\n");

    fmi3Terminate(fmu);

    printf("  FMU successfully terminated.\n");

    fmi3FreeInstance(fmu);

    return 0;
}


int testFMI3ME(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride) {
    //Instantiate FMU
    if(!fmi3InstantiateModelExchange(fmu, fmi2False, fmi2True, NULL, loggerFmi3)) {
        printf("  fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;
    double tolerance;

    if(fmi3DefaultStartTimeDefined(fmu)) {
        startTime = fmi3GetDefaultStartTime(fmu);
    }
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    else if(fmi3DefaultStepSizeDefined(fmu)) {
        stepSize = fmi3GetDefaultStepSize(fmu);
    }
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }
    else if(fmi3DefaultStopTimeDefined(fmu)) {
        stopTime = fmi3GetDefaultStopTime(fmu);
    }
    if(fmi3DefaultToleranceDefined(fmu)) {
        tolerance = fmi3GetDefaultTolerance(fmu);
    }

    fmi3Status status;

    //Enter initialization mode
    status = fmi3EnterInitializationMode(fmu,
                                         fmi3DefaultToleranceDefined(fmu), tolerance,
                                         startTime,
                                         fmi3DefaultStopTimeDefined(fmu), stopTime);
    if(status != fmi3OK) {
        printf("  fmi3EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi3ExitInitializationMode(fmu);
    if(status != fmi3OK) {
        printf("  fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    fmi3Boolean terminateSimulation = fmi3False;
    size_t nStates;
    fmi3Float64* states;
    fmi3Float64* nominalStates;
    fmi3Float64* derivatives;
    size_t nEventIndicators;
    fmi3Float64* eventIndicators;
    fmi3Float64* eventIndicatorsPrev;
    fmi3Boolean discreteStatesNeedUpdate = fmi3False;
    fmi3Boolean nominalsOfContinuousStatesChanged;
    fmi3Boolean valuesOfContinuousStatesChanged;
    fmi3Boolean nextEventTimeDefined;
    fmi3Float64 nextEventTime;
    fmi3Boolean timeEvent;
    fmi3Boolean stateEvent;
    fmi3Boolean stepEvent;
    fmi3Int32* rootsFound;

    status = fmi3GetNumberOfContinuousStates(fmu, &nStates);
    if(status != fmi3OK) {
        printf("  fmi3GetNumberOfContinuousStates() failed\n");
        exit(1);
    }
    status = fmi3GetNumberOfEventIndicators(fmu, &nEventIndicators);
    if(status != fmi3OK) {
        printf("  fmi3GetNumberOfEventIndicators() failed\n");
        exit(1);
    }

    states = malloc(nStates*sizeof(fmi3Float64));
    derivatives = malloc(nStates*sizeof(fmi3Float64));
    eventIndicators = malloc(nEventIndicators*sizeof(fmi3Float64));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(fmi3Float64));
    rootsFound = malloc(nEventIndicators*sizeof(fmi3Int64));

    for(int i=0; i<nEventIndicators; ++i) {
        rootsFound[i] = 0;
    }

    while (discreteStatesNeedUpdate) {
        fmi3UpdateDiscreteStates(fmu,
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

    fmi3EnterContinuousTimeMode(fmu);

    status = fmi3GetContinuousStates(fmu, states, nStates);
    if(status != fmi3OK) {
        printf("  fmi3GetContinuousStates() failed\n");
        exit(1);
    }
    status = fmi3GetNominalsOfContinuousStates(fmu, nominalStates, nStates);
    if(status != fmi3OK) {
        printf("  fmi3GetNominalsOfContinuousStates() failed\n");
        exit(1);
    }
    status = fmi3GetEventIndicators(fmu, eventIndicators, nEventIndicators);
    if(status != fmi3OK) {
        printf("  fmi3GetEventIndicators() failed\n");
        exit(1);
    }

    outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi3GetVariableName(fmi3GetVariableByValueReference(fmu, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    printf("  Simulating from %f to %f with a step size of %f...\n",startTime, stopTime, stepSize);

    for(double time=startTime; time <= stopTime; time+=stepSize) {
        if(terminateSimulation || terminateSimulation) {
            printf("Terminating simulation at time = %f\n", time);
            break;
        }

        fmi3GetContinuousStateDerivatives(fmu, derivatives, nStates);

        fmi3SetTime(fmu, time);

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi3VariableHandle *var = fmi3GetVariableByName(fmu, interpolationData[i].name);
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
            fmi3ValueReference vr = fmi3GetVariableValueReference(var);
            fmi3SetFloat64(fmu, &vr, 1, &value, 1);
        }

        //Integrate one step
        for(size_t i=0; i<nStates; ++i) {
            states[i] += stepSize*derivatives[i]; // forward Euler method
        }

        fmi3SetContinuousStates(fmu, states, nStates);

        // get event indicators at t = time
        fmi3GetEventIndicators(fmu, eventIndicators, nEventIndicators);

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
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi3GetFloat64(fmu, &outputRefs[i], 1, &value, 1 );
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");

        fmi3CompletedIntegratorStep(fmu, fmi3True, &stepEvent, &terminateSimulation);
    }
    free(derivatives);
    free(eventIndicatorsPrev);
    free(rootsFound);
    free(states);
    free(eventIndicators);
    printf("  Simulation finished.\n");

    fmi3Terminate(fmu);

    printf("  FMU successfully terminated.\n");

    fmi3FreeInstance(fmu);

    return 0;
}


int testFMI3(fmiHandle *fmu, bool forceModelExchange, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Loop through variables in FMU
    for(size_t i=0; i<fmi3GetNumberOfVariables(fmu); ++i)
    {
        fmi3VariableHandle* var = fmi3GetVariableByIndex(fmu, i);
        const char* name = fmi3GetVariableName(var);
        fmi3DataType type = fmi3GetVariableDataType(var);
        fmi3Causality causality = fmi3GetVariableCausality(var);
        unsigned int vr = fmi3GetVariableValueReference(var);

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

    if(fmi3SupportsCoSimulation(fmu) && !forceModelExchange) {
        return testFMI3CS(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else if(fmi3SupportsModelExchange(fmu)) {
        return testFMI3ME(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else {
        printf("Requested FMU type cannot be simulated.\n");
        return 1;
    }
}
