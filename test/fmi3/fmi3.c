#define MODEL_IDENTIFIER fmi2

#include "fmi3Functions.h"
#include "fmi4c_common.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define VR_DX 1
#define VR_X 2

typedef struct {
    fmi3String instanceName;
    fmi3String instantiationToken;
    fmi3String resourcePath;
    fmi3InstanceEnvironment fmi3InstanceEnvironment;
    fmi3LogMessageCallback logger;
    fmi3IntermediateUpdateCallback intermediateUpdate;
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
    return fmi3OK;
}

fmi3Instance fmi3InstantiateModelExchange(fmi3String instanceName,
                                          fmi3String instantiationToken,
                                          fmi3String resourcePath,
                                          fmi3Boolean visible,
                                          fmi3Boolean loggingOn,
                                          fmi3InstanceEnvironment instanceEnvironment,
                                          fmi3LogMessageCallback logMessage)
{
    UNUSED(resourcePath);
    UNUSED(visible);

    fmuContext *fmu = calloc(1, sizeof(fmuContext));
    fmu->instanceName = _strdup(instanceName);
    fmu->instantiationToken = _strdup(instantiationToken);
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
                                         fmi3LogMessageCallback logMessage,
                                         fmi3IntermediateUpdateCallback intermediateUpdate)
{
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(eventModeUsed);
    UNUSED(earlyReturnAllowed);
    UNUSED(requiredIntermediateVariables);
    UNUSED(nRequiredIntermediateVariables);

    fmuContext *fmu = calloc(1, sizeof(fmuContext));

    fmu->instanceName = _strdup(instanceName);
    fmu->instantiationToken = _strdup(instantiationToken);
    fmu->resourcePath = resourcePath;
    fmu->fmi3InstanceEnvironment = instanceEnvironment;
    fmu->logger = logMessage;
    fmu->intermediateUpdate = intermediateUpdate;
    fmu->loggingOn = loggingOn;
    if(fmu->loggingOn) {
        fmu->logger(fmu->fmi3InstanceEnvironment, fmi3OK, "info", "Successfully instantiated FMU");
    }

    return fmu;
}

fmi3Instance fmi3InstantiateScheduledExecution(fmi3String instanceName,
                                               fmi3String instantiationToken,
                                               fmi3String resourcePath,
                                               fmi3Boolean visible,
                                               fmi3Boolean loggingOn,
                                               fmi3InstanceEnvironment instanceEnvironment,
                                               fmi3LogMessageCallback logMessage,
                                               fmi3ClockUpdateCallback intermediateUpdate,
                                               fmi3LockPreemptionCallback lockPreemption,
                                               fmi3UnlockPreemptionCallback unlockPreemption) {
    UNUSED(instanceName);
    UNUSED(instantiationToken);
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(loggingOn);
    UNUSED(instanceEnvironment);
    UNUSED(logMessage);
    UNUSED(intermediateUpdate);
    UNUSED(lockPreemption);
    UNUSED(unlockPreemption);
    return NULL;
}

void fmi3FreeInstance(fmi3Instance instance)
{
    fmuContext *fmu = (fmuContext*)instance;
    free((char*)fmu->instanceName);
    free((char*)fmu->instantiationToken);
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

fmi3Status fmi3EnterEventMode(fmi3Instance instance)
{
    UNUSED(instance);
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


fmi3Status fmi3GetFloat32(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Float32 values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[],
                       size_t nValueReferences,
                       fmi3Int8 values[],
                       size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3UInt8 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Int16 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt16(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3UInt16 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt32(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Int32 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt32(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3UInt32 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt64(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Int64 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt64(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3UInt64 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetBoolean(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Boolean values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetString(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3String values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetBinary(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         size_t valueSizes[],
                         fmi3Binary values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(valueSizes);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Clock values[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    return fmi3Warning;
}

fmi3Status fmi3SetFloat32(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Float32 values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[],
                       size_t nValueReferences,
                       const fmi3Int8 values[],
                       size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3UInt8 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Int16 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt16(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3UInt16 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt32(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Int32 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt32(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3UInt32 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt64(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Int64 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt64(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3UInt64 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetBoolean(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Boolean values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetString(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3String values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetBinary(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const size_t valueSizes[],
                         const fmi3Binary values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(valueSizes);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Clock values[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    return fmi3Warning;
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
    for(size_t i=0; i<nContinuousStates; ++i) {
        nominals[i] = 1.0;
    }
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

    *lastSuccessfulTime = currentCommunicationPoint+communicationStepSize;

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

fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Instance instance,
                                               fmi3ValueReference valueReference,
                                               size_t* nDependencies) {
    UNUSED(instance);
    UNUSED(valueReference);
    UNUSED(nDependencies);
    return fmi3Warning;
}

fmi3Status fmi3GetVariableDependencies(fmi3Instance instance,
                                       fmi3ValueReference dependent,
                                       size_t elementIndicesOfDependent[],
                                       fmi3ValueReference independents[],
                                       size_t elementIndicesOfIndependents[],
                                       fmi3DependencyKind dependencyKinds[],
                                       size_t nDependencies) {
    UNUSED(instance);
    UNUSED(dependent);
    UNUSED(elementIndicesOfDependent);
    UNUSED(independents);
    UNUSED(elementIndicesOfIndependents);
    UNUSED(dependencyKinds);
    UNUSED(nDependencies);
    return fmi3Warning;
}

fmi3Status fmi3GetFMUState(fmi3Instance instance,
                           fmi3FMUState* FMUState) {
    UNUSED(instance);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3SetFMUState(fmi3Instance instance,
                           fmi3FMUState FMUState) {
    UNUSED(instance);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3FreeFMUState(fmi3Instance instance,
                            fmi3FMUState* FMUState) {
    UNUSED(instance);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3SerializedFMUStateSize(fmi3Instance instance,
                                      fmi3FMUState FMUState,
                                      size_t* size) {
    UNUSED(instance);
    UNUSED(FMUState);
    UNUSED(size);
    return fmi3Warning;
}

fmi3Status fmi3SerializeFMUState(fmi3Instance instance,
                                 fmi3FMUState FMUState,
                                 fmi3Byte serializedState[],
                                 size_t size) {
    UNUSED(instance);
    UNUSED(FMUState);
    UNUSED(serializedState);
    UNUSED(size);
    return fmi3Warning;
}

fmi3Status fmi3DeserializeFMUState(fmi3Instance instance,
                                   const fmi3Byte serializedState[],
                                   size_t size,
                                   fmi3FMUState* FMUState) {
    UNUSED(instance);
    UNUSED(serializedState);
    UNUSED(size);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3GetDirectionalDerivative(fmi3Instance instance,
                                        const fmi3ValueReference unknowns[],
                                        size_t nUnknowns,
                                        const fmi3ValueReference knowns[],
                                        size_t nKnowns,
                                        const fmi3Float64 seed[],
                                        size_t nSeed,
                                        fmi3Float64 sensitivity[],
                                        size_t nSensitivity) {
    UNUSED(instance);
    UNUSED(unknowns);
    UNUSED(nUnknowns);
    UNUSED(knowns);
    UNUSED(nKnowns);
    UNUSED(seed);
    UNUSED(nSeed);
    UNUSED(sensitivity);
    UNUSED(nSensitivity);
    return fmi3Warning;
}

fmi3Status fmi3GetAdjointDerivative(fmi3Instance instance,
                                    const fmi3ValueReference unknowns[],
                                    size_t nUnknowns,
                                    const fmi3ValueReference knowns[],
                                    size_t nKnowns,
                                    const fmi3Float64 seed[],
                                    size_t nSeed,
                                    fmi3Float64 sensitivity[],
                                    size_t nSensitivity) {
    UNUSED(instance);
    UNUSED(unknowns);
    UNUSED(nUnknowns);
    UNUSED(knowns);
    UNUSED(nKnowns);
    UNUSED(seed);
    UNUSED(nSeed);
    UNUSED(sensitivity);
    UNUSED(nSensitivity);
    return fmi3Warning;
}

fmi3Status fmi3EnterConfigurationMode(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3Warning;
}

fmi3Status fmi3ExitConfigurationMode(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3Warning;
}

fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  fmi3Float64 intervals[],
                                  fmi3IntervalQualifier qualifiers[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervals);
    UNUSED(qualifiers);
    return fmi3Warning;
}

fmi3Status fmi3GetIntervalFraction(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   fmi3UInt64 intervalCounters[],
                                   fmi3UInt64 resolutions[],
                                   fmi3IntervalQualifier qualifiers[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervalCounters);
    UNUSED(resolutions);
    UNUSED(qualifiers);
    return fmi3Warning;
}

fmi3Status fmi3GetShiftDecimal(fmi3Instance instance,
                               const fmi3ValueReference valueReferences[],
                               size_t nValueReferences,
                               fmi3Float64 shifts[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(shifts);
    return fmi3Warning;
}

fmi3Status fmi3GetShiftFraction(fmi3Instance instance,
                                const fmi3ValueReference valueReferences[],
                                size_t nValueReferences,
                                fmi3UInt64 shiftCounters[],
                                fmi3UInt64 resolutions[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(shiftCounters);
    UNUSED(resolutions);
    return fmi3Warning;
}

fmi3Status fmi3SetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  const fmi3Float64 intervals[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervals);
    return fmi3Warning;
}

fmi3Status fmi3SetIntervalFraction(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   const fmi3UInt64 intervalCounters[],
                                   const fmi3UInt64 resolutions[]){
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervalCounters);
    UNUSED(resolutions);
    return fmi3Warning;
}

fmi3Status fmi3EvaluateDiscreteStates(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3Warning;
}


fmi3Status fmi3SetShiftDecimal(fmi3Instance instance,
                               const fmi3ValueReference valueReferences[],
                               size_t nValueReferences,
                               const fmi3Float64 shifts[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(shifts);
    return fmi3Warning;
}

fmi3Status fmi3SetShiftFraction(fmi3Instance instance,
                                const fmi3ValueReference valueReferences[],
                                size_t nValueReferences,
                                const fmi3UInt64 counters[],
                                const fmi3UInt64 resolutions[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(counters);
    UNUSED(resolutions);
    return fmi3Warning;
}

fmi3Status fmi3GetOutputDerivatives(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Int32 orders[],
                                    fmi3Float64 values[],
                                    size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(orders);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}
