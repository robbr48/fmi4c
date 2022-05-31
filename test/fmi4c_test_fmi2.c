#include <stdarg.h>

#include "fmi4c.h"
#include "fmi4c_test.h"
#include "fmi4c_test_fmi2.h"

void loggerFmi2(fmi2ComponentEnvironment componentEnvironment,
                fmi2String instanceName,
                fmi2Status status,
                fmi2String category,
                fmi2String message,
                ...)
{
    UNUSED(componentEnvironment)
            UNUSED(instanceName)
            UNUSED(status)
            UNUSED(category)

            va_list args;
    va_start(args, message);
    printf("%s: %s\n",message, args);
    va_end(args);
}


int testFMI2ME(fmiHandle *fmu)
{
    //Instantiate FMU
    if(!fmi2Instantiate(fmu, fmi2ModelExchange, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("--- Test simulation ---\n");
    printf("  FMU successfully instantiated.\n");


    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi2DefaultStartTimeDefined(fmu)) {
        startTime = fmi2GetDefaultStartTime(fmu);
    }
    if(fmi2DefaultStepSizeDefined(fmu)) {
        stepSize = fmi2GetDefaultStepSize(fmu);
    }
    if(fmi2DefaultStopTimeDefined(fmu)) {
        stopTime = fmi2GetDefaultStopTime(fmu);
    }

    fmi2Status status;

    //Setup experiment
    status = fmi2SetupExperiment(fmu, fmi2False, 0, startTime, fmi2False, 0.0);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Enter initialization mode
    status = fmi2EnterInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi2ExitInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    double actualStepSize = stepSize;
    fmi2Boolean terminateSimulation = fmi2False;
    fmi2Boolean callEventUpdate = fmi2False;
    fmi2EventInfo eventInfo;
    size_t nStates;
    fmi2Real* states;
    fmi2Real* derivatives;
    size_t nEventIndicators;
    fmi2Real* eventIndicators;
    fmi2Real *eventIndicatorsPrev;
    eventInfo.newDiscreteStatesNeeded = fmi2False;
    eventInfo.terminateSimulation = fmi2False;
    eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
    eventInfo.valuesOfContinuousStatesChanged = fmi2True;
    eventInfo.nextEventTimeDefined = fmi2False;
    eventInfo.nextEventTime = 0.0;

    nStates = fmi2GetNumberOfContinuousStates(fmu);
    nEventIndicators = fmi2GetNumberOfEventIndicators(fmu);

    states = malloc(nStates*sizeof(double));
    derivatives = malloc(nStates*sizeof(double));
    eventIndicators = malloc(nEventIndicators*sizeof(double));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(double));

    //Perform event iteration
    eventInfo.newDiscreteStatesNeeded = fmi2True;
    eventInfo.terminateSimulation     = fmi2False;
    while (eventInfo.newDiscreteStatesNeeded && !eventInfo.terminateSimulation) {
        fmi2NewDiscreteStates(fmu, &eventInfo);
    }
    fmi2EnterContinuousTimeMode(fmu);

    status = fmi2GetContinuousStates(fmu, states, nStates);
    if(status != fmi2OK) {
        printf("fmi2GetContinuousStates() failed\n");
        exit(1);
    }
    status = fmi2GetEventIndicators(fmu, eventIndicators, nEventIndicators);
    if(status != fmi2OK) {
        printf("fmi2GetEventIndicators() failed\n");
        exit(1);
    }

    outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi2GetVariableName(fmi2GetVariableByValueReference(fmu, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    for(double time=startTime; time < stopTime;) {
        if(eventInfo.terminateSimulation || terminateSimulation) {
            printf("Terminating simulation at time = %f\n", time);
            break;
        }

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi2VariableHandle *var = fmi2GetVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi2Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi2ValueReference vr = fmi2GetVariableValueReference(var);
            fmi2SetReal(fmu, &vr, 1, &value);
        }

        size_t k;
        fmi2Real lastTime;
        int zeroCrossingEvent = 0;

        status = fmi2SetTime(fmu, time);
        if(status != fmi2OK) {
            printf("fmi2SetTime() failed\n");
            exit(1);
        }

        eventIndicatorsPrev = eventIndicators;
        status = fmi2GetEventIndicators(fmu, eventIndicators, nEventIndicators);
        if(status != fmi2OK) {
            printf("fmi2GetEventIndicators() failed\n");
            exit(1);
        }

        /* Check if an event indicator has triggered */
        for (k = 0; k < nEventIndicators; k++) {
            if ((eventIndicators[k] > 0) != (eventIndicatorsPrev[k] > 0)) {
                zeroCrossingEvent = 1;
                break;
            }
        }

        //Handle events
        if (callEventUpdate || zeroCrossingEvent ||
                (eventInfo.nextEventTimeDefined && time == eventInfo.nextEventTime)) {
            status = fmi2EnterEventMode(fmu);
            if(status != fmi2OK) {
                printf("fmi2EnterEventMode() failed\n");
                exit(1);
            }

            //Perform event iteration
            eventInfo.newDiscreteStatesNeeded = fmi2True;
            eventInfo.terminateSimulation     = fmi2False;
            while (eventInfo.newDiscreteStatesNeeded && !eventInfo.terminateSimulation) {
                fmi2NewDiscreteStates(fmu, &eventInfo);
            }
            status = fmi2EnterContinuousTimeMode(fmu);
            if(status != fmi2OK) {
                printf("fmi2EnterContinuousTimeMode() failed\n");
                exit(1);
            }
            status = fmi2GetContinuousStates(fmu, states, nStates);
            if(status != fmi2OK) {
                printf("fmi2GetContinuousStates() failed\n");
                exit(1);
            }
            status = fmi2GetEventIndicators(fmu, eventIndicators, nEventIndicators);
            if(status != fmi2OK) {
                printf("fmi2GetEventIndicators() failed\n");
                exit(1);
            }
        }

        //Update actual time stpe
        lastTime = time;
        time += stepSize;
        if (eventInfo.nextEventTimeDefined && (time >= eventInfo.nextEventTime)) {
            time = eventInfo.nextEventTime;
        }
        actualStepSize = time - lastTime;
        if(time > stopTime) {
            time = stopTime;
            actualStepSize = time - lastTime;
        }

        //Perform integration
        status = fmi2GetDerivatives(fmu, derivatives, nStates);
        if(status != fmi2OK) {
            printf("fmi2GetDerivatives() failed\n");
            exit(1);
        }
        for (k = 0; k < nStates; k++) {
            states[k] = states[k] + actualStepSize*derivatives[k];
        }

        status = fmi2SetContinuousStates(fmu, states, nStates);
        if(status != fmi2OK) {
            printf("fmi2SetContinuousStates() failed\n");
            exit(1);
        }
        status = fmi2CompletedIntegratorStep(fmu, fmi2True, &callEventUpdate, &terminateSimulation);
        if(status != fmi2OK) {
            printf("fmi2CompletedIntegratorStep() failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi2GetReal(fmu, &outputRefs[i], 1, &value);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);

    printf("  Simulation finished.\n");

    fmi2Terminate(fmu);
    printf("  FMU successfully terminated.\n");

    fmi2FreeInstance(fmu);
    return 0;
}


int testFMI2CS(fmiHandle *fmu)
{
    //Instantiate FMU
    if(!fmi2Instantiate(fmu, fmi2CoSimulation, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("--- Test simulation ---\n");
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi2DefaultStartTimeDefined(fmu)) {
        startTime = fmi2GetDefaultStartTime(fmu);
    }
    if(fmi2DefaultStepSizeDefined(fmu)) {
        stepSize = fmi2GetDefaultStepSize(fmu);
    }
    if(fmi2DefaultStopTimeDefined(fmu)) {
        stopTime = fmi2GetDefaultStopTime(fmu);
    }

    fmi2Status status;

    //Setup experiment
    status = fmi2SetupExperiment(fmu, fmi2False, 0, startTime, fmi2False, 0.0);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Enter initialization mode
    status = fmi2EnterInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi2ExitInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi2GetVariableName(fmi2GetVariableByValueReference(fmu, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    for(double time=startTime; time <= stopTime; time+=stepSize) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi2VariableHandle *var = fmi2GetVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi2Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi2ValueReference vr = fmi2GetVariableValueReference(var);
            fmi2SetReal(fmu, &vr, 1, &value);
        }

        //Take a step
        status = fmi2DoStep(fmu, time, stepSize, fmi2True);
        if(status != fmi2OK) {
            printf("fmi2DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi2GetReal(fmu, &outputRefs[i], 1, &value);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);
    printf("  Simulation finished.\n");

    fmi2Terminate(fmu);
    printf("  FMU successfully terminated.\n");
    fmi2FreeInstance(fmu);
    return 0;
}


int testFMI2(fmiHandle *fmu, bool forceModelExchange)
{
    //Loop through variables in FMU
    for(size_t i=0; i<fmi2GetNumberOfVariables(fmu); ++i)
    {
        fmi2VariableHandle* var = fmi2GetVariableByIndex(fmu, i);
        const char* name = fmi2GetVariableName(var);
        fmi2DataType type = fmi2GetVariableDataType(var);
        fmi2Causality causality = fmi2GetVariableCausality(var);
        unsigned int vr = fmi2GetVariableValueReference(var);

        if(causality == fmi2CausalityOutput)
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

    if(fmi2GetSupportsCoSimulation(fmu) && !forceModelExchange) {
        return testFMI2CS(fmu);
    }
    else if(fmi2GetSupportsModelExchange(fmu)){
        return testFMI2ME(fmu);
    }
    else {
        printf("Failed to simulate FMU: Requested type not supported.\n");
        return 1;
    }
}
