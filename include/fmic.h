#ifndef FMIC_H
#define FMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include "fmic_types.h"
#include "fmic_types_fmi1.h"
#include "fmic_types_fmi2.h"
#include "fmic_types_fmi3.h"
#include "fmic_functions_fmi1.h"
#include "fmic_functions_fmi2.h"
#include "fmic_functions_fmi3.h"
#include "fmic_public.h"

#undef fmiVersion

#ifdef __cplusplus
#define FMIQUICK_DLLEXPORT __declspec(dllexport)
extern "C" {
#else
#define FMIQUICK_DLLEXPORT
#endif

FMIQUICK_DLLEXPORT fmiHandle* unzipFmu(const char *fmufile, const char* instanceName);
FMIQUICK_DLLEXPORT fmiVersion_t getFmiVersion(fmiHandle *fmu);
FMIQUICK_DLLEXPORT fmi1Handle* loadFmu1(fmiHandle* fmu);
FMIQUICK_DLLEXPORT fmi2Handle* loadFmu2(fmiHandle* fmu);
FMIQUICK_DLLEXPORT fmi3Handle* loadFmu3(fmiHandle* fmu);
FMIQUICK_DLLEXPORT void *freeFmu2(fmi2Handle* fmuContents);
FMIQUICK_DLLEXPORT void *freeFmu3(fmi3Handle* fmuContents);


FMIQUICK_DLLEXPORT bool fmi2DefaultStartTimeDefined(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT bool fmi2DefaultStopTimeDefined(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT bool fmi2DefaultToleranceDefined(fmi2Handle *fmu) ;
FMIQUICK_DLLEXPORT bool fmi2DefaultStepSizeDefined(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT double fmi2GetDefaultStartTime(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT double fmi2GetDefaultStopTime(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT double fmi2GetDefaultTolerance(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT double fmi2GetDefaultStepSize(fmi2Handle *fmu);

FMIQUICK_DLLEXPORT int fmi2GetNumberOfVariables(fmi2Handle *fmu);
FMIQUICK_DLLEXPORT fmi2VariableHandle* fmi2GetVariableByIndex(fmi2Handle *fmu, int i);
FMIQUICK_DLLEXPORT fmi2VariableHandle* fmi2GetVariableByValueReference(fmi2Handle *fmu, fmi3ValueReference vr);
FMIQUICK_DLLEXPORT const char* fmi2GetVariableName(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT const char* fmi2GetVariableDescription(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT const char* fmi2GetVariableQuantity(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT const char* fmi2GetVariableUnit(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT const char* fmi2GetVariableDisplayUnit(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT fmi2Real fmi2GetVariableStartReal(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT long fmi2GetVariableValueReference(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT fmi2Causality fmi2GetVariableCausality(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT fmi2Variability fmi2GetVariableVariability(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT fmi2Initial fmi2GetVariableInitial(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT bool fmi2GetVariableCanHandleMultipleSetPerTimeInstant(fmi2VariableHandle* var);
FMIQUICK_DLLEXPORT fmi2DataType fmi2GetVariableDataType(fmi2VariableHandle* var);

FMIQUICK_DLLEXPORT const char* fmi2GetTypesPlatform(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT const char* fmi2GetVersion(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT fmi2Status fmi2SetDebugLogging(fmi2Handle* fmu, fmi2Boolean, size_t, const fmi2String[]);
FMIQUICK_DLLEXPORT const char* fmi2GetGuid(fmi2Handle *fmu);

FMIQUICK_DLLEXPORT const char* fmi2GetModelIdentifier(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetNeedsExecutionTool(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanHandleVariableCommunicationStepSize(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanInterpolateInputs(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT int fmi2GetMaxOutputDerivativeOrder(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanRunAsynchronuously(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanBeInstantiatedOnlyOncePerProcess(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanNotUseMemoryManagementFunctions(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanGetAndSetFMUState(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetCanSerializeFMUState(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi2GetProvidesDirectionalDerivative(fmi2Handle* fmu);

FMIQUICK_DLLEXPORT bool fmi2Instantiate(fmi2Handle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn);
FMIQUICK_DLLEXPORT void fmi2FreeInstance(fmi2Handle* fmu);

FMIQUICK_DLLEXPORT fmi2Status fmi2SetupExperiment(fmi2Handle* fmu, fmi2Boolean, fmi2Real, fmi2Real, fmi2Boolean, fmi2Real);
FMIQUICK_DLLEXPORT fmi2Status fmi2EnterInitializationMode(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT fmi2Status fmi2ExitInitializationMode(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT fmi2Status fmi2Terminate(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT fmi2Status fmi2Reset(fmi2Handle* fmu);

FMIQUICK_DLLEXPORT fmi2Status fmi2GetReal(fmi2Handle* fmu, const fmi2ValueReference[], size_t, fmi2Real   []);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetInteger(fmi2Handle* fmu, const fmi2ValueReference[], size_t, fmi2Integer[]);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetBoolean(fmi2Handle* fmu, const fmi2ValueReference[], size_t, fmi2Boolean[]);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetString(fmi2Handle* fmu, const fmi2ValueReference[], size_t, fmi2String []);

FMIQUICK_DLLEXPORT fmi2Status fmi2SetReal(fmi2Handle* fmu, const fmi2ValueReference[], size_t, const fmi2Real   []);
FMIQUICK_DLLEXPORT fmi2Status fmi2SetInteger(fmi2Handle* fmu, const fmi2ValueReference[], size_t, const fmi2Integer[]);
FMIQUICK_DLLEXPORT fmi2Status fmi2SetBoolean(fmi2Handle* fmu, const fmi2ValueReference[], size_t, const fmi2Boolean[]);
FMIQUICK_DLLEXPORT fmi2Status fmi2SetString(fmi2Handle* fmu, const fmi2ValueReference[], size_t, const fmi2String []);

FMIQUICK_DLLEXPORT fmi2Status fmi2GetFMUstate(fmi2Handle* fmu, fmi2FMUstate*);
FMIQUICK_DLLEXPORT fmi2Status fmi2SetFMUstate(fmi2Handle* fmu, fmi2FMUstate);
FMIQUICK_DLLEXPORT fmi2Status fmi2FreeFMUstate(fmi2Handle* fmu, fmi2FMUstate*);
FMIQUICK_DLLEXPORT fmi2Status fmi2SerializedFMUstateSize(fmi2Handle* fmu, fmi2FMUstate, size_t*);
FMIQUICK_DLLEXPORT fmi2Status fmi2SerializeFMUstate(fmi2Handle* fmu, fmi2FMUstate, fmi2Byte[], size_t);
FMIQUICK_DLLEXPORT fmi2Status fmi2DeSerializeFMUstate(fmi2Handle* fmu, const fmi2Byte[], size_t, fmi2FMUstate*);

FMIQUICK_DLLEXPORT fmi2Status fmi2GetDirectionalDerivative(fmi2Handle* fmu, const fmi2ValueReference[], size_t,
                                                                const fmi2ValueReference[], size_t,
                                                                const fmi2Real[], fmi2Real[]);

FMIQUICK_DLLEXPORT fmi2Status fmi2EnterEventMode(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT fmi2Status fmi2NewDiscreteStates(fmi2Handle* fmu, fmi2EventInfo*);
FMIQUICK_DLLEXPORT fmi2Status fmi2EnterContinuousTimeMode(fmi2Handle* fmu);
FMIQUICK_DLLEXPORT fmi2Status fmi2CompletedIntegratorStep(fmi2Handle* fmu, fmi2Boolean, fmi2Boolean*, fmi2Boolean*);

FMIQUICK_DLLEXPORT fmi2Status fmi2SetTime(fmi2Handle* fmu, fmi2Real);
FMIQUICK_DLLEXPORT fmi2Status fmi2SetContinuousStates(fmi2Handle* fmu, const fmi2Real[], size_t);

FMIQUICK_DLLEXPORT fmi2Status fmi2GetDerivatives(fmi2Handle* fmu, fmi2Real[], size_t);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetEventIndicators(fmi2Handle* fmu, fmi2Real[], size_t);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetContinuousStates(fmi2Handle* fmu, fmi2Real[], size_t);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Handle* fmu, fmi2Real[], size_t);

FMIQUICK_DLLEXPORT fmi2Status fmi2SetRealInputDerivatives(fmi2Handle* fmu, const fmi2ValueReference [], size_t, const fmi2Integer [], const fmi2Real []);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetRealOutputDerivatives(fmi2Handle* fmu, const fmi2ValueReference [], size_t, const fmi2Integer [], fmi2Real []);

FMIQUICK_DLLEXPORT fmi2Status fmi2DoStep(fmi2Handle *fmu, fmi2Real, fmi2Real, fmi2Boolean);
FMIQUICK_DLLEXPORT fmi2Status fmi2CancelStep(fmi2Handle* fmu);

FMIQUICK_DLLEXPORT fmi2Status fmi2GetStatus(fmi2Handle* fmu, const fmi2StatusKind, fmi2Status* );
FMIQUICK_DLLEXPORT fmi2Status fmi2GetRealStatus(fmi2Handle* fmu, const fmi2StatusKind, fmi2Real*   );
FMIQUICK_DLLEXPORT fmi2Status fmi2GetIntegerStatus(fmi2Handle* fmu, const fmi2StatusKind, fmi2Integer*);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetBooleanStatus(fmi2Handle* fmu, const fmi2StatusKind, fmi2Boolean*);
FMIQUICK_DLLEXPORT fmi2Status fmi2GetStringStatus(fmi2Handle* fmu, const fmi2StatusKind, fmi2String* );


FMIQUICK_DLLEXPORT int fmi3GetNumberOfVariables(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT fmi3VariableHandle* fmi3GetVariableByIndex(fmi3Handle *fmu, int i);
FMIQUICK_DLLEXPORT fmi3VariableHandle* fmi3GetVariableByValueReference(fmi3Handle *fmu, fmi3ValueReference vr);
FMIQUICK_DLLEXPORT const char* fmi3GetVariableName(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT fmi3Causality fmi3GetVariableCausality(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT fmi3Variability fmi3GetVariableVariability(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT fmi3Initial fmi3GetVariableInitial(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT bool fmi3GetVariableSupportsIntermediateUpdate(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT fmi3DataType fmi3GetVariableDataType(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT char *fmi3GetVariableDescription(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT char *fmi3GetVariableQuantity(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT char *fmi3GetVariableUnit(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT char *fmi3GetVariableDisplayUnit(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT fmi3Float64 fmi3GetVariableStartFloat64(fmi3VariableHandle* var);
FMIQUICK_DLLEXPORT fmi3ValueReference fmi3GetVariableValueReference(fmi3VariableHandle* var);

FMIQUICK_DLLEXPORT const char* fmi3ModelName(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3InstantiationToken(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3Description(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3Author(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3Version(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3Copyright(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3License(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3GenerationTool(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3GenerationDateAndTime(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT const char* fmi3VariableNamingConvention(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT bool fmi3DefaultStartTimeDefined(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT bool fmi3DefaultStopTimeDefined(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT bool fmi3DefaultToleranceDefined(fmi3Handle *fmu) ;
FMIQUICK_DLLEXPORT bool fmi3DefaultStepSizeDefined(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT double fmi3GetDefaultStartTime(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT double fmi3GetDefaultStopTime(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT double fmi3GetDefaultTolerance(fmi3Handle *fmu);
FMIQUICK_DLLEXPORT double fmi3GetDefaultStepSize(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT const char* fmi3GetModelIdentifier(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetNeedsExecutionTool(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetCanBeInstantiatedOnlyOncePerProcess(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetCanGetAndSetFMUState(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetCanSerializeFMUState(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetProvidesDirectionalDerivative(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetProvidesAdjointDerivatives(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetProvidesPerElementDependencies(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT int fmi3GetMaxOutputDerivativeOrder(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetCanHandleVariableCommunicationStepSize(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetCanReturnEarlyAfterIntermediateUpdate(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT double fmi3GetFixedInternalStepSize(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetHasEventMode(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetProvidesIntermediateUpdate(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetProvidesEvaluateDiscreteStates(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetRecommendedIntermediateInputSmoothness(fmi3Handle* fmu);
FMIQUICK_DLLEXPORT bool fmi3GetCompletedIntegratorStepNotNeeded(fmi3Handle* fmu);

FMIQUICK_DLLEXPORT void fmi3InstantiateCoSimulation(fmi3Handle *fmu,
                             fmi3Boolean                    visible,
                             fmi3Boolean                    loggingOn,
                             fmi3Boolean                    eventModeUsed,
                             fmi3Boolean                    earlyReturnAllowed,
                             const fmi3ValueReference       requiredIntermediateVariables[],
                             size_t                         nRequiredIntermediateVariables,
                             fmi3InstanceEnvironment        instanceEnvironment,
                             fmi3CallbackLogMessage         logMessage,
                             fmi3CallbackIntermediateUpdate intermediateUpdate);

FMIQUICK_DLLEXPORT const char* fmi3GetVersion(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetDebugLogging(fmi3Handle *fmu,
                           fmi3Boolean loggingOn,
                           size_t nCategories,
                           const fmi3String categories[]);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetFloat64(fmi3Handle *fmu,
                      const fmi3ValueReference valueReferences[],
                      size_t nValueReferences,
                      fmi3Float64 values[],
                      size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetFloat64(fmi3Handle *fmu,
                      const fmi3ValueReference valueReferences[],
                      size_t nValueReferences,
                      fmi3Float64 values[],
                      size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3EnterInitializationMode(fmi3Handle *fmu,
                                   fmi3Boolean toleranceDefined,
                                   fmi3Float64 tolerance,
                                   fmi3Float64 startTime,
                                   fmi3Boolean stopTimeDefined,
                                   fmi3Float64 stopTime);

FMIQUICK_DLLEXPORT fmi3Status fmi3ExitInitializationMode(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3Terminate(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT void fmi3FreeInstance(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3DoStep(fmi3Handle *fmu,
                  fmi3Float64 currentCommunicationPoint,
                  fmi3Float64 communicationStepSize,
                  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                  fmi3Boolean *eventEncountered,
                  fmi3Boolean *terminateSimulation,
                  fmi3Boolean *earlyReturn,
                  fmi3Float64 *lastSuccessfulTime);

FMIQUICK_DLLEXPORT fmi3Status fmi3EnterEventMode(fmi3Handle *fmu,
                                                 fmi3Boolean stepEvent,
                                                 fmi3Boolean stateEvent,
                                                 const fmi3Int32 rootsFound[],
                                                 size_t nEventIndicators,
                                                 fmi3Boolean timeEvent);

FMIQUICK_DLLEXPORT fmi3Status fmi3Reset(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetFloat32(fmi3Handle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             fmi3Float32 values[],
                                             size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetInt8(fmi3Handle *fmu,
                                          const fmi3ValueReference valueReferences[],
                                          size_t nValueReferences,
                                          fmi3Int8 values[],
                                          size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetUInt8(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3UInt8 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetInt16(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Int16 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetUInt16(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt16 values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetInt32(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Int32 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetUInt32(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt32 values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetInt64(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Int64 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetUInt64(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt64 values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetBoolean(fmi3Handle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             fmi3Boolean values[],
                                             size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetString(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3String values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetBinary(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            size_t valueSizes[],
                                            fmi3Binary values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetClock(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Clock values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetFloat32(fmi3Handle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             const fmi3Float32 values[],
                                             size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetInt8(fmi3Handle *fmu,
                                          const fmi3ValueReference valueReferences[],
                                          size_t nValueReferences,
                                          const fmi3Int8 values[],
                                          size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetUInt8(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3UInt8 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetInt16(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Int16 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetUInt16(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3UInt16 values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetInt32(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Int32 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetUInt32(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3UInt32 values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetInt64(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Int64 values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetUInt64(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3UInt64 values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetBoolean(fmi3Handle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             const fmi3Boolean values[],
                                             size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetString(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3String values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetBinary(fmi3Handle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const size_t valueSizes[],
                                            const fmi3Binary values[],
                                            size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetClock(fmi3Handle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Clock values[],
                                           size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Handle *fmu,
                                                                  fmi3ValueReference valueReference,
                                                                  size_t* nDependencies);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetVariableDependencies(fmi3Handle *fmu,
                                                          fmi3ValueReference dependent,
                                                          size_t elementIndicesOfDependent[],
                                                          fmi3ValueReference independents[],
                                                          size_t elementIndicesOfIndependents[],
                                                          fmi3DependencyKind dependencyKinds[],
                                                          size_t nDependencies);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetFMUState(fmi3Handle *fmu, fmi3FMUState* FMUState);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetFMUState(fmi3Handle *fmu, fmi3FMUState  FMUState);

FMIQUICK_DLLEXPORT fmi3Status fmi3FreeFMUState(fmi3Handle *fmu, fmi3FMUState* FMUState);

FMIQUICK_DLLEXPORT fmi3Status fmi3SerializedFMUStateSize(fmi3Handle *fmu,
                                                         fmi3FMUState  FMUState,
                                                         size_t* size);

FMIQUICK_DLLEXPORT fmi3Status fmi3SerializeFMUState(fmi3Handle *fmu,
                                                    fmi3FMUState  FMUState,
                                                    fmi3Byte serializedState[],
                                                    size_t size);

FMIQUICK_DLLEXPORT fmi3Status fmi3DeSerializeFMUState(fmi3Handle *fmu,
                                                      const fmi3Byte serializedState[],
                                                      size_t size,
                                                      fmi3FMUState* FMUState);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetDirectionalDerivative(fmi3Handle *fmu,
                                                           const fmi3ValueReference unknowns[],
                                                           size_t nUnknowns,
                                                           const fmi3ValueReference knowns[],
                                                           size_t nKnowns,
                                                           const fmi3Float64 seed[],
                                                           size_t nSeed,
                                                           fmi3Float64 sensitivity[],
                                                           size_t nSensitivity);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetAdjointDerivative(fmi3Handle *fmu,
                                                       const fmi3ValueReference unknowns[],
                                                       size_t nUnknowns,
                                                       const fmi3ValueReference knowns[],
                                                       size_t nKnowns,
                                                       const fmi3Float64 seed[],
                                                       size_t nSeed,
                                                       fmi3Float64 sensitivity[],
                                                       size_t nSensitivity);

FMIQUICK_DLLEXPORT fmi3Status fmi3EnterConfigurationMode(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3ExitConfigurationMode(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetIntervalDecimal(fmi3Handle *fmu,
                                                     const fmi3ValueReference valueReferences[],
                                                     size_t nValueReferences,
                                                     fmi3Float64 intervals[],
                                                     fmi3IntervalQualifier qualifiers[],
                                                     size_t nIntervals);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetIntervalFraction(fmi3Handle *fmu,
                                                      const fmi3ValueReference valueReferences[],
                                                      size_t nValueReferences,
                                                      fmi3UInt64 intervalCounters[],
                                                      fmi3UInt64 resolutions[],
                                                      fmi3IntervalQualifier qualifiers[],
                                                      size_t nIntervals);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetShiftDecimal(fmi3Handle *fmu,
                                                  const fmi3ValueReference valueReferences[],
                                                  size_t nValueReferences,
                                                  fmi3Float64 shifts[],
                                                  size_t nShifts);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetShiftFraction(fmi3Handle *fmu,
                                                   const fmi3ValueReference valueReferences[],
                                                   size_t nValueReferences,
                                                   fmi3UInt64 shiftCounters[],
                                                   fmi3UInt64 resolutions[],
                                                   size_t nShifts);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetIntervalDecimal(fmi3Handle *fmu,
                                                     const fmi3ValueReference valueReferences[],
                                                     size_t nValueReferences,
                                                     const fmi3Float64 intervals[],
                                                     size_t nIntervals);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetIntervalFraction(fmi3Handle *fmu,
                                                      const fmi3ValueReference valueReferences[],
                                                      size_t nValueReferences,
                                                      const fmi3UInt64 intervalCounters[],
                                                      const fmi3UInt64 resolutions[],
                                                      size_t nIntervals);

FMIQUICK_DLLEXPORT fmi3Status fmi3EvaluateDiscreteStates(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3UpdateDiscreteStates(fmi3Handle *fmu,
                                                       fmi3Boolean* discreteStatesNeedUpdate,
                                                       fmi3Boolean* terminateSimulation,
                                                       fmi3Boolean* nominalsOfContinuousStatesChanged,
                                                       fmi3Boolean* valuesOfContinuousStatesChanged,
                                                       fmi3Boolean* nextEventTimeDefined,
                                                       fmi3Float64* nextEventTime);

FMIQUICK_DLLEXPORT fmi3Status fmi3EnterContinuousTimeMode(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3CompletedIntegratorStep(fmi3Handle *fmu,
                                                          fmi3Boolean  noSetFMUStatePriorToCurrentPoint,
                                                          fmi3Boolean* enterEventMode,
                                                          fmi3Boolean* terminateSimulation);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetTime(fmi3Handle *fmu, fmi3Float64 time);

FMIQUICK_DLLEXPORT fmi3Status fmi3SetContinuousStates(fmi3Handle *fmu,
                                                      const fmi3Float64 continuousStates[],
                                                      size_t nContinuousStates);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetContinuousStateDerivatives(fmi3Handle *fmu,
                                                                fmi3Float64 derivatives[],
                                                                size_t nContinuousStates);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetEventIndicators(fmi3Handle *fmu,
                                                     fmi3Float64 eventIndicators[],
                                                     size_t nEventIndicators);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetContinuousStates(fmi3Handle *fmu,
                                                      fmi3Float64 continuousStates[],
                                                      size_t nContinuousStates);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Handle *fmu,
                                                                fmi3Float64 nominals[],
                                                                size_t nContinuousStates);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetNumberOfEventIndicators(fmi3Handle *fmu,
                                                             size_t* nEventIndicators);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetNumberOfContinuousStates(fmi3Handle *fmu,
                                                              size_t* nContinuousStates);

FMIQUICK_DLLEXPORT fmi3Status fmi3EnterStepMode(fmi3Handle *fmu);

FMIQUICK_DLLEXPORT fmi3Status fmi3GetOutputDerivatives(fmi3Handle *fmu,
                                                       const fmi3ValueReference valueReferences[],
                                                       size_t nValueReferences,
                                                       const fmi3Int32 orders[],
                                                       fmi3Float64 values[],
                                                       size_t nValues);

FMIQUICK_DLLEXPORT fmi3Status fmi3ActivateModelPartition(fmi3Handle *fmu,
                                                         fmi3ValueReference clockReference,
                                                         size_t clockElementIndex,
                                                         fmi3Float64 activationTime);

#ifdef __cplusplus
}
#endif


#endif // FMIC_H
