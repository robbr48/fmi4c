#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "fmic.h"

#define UNUSED(x) (void)(x);

#define VAR_MAX 1024

#define TABLE_SIZE 1500
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    const char *name;
    double data[1024];
} namedData;

static int numOutputs = 0;
static unsigned int outputRefs[VAR_MAX];

//Default experiment settings (may be overwritten by default experiment in FMU)
static double startTime = 0;
static double stepSize = 0.001;
static double stopTime = 1;
static double tolerance = 0;

static const char* outputCsvPath = "\0";
static size_t nInterpolators = 0;
static namedData interpolationData[20];
static size_t dataSize = 0;

typedef struct {
    fmi3Handle *fmu1;
    double table1[2][TABLE_SIZE];
    size_t table1start;
    size_t table1end;

    fmi3Handle *fmu2;
    double table2[2][TABLE_SIZE];
    size_t table2start;
    size_t table2end;

    double Zc;
    double dt;
} tlmContext;

static tlmContext tlm;

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
    UNUSED(instanceEnvironment);
    UNUSED(clocksTicked);
    UNUSED(intermediateVariableSetRequested);
    UNUSED(intermediateVariableGetAllowed);
    UNUSED(intermediateStepFinished);
    UNUSED(canReturnEarly);
    UNUSED(earlyReturnRequested);
    UNUSED(earlyReturnTime);
}

double getInterpolatedForce1(double time)
{
    while(tlm.table1[0][tlm.table1end] < time) {
        Sleep(1);
    }

    pthread_mutex_lock(&mutex1);

    int i = tlm.table1end;

    //find first element where time is not too small
    while(tlm.table1[0][i] > time) {
        --i;
        if(i < 0) {
            i = TABLE_SIZE-1;
        }
    }
    int j = i+1;

    double t1 = tlm.table1[0][j];
    double t2 = tlm.table1[0][i];
    double x1 = tlm.table1[1][j];
    double x2 = tlm.table1[1][i];

    double force = x1 + ((x2-x1)/(t2-t1)) * (time - t1);
    pthread_mutex_unlock(&mutex1);
    return force;
}


double getInterpolatedForce2(double time)
{
    while(tlm.table2[0][tlm.table2end] < time) {
        Sleep(1);
    }

    pthread_mutex_lock(&mutex2);

    int i = tlm.table2end;

    //find first element where time is not too small
    while(tlm.table2[0][i] > time) {
        --i;
        if(i < 0) {
            i = TABLE_SIZE-1;
        }
    }
    int j = i+1;

    double t1 = tlm.table2[0][j];
    double t2 = tlm.table2[0][i];
    double x1 = tlm.table2[1][j];
    double x2 = tlm.table2[1][i];

    double force = x1 + ((x2-x1)/(t2-t1)) * (time - t1);
    pthread_mutex_unlock(&mutex2);
    return force;
}

void intermediateUpdateTLM(
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
    UNUSED(canReturnEarly);
    UNUSED(earlyReturnRequested);
    UNUSED(earlyReturnTime);

    fmi3ValueReference vr_v = 0;    //Connection velocity (output)
    fmi3ValueReference vr_f = 1;    //Connection force (input)

    fmi3Handle *fmu = (fmi3Handle*)instanceEnvironment;
    if(intermediateVariableGetAllowed) {
        double v;
        fmi3GetFloat64(fmu, &vr_v, 1, &v, 1);
        if(intermediateStepFinished) {
            if(fmu == tlm.fmu1) {
                ++tlm.table2end;
                while(tlm.table2[0][tlm.table2start] < intermediateUpdateTime - 2*tlm.dt) {
                    ++tlm.table2start;
                }
                if(tlm.table2end > TABLE_SIZE-1) {    //Prevent overflow (circular buffer)
                    tlm.table2end = 0;
                }
                if(tlm.table2start > TABLE_SIZE-1) {
                    tlm.table2start = 0;
                }
                tlm.table2[0][tlm.table2end] = intermediateUpdateTime;
                double c = getInterpolatedForce1(intermediateUpdateTime - tlm.dt);
                tlm.table2[1][tlm.table2end] = 2*tlm.Zc*v + c;
            }
            if(fmu == tlm.fmu2) { //!@todo Costly to compare strings
                ++tlm.table1end;
                while(tlm.table1[0][tlm.table1start] < intermediateUpdateTime - 2*tlm.dt) {
                    ++tlm.table1start;
                }
                if(tlm.table1end > TABLE_SIZE-1) {    //Prevent overflow (circular buffer)
                    tlm.table1end = 0;
                }
                if(tlm.table1start > TABLE_SIZE-1) {
                    tlm.table1start = 0;
                }
                tlm.table1[0][tlm.table1end] = intermediateUpdateTime;
                double c = getInterpolatedForce2(intermediateUpdateTime - tlm.dt);
                tlm.table1[1][tlm.table1end] = 2*tlm.Zc*v + c;
            }
        }
    }
    if(intermediateVariableGetAllowed && intermediateVariableSetRequested) {
        double c;
        if(fmu == tlm.fmu1) {
            c = getInterpolatedForce1(intermediateUpdateTime-tlm.dt);
        }
        else {
            c = getInterpolatedForce2(intermediateUpdateTime-tlm.dt);
        }
        double v;
        fmi3GetFloat64(fmu, &vr_v, 1, &v, 1);

        double F = v*tlm.Zc + c;
        fmi3SetFloat64(fmu, &vr_f, 1, &F, 1);
    }
}

int testFMI1ME(fmi1Handle *fmu1) {
    //Instantiate FMU
    if(!fmi1InstantiateModel(fmu1, loggerFmi1, calloc, free, NULL, fmi1True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

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
            printf("Setting real (%i): %f\n", vr, value);
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
            printf("Reading real  (%i): %f\n", outputRefs[i], value);
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
            printf("Output varaible: %s\n", name);
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


int testFMI2ME(fmi2Handle *fmu2)
{
    //Instantiate FMU
    if(!fmi2Instantiate(fmu2, fmi2ModelExchange, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

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
    printf("FMU successfully initialized!\n");

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

    fmi2Terminate(fmu2);
    fmi2FreeInstance(fmu2);
}


int testFMI2CS(fmi2Handle *fmu2)
{
    //Instantiate FMU
    if(!fmi2Instantiate(fmu2, fmi2CoSimulation, loggerFmi2, calloc, free, NULL, NULL, fmi2False, fmi2True)) {
        printf("fmi2Instantiate() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

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
    printf("FMU successfully initialized!\n");

    printf("Simulating from %f to %f...\n",startTime, stopTime);
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

    fmi2Terminate(fmu2);
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
            printf("Output varaible: %s\n", name);
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


int testFMI3CS(fmi3Handle *fmu3)
{
    fmi3Status status;

    int nRequiredIntermediateVariables = 0;
    if(!fmi3InstantiateCoSimulation(fmu3, fmi3False, fmi3True, fmi3False, fmi3False, NULL, nRequiredIntermediateVariables, NULL, loggerFmi3, intermediateUpdate)) {
        printf("fmi3InstantiateCoSimulation() failed\n");
        exit(1);
    }
    printf("FMU successfully instantiated!\n");

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

    printf("Simulating from %f to %f...\n",startTime, stopTime);
    FILE *outputFile = fopen(outputCsvPath, "w");
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

    FILE *outputFile = fopen(outputCsvPath, "w");
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
            printf("Output varaible: %s\n", name);
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


//Argument struct for doStep thread
struct doStepArgs {
    fmi3Handle *fmu;
    fmi3Float64 tcur;
    fmi3Float64 tstep;
};


//doStep thread
void* doStepInThread(void* argsptr)
{
    struct doStepArgs *args = (struct doStepArgs*)argsptr;
    bool eventEncountered, terminateSimulation, earlyReturn;
    double lastT;
    fmi3DoStep(args->fmu, args->tcur, args->tstep, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastT);
    return NULL;
}


int testFMI3TLM(fmiHandle *fmua, fmiHandle *fmub)
{
    fmi3Handle *fmu3a = loadFmu3(fmua);
    if(fmu3a == NULL) {
        printf("Failed to load first FMU file\n");
        exit(1);
    }
    fmi3Handle *fmu3b = loadFmu3(fmub);
    if(fmu3b == NULL) {
        printf("Failed to load second FMU file\n");
        exit(1);
    }

    //TLM coupling parameters
    fmi3Float64 mtlm = 1;       //Distributed inertia [kg]
    fmi3Float64 ktlm = 2e5;     //Distributed stiffness [N/m]

    //Simulation parameters
    fmi3Float64 tcur = 0;       //Current simulation time
    fmi3Float64 tstop = 2;      //Simulation stop time
    fmi3Float64 tstep = 0.001;  //Simulation time step

    //Model parameters
    fmi3Float64 fd = 100;       //Disturbance force

    //Compute TLM parameters
    tlm.Zc = sqrt(mtlm*ktlm);       //Resulting characteristic impedance (global variable)
    tlm.dt = sqrt(mtlm/ktlm);       //Resulting time delay (global variable)
    printf("dt = %f\n",tlm.dt);

    //Value references
    fmi3ValueReference vr_v = 0;    //Connection velocity (output)
    fmi3ValueReference vr_f = 1;    //Connection force (input)
    fmi3ValueReference vr_fd = 5;   //Disturbance force (input to one of the FMUs)

    //Initialize interpolation tables (zero force)
    tlm.fmu1 = fmu3a;
    tlm.table1start = 0;
    tlm.table1end = 1;
    tlm.table1[0][tlm.table1start] = -10000;
    tlm.table1[1][tlm.table1start] = 0;
    tlm.table1[0][tlm.table1end] = 0;
    tlm.table1[1][tlm.table1end] = 0;

    tlm.fmu2 = fmu3b;
    tlm.table2start = 0;
    tlm.table2end = 1;
    tlm.table2[0][tlm.table2start] = -10000;
    tlm.table2[1][tlm.table2start] = 0;
    tlm.table2[0][tlm.table2end] = 0;
    tlm.table2[1][tlm.table2end] = 0;

    //Instantiate
    fmi3ValueReference requiredIntermediateVariables[2] = {0, 1};
    fmi3ValueReference nRequiredIntermediateVaraibles = 2;
    fmi3InstantiateCoSimulation(fmu3a, fmi3False, fmi3True, fmi3False, fmi3False, requiredIntermediateVariables, nRequiredIntermediateVaraibles, fmu3a, loggerFmi3, intermediateUpdateTLM);
    fmi3InstantiateCoSimulation(fmu3b, fmi3False, fmi3True, fmi3False, fmi3False, requiredIntermediateVariables, nRequiredIntermediateVaraibles, fmu3b, loggerFmi3, intermediateUpdateTLM);

    //Enter initialization
    fmi3EnterInitializationMode(fmu3a, fmi3False, 0, tcur, fmi3True, tstop);
    fmi3EnterInitializationMode(fmu3b, fmi3False, 0, tcur, fmi3True, tstop);

    //Exit initialization
    fmi3ExitInitializationMode(fmu3a);
    fmi3ExitInitializationMode(fmu3b);

    //Simlation loop
    double f = 0;
    fmi3SetFloat64(fmu3a, &vr_f, 1, &f, 1);    //Initialize TLM force to zero
    fmi3SetFloat64(fmu3b, &vr_f, 1, &f, 1);

    fmi3Float64 m1 = 5;         //Mass 1
    fmi3Float64 c1 = 50;        //Damping 1
    fmi3Float64 k1 = 1000;      //Spring stiffness 1
    fmi3Float64 m2 = 7;         //Mass 2
    fmi3Float64 c2 = 60;        //Damping 2
    fmi3Float64 k2 = 600;       //Spring stiffness 2

    fmi3ValueReference vr_m = 2;
    fmi3ValueReference vr_c = 3;
    fmi3ValueReference vr_k = 4;

    fmi3SetFloat64(fmu3a, &vr_m, 1, &m1, 1);
    fmi3SetFloat64(fmu3a, &vr_c, 1, &c1, 1);
    fmi3SetFloat64(fmu3a, &vr_k, 1, &k1, 1);
    fmi3SetFloat64(fmu3b, &vr_m, 1, &m2, 1);
    fmi3SetFloat64(fmu3b, &vr_c, 1, &c2, 1);
    fmi3SetFloat64(fmu3b, &vr_k, 1, &k2, 1);

    FILE *pResultFile;
    fclose(fopen(outputCsvPath, "w"));          //Clear old results in file
    pResultFile = fopen(outputCsvPath, "a");    //Open result file for appending
    fprintf(pResultFile, "t,v1,v2,f1,f2\n");          //Print header row

    struct doStepArgs stepArgs1;
    stepArgs1.tstep = tstep;
    stepArgs1.fmu = fmu3a;

    struct doStepArgs stepArgs2;
    stepArgs2.tstep = tstep;
    stepArgs2.fmu = fmu3b;

    pthread_t thread1, thread2;

    printf("Starting simulation...\n");

    while(tcur < tstop) {
        if(tcur > 0.1/* && tcur < 0.1+tstep*/) {
            fmi3SetFloat64(fmu3b, &vr_fd, 1, &fd, 1); //Apply disturbance force
        }

        //Create threads
        stepArgs1.tcur = tcur;
        pthread_create(&thread1, NULL, doStepInThread, &stepArgs1);

        stepArgs2.tcur = tcur;
        pthread_create(&thread2, NULL, doStepInThread, &stepArgs2);

        // wait for them to finish
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

        double v1, v2, f1, f2;
        fmi3GetFloat64(fmu3a, &vr_v, 1, &v1, 1);
        fmi3GetFloat64(fmu3b, &vr_v, 1, &v2, 1);
        fmi3GetFloat64(fmu3a, &vr_f, 1, &f1, 1);
        fmi3GetFloat64(fmu3b, &vr_f, 1, &f2, 1);
        fprintf(pResultFile, "%f,%f,%f,%f,%f\n",tcur,v1,v2,f1,f2);
        tcur += tstep;

        printf("\rSimulating, %.0f%% done...",tcur/tstop*100);
        fflush(stdout);
    }

    fclose(pResultFile);

    fmi3Terminate(fmu3a);
    fmi3Terminate(fmu3b);
    fmi3FreeInstance(fmu3a);
    fmi3FreeInstance(fmu3b);

    printf("TLM test finished!\n");

    return 0;
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

    printf("FMU path: %s\n", fmuPath);
    if(inputCsvPath != "\0") {
        printf("Input CSV path: %s\n", inputCsvPath);
    }
    printf("Output CSV path: %s\n", outputCsvPath);

    fmiHandle *fmu = unzipFmu(fmuPath, "testfmu");

    if(fmu == NULL) {
        printf("Failed to load FMU\n");
        exit(1);
    }
    printf("Successfully loaded FMU\n");

    fmiVersion_t version = getFmiVersion(fmu);
    if(version == fmiVersion1) {
        printf("FMI is of version 1\n");
    }
    else if(version == fmiVersion2) {
        printf("FMI is of version 2\n");
    }
    else if(version == fmiVersion3) {
        printf("FMI is of version 3\n");
    }
    else {
        printf("Unknown FMI version.\n");
        exit(1);
    }

    char *cwd = (char*)malloc(sizeof(char)*FILENAME_MAX);
    _getcwd(cwd, sizeof(char)*FILENAME_MAX);
    printf("CWD (katt): %s\n",cwd);

    FILE *inputFile = fopen(inputCsvPath, "r");
    char lineStr[1024];
    size_t lineNumber = 0;
    while(fgets(lineStr , sizeof(lineStr) , inputFile) != NULL) {
        lineStr[strcspn(lineStr, "\r\n")] = 0;
        char* word = strtok(lineStr, ",");
        int i=0;
        while (word != NULL && word != "") {
            if(lineNumber == 0) {
                namedData data;
                data.name = strdup(word);
                interpolationData[nInterpolators] = data;
                ++nInterpolators;
            }
            else {
                interpolationData[i].data[lineNumber-1] = atof(word);
            }
            word = strtok(NULL, ",");

            ++i;
        }
        lineNumber++;
    }
    fclose(inputFile);
    dataSize = lineNumber-1;

    if(testTLM) {
        if(version != fmiVersion3) {
            printf("Can only test TLM with FMI 3. First FMU is of wrong version.\n");
            return 1;
        }

        //Load second FMU
        fmiHandle *fmu2 = unzipFmu(fmuPath2, "testfmu2");

        if(fmu2 == NULL) {
            printf("Failed to load second FMU\n");
            exit(1);
        }
        printf("Successfully loaded second FMU\n");

        fmiVersion_t version2 = getFmiVersion(fmu2);
        if(version2 != fmiVersion3) {
            printf("Can only test TLM with FMI 3. Second FMU is of wrong version.\n");
            return 1;
        }

        return testFMI3TLM(fmu, fmu2);
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


