#include "fmi4c.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi3.h"

void  loggerFmi3(fmi3InstanceEnvironment instanceEnvironment,
                 fmi3String instanceName,
                 fmi3Status status,
                 fmi3String category,
                 fmi3String message)
{
    UNUSED(instanceEnvironment)
            UNUSED(instanceName)
            UNUSED(status)
            UNUSED(category)

            printf("%s\n",message);
}


void intermediateUpdate(
        fmi3InstanceEnvironment instanceEnvironment,
        fmi3Float64  intermediateUpdateTime,
        fmi3Boolean  clocksTicked,
        fmi3Boolean  intermediateVariableSetRequested,
        fmi3Boolean  intermediateVariableGetAllowed,
        fmi3Boolean  intermediateStepFinished,
        fmi3Boolean  canReturnEarly,
        fmi3Boolean* earlyReturnRequested,
        fmi3Float64* earlyReturnTime)
{
    UNUSED(clocksTicked);
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
            fmi3GetFloat64((fmi3Handle *)instanceEnvironment, &outputRefs[i], 1, &value, 1);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
}


int testFMI3CS(fmi3Handle *fmu3)
{
    fmi3Status status;

    int nRequiredIntermediateVariables = 0;
    if(!fmi3InstantiateCoSimulation(fmu3, fmi3False, fmi3True, fmi3False, fmi3False, NULL, nRequiredIntermediateVariables, NULL, loggerFmi3, intermediateUpdate)) {
        printf("fmi3InstantiateCoSimulation() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi3DefaultStartTimeDefined(fmu3)) {
        startTime = fmi3GetDefaultStartTime(fmu3);
    }
    if(fmi3DefaultStepSizeDefined(fmu3)) {
        stepSize = fmi3GetDefaultStepSize(fmu3);
    }
    if(fmi3DefaultStopTimeDefined(fmu3)) {
        stopTime = fmi3GetDefaultStopTime(fmu3);
    }

    //Enter initialization mode
    double tstop = 10;
    status = fmi3EnterInitializationMode(fmu3, fmi3False, 0, startTime, fmi3True, tstop);
    if(status != fmi3OK) {
        printf("fmi3EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi3ExitInitializationMode(fmu3);
    if(status != fmi3OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }

    printf("FMU successfully initialized!\n");

    printf("Simulating from %f to %f with a step size of %f...\n",startTime, stopTime, stepSize);
    outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi3GetVariableName(fmi3GetVariableByValueReference(fmu3, outputRefs[i])));
    }
    fprintf(outputFile,"\n");
    for(double time=startTime; time <= stopTime; time+=stepSize) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi3VariableHandle *var = fmi3GetVariableByName(fmu3, interpolationData[i].name);
            fmi3Float64 value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi3ValueReference vr = fmi3GetVariableValueReference(var);
            fmi3SetFloat64(fmu3, &vr, 1, &value, 1);
        }

        //Take a step
        bool eventEncountered, terminateSimulation, earlyReturn;
        double lastT;
        status = fmi3DoStep(fmu3,  time, stepSize, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastT);
        if(status != fmi3OK) {
            printf("fmu3DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi3GetFloat64(fmu3, &outputRefs[i], 1, &value, 1);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);

    fmi3Terminate(fmu3);
    fmi3FreeInstance(fmu3);

    return 0;
}


int testFMI3ME(fmi3Handle *fmu3) {
    //Instantiate FMU
    if(!fmi3InstantiateModelExchange(fmu3, fmi2False, fmi2True, NULL, loggerFmi3)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated (FMI 3 ME)!\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;
    double tolerance;

    if(fmi3DefaultStartTimeDefined(fmu3)) {
        startTime = fmi3GetDefaultStartTime(fmu3);
    }
    if(fmi3DefaultStepSizeDefined(fmu3)) {
        stepSize = fmi3GetDefaultStepSize(fmu3);
    }
    if(fmi3DefaultStopTimeDefined(fmu3)) {
        stopTime = fmi3GetDefaultStopTime(fmu3);
    }
    if(fmi3DefaultToleranceDefined(fmu3)) {
        tolerance = fmi3GetDefaultTolerance(fmu3);
    }

    printf("Debug 0\n");

    fmi3Status status;

    //Enter initialization mode
    status = fmi3EnterInitializationMode(fmu3,
                                         fmi3DefaultToleranceDefined(fmu3), tolerance,
                                         startTime,
                                         fmi3DefaultStopTimeDefined(fmu3), stopTime);
    if(status != fmi3OK) {
        printf("fmi3EnterInitializationMode() failed\n");
        exit(1);
    }

    printf("Debug 0.1\n");

    //Exit initialization mode
    status = fmi3ExitInitializationMode(fmu3);
    if(status != fmi3OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("FMU successfully initialized!\n");

    printf("Debug 0.2\n");

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

    status = fmi3GetNumberOfContinuousStates(fmu3, &nStates);
    status = fmi3GetNumberOfEventIndicators(fmu3, &nEventIndicators);

    states = malloc(nStates*sizeof(fmi3Float64));
    derivatives = malloc(nStates*sizeof(fmi3Float64));
    eventIndicators = malloc(nEventIndicators*sizeof(fmi3Float64));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(fmi3Float64));
    rootsFound = malloc(nEventIndicators*sizeof(fmi3Int64));

    for(int i=0; i<nEventIndicators; ++i) {
        rootsFound[i] = 0;
    }

    printf("Debug 1\n");

    while (discreteStatesNeedUpdate) {
        fmi3UpdateDiscreteStates(fmu3,
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

    printf("Debug 2\n");

    fmi3EnterContinuousTimeMode(fmu3);



    status = fmi3GetContinuousStates(fmu3, states, nStates);
    status = fmi3GetNominalsOfContinuousStates(fmu3, nominalStates, nStates);
    status = fmi3GetEventIndicators(fmu3, eventIndicators, nEventIndicators);

    outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi3GetVariableName(fmi3GetVariableByValueReference(fmu3, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    for(double time=startTime; time <= stopTime; time+=stepSize) {
        if(terminateSimulation || terminateSimulation) {
            printf("Terminating simulation at time = %d\n", time);
            break;
        }

        printf("Debug 3\n");

        timeEvent = nextEventTimeDefined && time >= nextEventTime;

        if (timeEvent || stateEvent || stepEvent) {

            printf("Debug 4\n");

            fmi3EnterEventMode(fmu3, stepEvent, stateEvent, rootsFound, nEventIndicators, timeEvent);

            nominalsOfContinuousStatesChanged = fmi3False;
            valuesOfContinuousStatesChanged   = fmi3False;

            //Event iteration
            bool doTerminate = false;
            do {
                printf("Debug 5\n");
                // set inputs at super dense time point
                // S->fmi3SetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)

                fmi3Boolean nominalsChanged = fmi3False;
                fmi3Boolean statesChanged   = fmi3False;

                // update discrete states
                fmi3UpdateDiscreteStates(fmu3, &discreteStatesNeedUpdate, &terminateSimulation, &nominalsChanged, &statesChanged, &nextEventTimeDefined, &nextEventTime);

                nominalsOfContinuousStatesChanged |= nominalsChanged;
                valuesOfContinuousStatesChanged   |= statesChanged;

                if (terminateSimulation) {
                    doTerminate = true;
                    break;
                }

            } while (discreteStatesNeedUpdate);

            printf("Debug 6\n");

            if(doTerminate) {
                break;
            }

            // enter Continuous-Time Mode
            fmi3EnterContinuousTimeMode(fmu3);

            if (valuesOfContinuousStatesChanged) {
                fmi3GetContinuousStates(fmu3, states, nStates);
            }

            if (nominalsOfContinuousStatesChanged) {
                fmi3GetNominalsOfContinuousStates(fmu3, nominalStates, nStates);
            }
        }

        printf("Debug 7\n");

        fmi3GetContinuousStateDerivatives(fmu3, derivatives, nStates);

        fmi3SetTime(fmu3, time);

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi3VariableHandle *var = fmi3GetVariableByName(fmu3, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi3Float64 value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi3ValueReference vr = fmi3GetVariableValueReference(var);
            fmi3SetFloat64(fmu3, &vr, 1, &value, 1);
        }

        //Integrate one step
        for(size_t i=0; i<nStates; ++i) {
            states[i] += stepSize*derivatives[i]; // forward Euler method
        }

        fmi3SetContinuousStates(fmu3, states, nStates);

        // get event indicators at t = time
        fmi3GetEventIndicators(fmu3, eventIndicators, nEventIndicators);

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
            fmi3GetFloat64(fmu3, &outputRefs[i], 1, &value, 1 );
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");

        fmi3CompletedIntegratorStep(fmu3, fmi3True, &stepEvent, &terminateSimulation);
    }

    fmi3Terminate(fmu3);
    fmi3FreeInstance(fmu3);

    return 0;
}


int testFMI3(fmiHandle *fmu, bool forceModelExchange)
{
    fmi3Handle *fmu3 = loadFmu3(fmu);
    if(fmu3 == NULL) {
        printf("Failed to load FMU file\n");
        exit(1);
    }

    //Loop through variables in FMU
    for(size_t i=0; i<fmi3GetNumberOfVariables(fmu3); ++i)
    {
        fmi3VariableHandle* var = fmi3GetVariableByIndex(fmu3, i);
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

    if(fmi3SupportsCoSimulation(fmu3) && !forceModelExchange) {
        return testFMI3CS(fmu3);
    }
    else if(fmi3SupportsModelExchange(fmu3)) {
        return testFMI3ME(fmu3);
    }
    else {
        printf("Requested FMU type cannot be simulated.\n");
        return 1;
    }
}
