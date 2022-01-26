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
    printf("%s\n",message, args);
    va_end(args);
}


int testFMI2ME(fmi2Handle *fmu2)
{
    //Instantiate FMU
    if(!fmi2Instantiate(fmu2, fmi2ModelExchange, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("--- Test simulation ---\n");
    printf("  FMU successfully instantiated.\n");


    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi2DefaultStartTimeDefined(fmu2)) {
        startTime = fmi2GetDefaultStartTime(fmu2);
    }
    if(fmi2DefaultStepSizeDefined(fmu2)) {
        stepSize = fmi2GetDefaultStepSize(fmu2);
    }
    if(fmi2DefaultStopTimeDefined(fmu2)) {
        stopTime = fmi2GetDefaultStopTime(fmu2);
    }

    fmi2Status status;

    //Setup experiment
    status = fmi2SetupExperiment(fmu2, fmi2False, 0, startTime, fmi2False, 0.0);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Enter initialization mode
    status = fmi2EnterInitializationMode(fmu2);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi2ExitInitializationMode(fmu2);
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

    nStates = fmi2GetNumberOfContinuousStates(fmu2);
    nEventIndicators = fmi2GetNumberOfEventIndicators(fmu2);

    states = malloc(nStates*sizeof(double));
    derivatives = malloc(nStates*sizeof(double));
    eventIndicators = malloc(nEventIndicators*sizeof(double));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(double));

    //Perform event iteration
    eventInfo.newDiscreteStatesNeeded = fmi2True;
    eventInfo.terminateSimulation     = fmi2False;
    while (eventInfo.newDiscreteStatesNeeded && !eventInfo.terminateSimulation) {
        fmi2NewDiscreteStates(fmu2, &eventInfo);
    }
    fmi2EnterContinuousTimeMode(fmu2);

    status = fmi2GetContinuousStates(fmu2, states, nStates);
    status = fmi2GetEventIndicators(fmu2, eventIndicators, nEventIndicators);

    FILE *outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi2GetVariableName(fmi2GetVariableByValueReference(fmu2, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    for(double time=startTime; time <= stopTime; time+=actualStepSize) {
        if(eventInfo.terminateSimulation || terminateSimulation) {
            printf("Terminating simulation at time = %d\n", time);
            break;
        }

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi2VariableHandle *var = fmi2GetVariableByName(fmu2, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi2Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi2ValueReference vr = fmi2GetVariableValueReference(var);
            fmi2SetReal(fmu2, &vr, 1, &value);
        }

        size_t k;
        fmi2Real lastTime;
        int zeroCrossingEvent = 0;

        status = fmi2SetTime(fmu2, time);

        eventIndicatorsPrev = eventIndicators;
        status = fmi2GetEventIndicators(fmu2, eventIndicators, nEventIndicators);

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
            status = fmi2EnterEventMode(fmu2);

            //Perform event iteration
            eventInfo.newDiscreteStatesNeeded = fmi2True;
            eventInfo.terminateSimulation     = fmi2False;
            while (eventInfo.newDiscreteStatesNeeded && !eventInfo.terminateSimulation) {
                fmi2NewDiscreteStates(fmu2, &eventInfo);
            }
            status = fmi2EnterContinuousTimeMode(fmu2);
            status = fmi2GetContinuousStates(fmu2, states, nStates);
            status = fmi2GetEventIndicators(fmu2, eventIndicators, nEventIndicators);
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
        status = fmi2GetDerivatives(fmu2, derivatives, nStates);
        for (k = 0; k < nStates; k++) {
            states[k] = states[k] + actualStepSize*derivatives[k];
        }

        status = fmi2SetContinuousStates(fmu2, states, nStates);
        status = fmi2CompletedIntegratorStep(fmu2, fmi2True, &callEventUpdate, &terminateSimulation);

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi2GetReal(fmu2, &outputRefs[i], 1, &value);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);

    printf("  Simulation finished.\n");

    fmi2Terminate(fmu2);
    printf("  FMU successfully terminated.\n");

    fmi2FreeInstance(fmu2);
}


int testFMI2CS(fmi2Handle *fmu2)
{
    //Instantiate FMU
    if(!fmi2Instantiate(fmu2, fmi2CoSimulation, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("--- Test simulation ---\n");
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi2DefaultStartTimeDefined(fmu2)) {
        startTime = fmi2GetDefaultStartTime(fmu2);
    }
    if(fmi2DefaultStepSizeDefined(fmu2)) {
        stepSize = fmi2GetDefaultStepSize(fmu2);
    }
    if(fmi2DefaultStopTimeDefined(fmu2)) {
        stopTime = fmi2GetDefaultStopTime(fmu2);
    }

    fmi2Status status;

    //Setup experiment
    status = fmi2SetupExperiment(fmu2, fmi2False, 0, startTime, fmi2False, 0.0);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Enter initialization mode
    status = fmi2EnterInitializationMode(fmu2);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi2ExitInitializationMode(fmu2);
    if(status != fmi2OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    FILE *outputFile = fopen(outputCsvPath, "w");
    fprintf(outputFile,"time");
    for(int i=0; i<numOutputs; ++i) {
        fprintf(outputFile,",%s",fmi2GetVariableName(fmi2GetVariableByValueReference(fmu2, outputRefs[i])));
    }
    fprintf(outputFile,"\n");

    for(double time=startTime; time <= stopTime; time+=stepSize) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi2VariableHandle *var = fmi2GetVariableByName(fmu2, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi2Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi2ValueReference vr = fmi2GetVariableValueReference(var);
            fmi2SetReal(fmu2, &vr, 1, &value);
        }

        //Take a step
        status = fmi2DoStep(fmu2, time, stepSize, fmi2True);

        if(status != fmi2OK) {
            printf("fmi2DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        fprintf(outputFile,"%f",time);
        for(int i=0; i<numOutputs; ++i) {
            fmi2GetReal(fmu2, &outputRefs[i], 1, &value);
            fprintf(outputFile,",%f",value);
        }
        fprintf(outputFile,"\n");
    }
    fclose(outputFile);
    printf("  Simulation finished.\n");

    fmi2Terminate(fmu2);
    printf("  FMU successfully terminated.\n");
    fmi2FreeInstance(fmu2);
}


int testFMI2(fmiHandle *fmu, bool forceModelExchange)
{
    fmi2Handle *fmu2 = loadFmu2(fmu);

    if(fmu2 == NULL) {
        printf("Failed to load FMU file\n");
        exit(1);
    }

    //Loop through variables in FMU
    for(size_t i=0; i<fmi2GetNumberOfVariables(fmu2); ++i)
    {
        fmi2VariableHandle* var = fmi2GetVariableByIndex(fmu2, i);
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

    if(fmi2GetSupportsCoSimulation(fmu2) && !forceModelExchange) {
        return testFMI2CS(fmu2);
    }
    else if(fmi2GetSupportsModelExchange){
        return testFMI2ME(fmu2);
    }
    else {
        printf("Failed to simulate FMU: Requested type not supported.\n");
        return 1;
    }
}
