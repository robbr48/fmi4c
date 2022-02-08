#define MODEL_IDENTIFIER fmi2

#include "fmi3Functions.h"
#include <stdbool.h>
#include <stdio.h>

#define VR_DX 1
#define VR_X 2
#define UNUSED(x)(void)(x)

typedef struct {
    fmi3String instanceName;
    fmi3String instantiationToken;
    fmi3InstanceEnvironment fmi3InstanceEnvironment;
    fmi3CallbackLogMessage logger;
    fmi3CallbackIntermediateUpdate intermediateUpdate;
    bool loggingOn;
    fmi3Float64 dx; //Input
    fmi3Float64 dxold; //Delayed variable(internal)
    fmi3Float64 x; //Output(integrated value)
    fmi3Float64 xold; //Delayed variable(internal)
} fmuContext;



const char* fmi3GetVersion(void) {
    return fmi3Version;
}


fmi3Status fmi3SetDebugLogging(fmi3Instance instance,
                               fmi3Boolean loggingOn,
                               size_t nCategories,
                               const fmi3String categories[])
{
    UNUSED(nCategories);
    UNUSED(categories);
    fmuContext *fmu =(fmuContext*)instance;
    fmu->loggingOn = loggingOn;
}

fmi3Instance fmi3InstantiateModelExchange(fmi3String instanceName,
                                          fmi3String instantiationToken,
                                          fmi3String resourcePath,
                                          fmi3Boolean visible,
                                          fmi3Boolean loggingOn,
                                          fmi3InstanceEnvironment instanceEnvironment,
                                          fmi3CallbackLogMessage logMessage)
{
    UNUSED(resourcePath);
    UNUSED(visible);

    fmuContext *fmu = malloc(sizeof(fmuContext));
    fmu->instanceName = instanceName;
    fmu->instantiationToken = instantiationToken;
    fmu->fmi3InstanceEnvironment = instanceEnvironment;
    fmu->logger = logMessage;
    fmu->loggingOn = loggingOn;

    return fmu;
}

fmi3Instance fmi3InstantiateCoSimulation(fmi3String instanceName,
                                         fmi3String instantiationToken,
                                         fmi3String resourcePath,
                                         fmi3Boolean visible,
                                         fmi3Boolean loggingOn,
                                         fmi3Boolean eventModeUsed,
                                         fmi3Boolean earlyReturnAllowed,
                                         const fmi3ValueReference requiredIntermediateVariables[],
                                         size_t nRequiredIntermediateVariables,
                                         fmi3InstanceEnvironment instanceEnvironment,
                                         fmi3CallbackLogMessage logMessage,
                                         fmi3CallbackIntermediateUpdate intermediateUpdate)
{
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(eventModeUsed);
    UNUSED(earlyReturnAllowed);
    UNUSED(requiredIntermediateVariables);
    UNUSED(nRequiredIntermediateVariables);

    fmuContext *fmu = malloc(sizeof(fmuContext));
    fmu->instanceName = instanceName;
    fmu->instantiationToken = instantiationToken;
    fmu->fmi3InstanceEnvironment = instanceEnvironment;
    fmu->logger = logMessage;
    fmu->intermediateUpdate = intermediateUpdate;
    fmu->loggingOn = loggingOn;

    if(fmu->loggingOn) {
        fmu->logger(fmu->fmi3InstanceEnvironment, fmu->instanceName, fmi3OK, "info", "Successfully instantiated FMU");
    }

    return fmu;
}

void fmi3FreeInstance(fmi3Instance instance)
{
    fmuContext *fmu = (fmuContext*)instance;
    free(fmu);
}

fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
                                       fmi3Boolean toleranceDefined,
                                       fmi3Float64 tolerance,
                                       fmi3Float64 startTime,
                                       fmi3Boolean stopTimeDefined,
                                       fmi3Float64 stopTime)
{
    UNUSED(toleranceDefined);
    UNUSED(tolerance);
    UNUSED(startTime);
    UNUSED(stopTimeDefined);
    UNUSED(stopTime);
    fmuContext *fmu = (fmuContext*)instance;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmi3OK;
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance,
                              fmi3Boolean stepEvent,
                              fmi3Boolean stateEvent,
                              const fmi3Int32 rootsFound[],
                              size_t nEventIndicators,
                              fmi3Boolean timeEvent)
{
    UNUSED(instance);
    UNUSED(stepEvent);
    UNUSED(stateEvent);
    UNUSED(rootsFound);
    UNUSED(nEventIndicators);
    UNUSED(timeEvent);
    return fmi3OK;  //Nothing to do
}


fmi3Status fmi3Terminate(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3OK; //Nothing to do
}

fmi3Status fmi3Reset(fmi3Instance instance) {
    fmuContext *fmu = (fmuContext*)instance;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmi3OK;
}

fmi3Status fmi3GetFloat64(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Float64 values[],
                          size_t nValues) {
    UNUSED(nValues);
    fmuContext *fmu =(fmuContext*)instance;
    fmi3Status status = fmi3OK;
    for(size_t i=0; i<nValueReferences; ++i) {
        switch(valueReferences[i]) {
        case VR_X:
            values[i] = fmu->x;
            break;
        case VR_DX:
            values[i] = fmu->dx;
            break;
        default:
            status = fmi3Warning;  // Non-existing value reference;
        }
    }

    return status;
}

fmi3Status fmi3SetFloat64(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Float64 values[],
                          size_t nValues)
{
    UNUSED(nValues);
    fmuContext *fmu =(fmuContext*)instance;
    fmi3Status status = fmi3OK;
    for(size_t i=0; i<nValueReferences; ++i) {
        switch(valueReferences[i]) {
        case VR_X:
            fmu->x = values[i];
            break;
        case VR_DX:
            fmu->dx = values[i];
            break;
        default:
            status = fmi3Warning;  // Non-existing value reference;
        }
    }

    return status;
}


fmi3Status fmi3SetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Clock values[],
                        size_t nValues)
{
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;  //This function should not be called
}

//Model exchange
fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance)
{
    UNUSED(instance);
    return fmi3OK;
}

fmi3Status fmi3CompletedIntegratorStep(fmi3Instance instance,
                                       fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                       fmi3Boolean* enterEventMode,
                                       fmi3Boolean* terminateSimulation)
{
    UNUSED(instance);
    UNUSED(noSetFMUStatePriorToCurrentPoint);
    UNUSED(enterEventMode);
    UNUSED(terminateSimulation);
    return fmi3OK;
}

fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 time)
{
    UNUSED(instance);
    UNUSED(time);
    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3SetContinuousStates(fmi3Instance instance,
                                   const fmi3Float64 continuousStates[],
                                   size_t nContinuousStates)
{
    fmuContext *fmu = (fmuContext *)instance;
    if(nContinuousStates > 0) {
        fmu->x = continuousStates[0];
        return fmi3OK;
    }
    return fmi3Warning; //Too few states were given
}
fmi3Status fmi3GetContinuousStateDerivatives(fmi3Instance instance,
                                             fmi3Float64 derivatives[],
                                             size_t nContinuousStates)
{
    fmuContext *fmu = (fmuContext *)instance;
    if(nContinuousStates > 0) {
        derivatives[0] = fmu->dx;
        return fmi3OK;
    }
    return fmi3Warning; //Asked for derivatives with too short array
}

fmi3Status fmi3GetEventIndicators(fmi3Instance instance,
                                  fmi3Float64 eventIndicators[],
                                  size_t nEventIndicators)
{
    UNUSED(instance);
    UNUSED(eventIndicators);
    UNUSED(nEventIndicators);
    return fmi3OK;   //Nothing to do
}

fmi3Status fmi3GetContinuousStates(fmi3Instance instance,
                                   fmi3Float64 continuousStates[],
                                   size_t nContinuousStates)
{
    fmuContext *fmu = (fmuContext *)instance;
    if(nContinuousStates > 0) {
        continuousStates[0] = fmu->x;
        return fmi3OK;
    }
    return fmi3Warning; //Asked for states with too short array
}

fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Instance instance,
                                             fmi3Float64 nominals[],
                                             size_t nContinuousStates)
{
    UNUSED(instance);
    UNUSED(nominals);
    UNUSED(nContinuousStates);
    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3GetNumberOfEventIndicators(fmi3Instance instance,
                                          size_t* nEventIndicators)
{
    UNUSED(instance);
    *nEventIndicators = 0;
    return fmi3OK;
}

fmi3Status fmi3GetNumberOfContinuousStates(fmi3Instance instance,
                                           size_t* nContinuousStates)
{
    UNUSED(instance);
    *nContinuousStates = 1;
    return fmi3OK;
}


//Co-simulation
fmi3Status fmi3EnterStepMode(fmi3Instance instance)
{
    UNUSED(instance);
    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3DoStep(fmi3Instance instance,
                      fmi3Float64 currentCommunicationPoint,
                      fmi3Float64 communicationStepSize,
                      fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                      fmi3Boolean* eventEncountered,
                      fmi3Boolean* terminateSimulation,
                      fmi3Boolean* earlyReturn,
                      fmi3Float64* lastSuccessfulTime)
{
    UNUSED(currentCommunicationPoint);
    UNUSED(noSetFMUStatePriorToCurrentPoint);
    UNUSED(eventEncountered);
    UNUSED(terminateSimulation);
    UNUSED(earlyReturn);
    UNUSED(lastSuccessfulTime);

    fmuContext *fmu =(fmuContext *)instance;

    //Integrate(trapezoid rule)
    fmu->x = fmu->xold + communicationStepSize/2.0*(fmu->dx + fmu->dxold);
    fmu->xold = fmu->x;
    fmu->dxold = fmu->dx;

    return fmi3OK;
}

fmi3Status fmi3UpdateDiscreteStates(fmi3Instance instance,
                                    fmi3Boolean* needsDiscreteStatesUpdate,
                                    fmi3Boolean* terminateSimulation,
                                    fmi3Boolean* nominalsOfContinuousStatesChanged,
                                    fmi3Boolean* valuesOfContinuousStatesChanged,
                                    fmi3Boolean* nextEventTimeDefined,
                                    fmi3Float64* nextEventTime)
{
    UNUSED(instance);
    UNUSED(needsDiscreteStatesUpdate);
    UNUSED(terminateSimulation);
    UNUSED(nominalsOfContinuousStatesChanged);
    UNUSED(valuesOfContinuousStatesChanged);
    UNUSED(nextEventTimeDefined);
    UNUSED(nextEventTime);
    return fmi3OK;
}
