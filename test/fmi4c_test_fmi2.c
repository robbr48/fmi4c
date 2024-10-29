#include <stdarg.h>
#include <string.h>

#include "fmi4c.h"
#include "fmi4c_common.h"
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
    UNUSED(category)


    if(status == fmi2OK && logLevel < 4 ||
        status == fmi2Pending && logLevel < 4 ||
        status == fmi2Warning && logLevel < 3 ||
        status == fmi2Discard && logLevel < 3 ||
        status == fmi2Error && logLevel < 2 ||
        status == fmi2Fatal && logLevel < 1) {
        return;
    }

    va_list args;
    va_start(args, message);
    char msgstr[1024];
    sprintf(msgstr, "%s: %s\n", category, message);
    printf(msgstr, args);
    va_end(args);
}


int testFMI2ME(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Instantiate FMU
    if(!fmi2_instantiate(fmu, fmi2ModelExchange, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("--- Test simulation ---\n");
    printf("  FMU successfully instantiated.\n");


    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi2_defaultStartTimeDefined(fmu)) {
        startTime = fmi2_getDefaultStartTime(fmu);
    }
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    else if(fmi2_defaultStepSizeDefined(fmu)) {
        stepSize = fmi2_getDefaultStepSize(fmu);
    }
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }
    else if(fmi2_defaultStopTimeDefined(fmu)) {
        stopTime = fmi2_getDefaultStopTime(fmu);
    }

    fmi2Status status;

    //Setup experiment
    status = fmi2_setupExperiment(fmu, fmi2False, 0, startTime, fmi2False, 0.0);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Enter initialization mode
    status = fmi2_enterInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi2_exitInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    double actualStepSize;
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

    nStates = fmi2_getNumberOfContinuousStates(fmu);
    nEventIndicators = fmi2_getNumberOfEventIndicators(fmu);

    states = malloc(nStates*sizeof(double));
    derivatives = malloc(nStates*sizeof(double));
    eventIndicators = malloc(nEventIndicators*sizeof(double));
    eventIndicatorsPrev = malloc(nEventIndicators*sizeof(double));

    //Perform event iteration
    eventInfo.newDiscreteStatesNeeded = fmi2True;
    eventInfo.terminateSimulation     = fmi2False;
    while (eventInfo.newDiscreteStatesNeeded && !eventInfo.terminateSimulation) {
        fmi2_newDiscreteStates(fmu, &eventInfo);
    }
    fmi2_enterContinuousTimeMode(fmu);

    status = fmi2_getContinuousStates(fmu, states, nStates);
    if(status != fmi2OK) {
        printf("fmi2_getContinuousStates() failed\n");
        exit(1);
    }
    status = fmi2_getEventIndicators(fmu, eventIndicators, nEventIndicators);
    if(status != fmi2OK) {
        printf("fmi2_getEventIndicators() failed\n");
        exit(1);
    }

    outputFile = fopen(outputCsvPath, "w");
    if(outputFile != NULL) {
        fprintf(outputFile,"time");
        for(int i=0; i<numOutputs; ++i) {
            fprintf(outputFile,",%s",fmi2_getVariableName(fmi2_getVariableByValueReference(fmu, outputRefs[i])));
        }
        fprintf(outputFile,"\n");
    }

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    for(double time=startTime; time < stopTime;) {
        if(eventInfo.terminateSimulation || terminateSimulation) {
            printf("Terminating simulation at time = %f\n", time);
            break;
        }

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi2VariableHandle *var = fmi2_getVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                free(derivatives);
                free(eventIndicatorsPrev);
                free(states);
                free(eventIndicators);
                return 1;
            }
            fmi2Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi2ValueReference vr = fmi2_getVariableValueReference(var);
            fmi2_setReal(fmu, &vr, 1, &value);
        }

        size_t k;
        fmi2Real lastTime;
        int zeroCrossingEvent = 0;

        status = fmi2_setTime(fmu, time);
        if(status != fmi2OK) {
            printf("fmi2_setTime() failed\n");
            exit(1);
        }

        memcpy(eventIndicatorsPrev, eventIndicators, nEventIndicators*sizeof(double));
        status = fmi2_getEventIndicators(fmu, eventIndicators, nEventIndicators);
        if(status != fmi2OK) {
            printf("fmi2_getEventIndicators() failed\n");
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
            status = fmi2_enterEventMode(fmu);
            if(status != fmi2OK) {
                printf("fmi2EnterEventMode() failed\n");
                exit(1);
            }

            //Perform event iteration
            eventInfo.newDiscreteStatesNeeded = fmi2True;
            eventInfo.terminateSimulation     = fmi2False;
            while (eventInfo.newDiscreteStatesNeeded && !eventInfo.terminateSimulation) {
                fmi2_newDiscreteStates(fmu, &eventInfo);
            }
            status = fmi2_enterContinuousTimeMode(fmu);
            if(status != fmi2OK) {
                printf("fmi2EnterContinuousTimeMode() failed\n");
                exit(1);
            }
            status = fmi2_getContinuousStates(fmu, states, nStates);
            if(status != fmi2OK) {
                printf("fmi2_getContinuousStates() failed\n");
                exit(1);
            }
            status = fmi2_getEventIndicators(fmu, eventIndicators, nEventIndicators);
            if(status != fmi2OK) {
                printf("fmi2_getEventIndicators() failed\n");
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
        status = fmi2_getDerivatives(fmu, derivatives, nStates);
        if(status != fmi2OK) {
            printf("fmi2_getDerivatives() failed\n");
            exit(1);
        }
        for (k = 0; k < nStates; k++) {
            states[k] = states[k] + actualStepSize*derivatives[k];
        }

        status = fmi2_setContinuousStates(fmu, states, nStates);
        if(status != fmi2OK) {
            printf("fmi2_setContinuousStates() failed\n");
            exit(1);
        }
        status = fmi2_completedIntegratorStep(fmu, fmi2True, &callEventUpdate, &terminateSimulation);
        if(status != fmi2OK) {
            printf("fmi2CompletedIntegratorStep() failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        if(outputFile != NULL) {
            fprintf(outputFile,"%f",time);
            for(int i=0; i<numOutputs; ++i) {
                fmi2_getReal(fmu, &outputRefs[i], 1, &value);
                fprintf(outputFile,",%f",value);
            }
            fprintf(outputFile,"\n");
        }
    }
    free(derivatives);
    free(eventIndicatorsPrev);
    free(states);
    free(eventIndicators);
    if(outputFile != NULL) {
        fclose(outputFile);
    }

    printf("  Simulation finished.\n");

    fmi2_terminate(fmu);
    printf("  FMU successfully terminated.\n");

    fmi2_freeInstance(fmu);

    return 0;
}


int testFMI2CS(fmiHandle *fmu, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Instantiate FMU
    if(!fmi2_instantiate(fmu, fmi2CoSimulation, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("--- Test simulation ---\n");
    printf("  FMU successfully instantiated.\n");

    double startTime = 0;
    double stepSize = 0.001;
    double stopTime = 1;

    if(fmi2_defaultStartTimeDefined(fmu)) {
        startTime = fmi2_getDefaultStartTime(fmu);
    }
    if(overrideTimeStep) {
        stepSize = timeStepOverride;
    }
    else if(fmi2_defaultStepSizeDefined(fmu)) {
        stepSize = fmi2_getDefaultStepSize(fmu);
    }
    if(overrideStopTime) {
        stopTime = stopTimeOverride;
    }
    else if(fmi2_defaultStopTimeDefined(fmu)) {
        stopTime = fmi2_getDefaultStopTime(fmu);
    }

    fmi2Status status;

    //Setup experiment
    status = fmi2_setupExperiment(fmu, fmi2False, 0, startTime, fmi2False, 0.0);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Enter initialization mode
    status = fmi2_enterInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi2EnterInitializationMode() failed\n");
        exit(1);
    }

    //Exit initialization mode
    status = fmi2_exitInitializationMode(fmu);
    if(status != fmi2OK) {
        printf("fmi3ExitInitializationMode() failed\n");
        exit(1);
    }
    printf("  FMU successfully initialized.\n");

    printf("  Simulating from %f to %f...\n",startTime, stopTime);
    if(outputFile != NULL) {
        outputFile = fopen(outputCsvPath, "w");
        fprintf(outputFile,"time");
        for(int i=0; i<numOutputs; ++i) {
            fprintf(outputFile,",%s",fmi2_getVariableName(fmi2_getVariableByValueReference(fmu, outputRefs[i])));
        }
        fprintf(outputFile,"\n");
    }

    double time=startTime;
    while(time <= stopTime) {

        //Interpolate inputs from CSV file
        for(int i=1; i<nInterpolators; ++i) {
            fmi2VariableHandle *var = fmi2_getVariableByName(fmu, interpolationData[i].name);
            if(var == NULL) {
                printf("Variable in input file does not exist in FMU: %s\n", interpolationData[i].name);
                return 1;
            }
            fmi2Real value = interpolate(&interpolationData[0], &interpolationData[i], time, dataSize);
            fmi2ValueReference vr = fmi2_getVariableValueReference(var);
            fmi2_setReal(fmu, &vr, 1, &value);
        }

        //Take a step
        status = fmi2_doStep(fmu, time, stepSize, fmi2True);
        if(status != fmi2OK) {
            printf("fmi2DoStep failed\n");
            exit(1);
        }

        //Print all output variables to CSV file
        double value;
        if(outputFile != NULL) {
            fprintf(outputFile,"%f",time);
            for(int i=0; i<numOutputs; ++i) {
                fmi2_getReal(fmu, &outputRefs[i], 1, &value);
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

    fmi2_terminate(fmu);
    printf("  FMU successfully terminated.\n");
    fmi2_freeInstance(fmu);
    return 0;
}


int testFMI2(fmiHandle *fmu, bool forceModelExchange, bool forceCosimulation, bool overrideStopTime, double stopTimeOverride, bool overrideTimeStep, double timeStepOverride)
{
    //Loop through variables in FMU
    for(size_t i=0; i<fmi2_getNumberOfVariables(fmu); ++i)
    {
        fmi2VariableHandle* var = fmi2_getVariableByIndex(fmu, i);
        fmi2Causality causality = fmi2_getVariableCausality(var);
        unsigned int vr = fmi2_getVariableValueReference(var);

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

    if(forceModelExchange && !fmi2_getSupportsModelExchange(fmu)) {
        printf("Model exchange mode not supported by FMU. Aborting.");
        exit(1);
    }
    else if(forceCosimulation && !fmi2_getSupportsCoSimulation(fmu)) {
        printf("Co-simulation mode not supported by FMU. Aborting.");
        exit(1);
    }

    if(fmi2_getSupportsCoSimulation(fmu) && !forceModelExchange) {
        return testFMI2CS(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else if(fmi2_getSupportsModelExchange(fmu)){
        return testFMI2ME(fmu, overrideStopTime, stopTimeOverride, overrideTimeStep, timeStepOverride);
    }
    else {
        printf("Failed to simulate FMU: Requested type not supported.\n");
        return 1;
    }
}
