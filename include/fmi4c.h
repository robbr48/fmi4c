#ifndef FMIC_H
#define FMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif

#include "fmi4c_types.h"
#include "fmi4c_types_fmi1.h"
#include "fmi4c_types_fmi2.h"
#include "fmi4c_types_fmi3.h"
#include "fmi4c_functions_fmi1.h"
#include "fmi4c_functions_fmi2.h"
#include "fmi4c_functions_fmi3.h"
#include "fmi4c_public.h"

#undef fmiVersion

#ifdef __cplusplus
#ifdef _WIN32
#define FMIC_DLLEXPORT __declspec(dllexport)
#else
#define FMIC_DLLEXPORT
#endif
extern "C" {
#else
#define FMIC_DLLEXPORT
#endif

// FMU access functions

FMIC_DLLEXPORT fmiVersion_t getFmiVersion(fmiHandle *fmu);
FMIC_DLLEXPORT fmiHandle* loadFmu(const char *fmufile, const char* instanceName);
FMIC_DLLEXPORT void freeFmu(fmiHandle* fmu);
const char* fmi4c_getErrorMessages();

// FMI 1 wrapper functions
FMIC_DLLEXPORT fmi1Type fmi1GetType(fmiHandle *fmu);
FMIC_DLLEXPORT int fmi1GetNumberOfContinuousStates(fmiHandle *fmu);
FMIC_DLLEXPORT int fmi1GetNumberOfEventIndicators(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi1DefaultStartTimeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi1DefaultStopTimeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi1DefaultToleranceDefined(fmiHandle *fmu) ;
FMIC_DLLEXPORT double fmi1GetDefaultStartTime(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi1GetDefaultStopTime(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi1GetDefaultTolerance(fmiHandle *fmu);

FMIC_DLLEXPORT int fmi1GetNumberOfVariables(fmiHandle *fmu);
FMIC_DLLEXPORT fmi1VariableHandle* fmi1GetVariableByIndex(fmiHandle *fmu, int i);
FMIC_DLLEXPORT fmi1VariableHandle* fmi1GetVariableByValueReference(fmiHandle *fmu, fmi1ValueReference vr);
FMIC_DLLEXPORT fmi1VariableHandle* fmi1GetVariableByName(fmiHandle *fmu, fmi1String name);
FMIC_DLLEXPORT const char* fmi1GetVariableName(fmi1VariableHandle* var);
FMIC_DLLEXPORT const char* fmi1GetVariableDescription(fmi1VariableHandle* var);
FMIC_DLLEXPORT const char* fmi1GetVariableQuantity(fmi1VariableHandle* var);
FMIC_DLLEXPORT const char* fmi1GetVariableUnit(fmi1VariableHandle* var);
FMIC_DLLEXPORT const char* fmi1GetVariableDisplayUnit(fmi1VariableHandle* var);
FMIC_DLLEXPORT fmi1Real fmi1GetVariableStartReal(fmi1VariableHandle* var);
FMIC_DLLEXPORT long fmi1GetVariableValueReference(fmi1VariableHandle* var);
FMIC_DLLEXPORT fmi1Causality fmi1GetVariableCausality(fmi1VariableHandle* var);
FMIC_DLLEXPORT fmi1Variability fmi1GetVariableVariability(fmi1VariableHandle* var);
FMIC_DLLEXPORT fmi1Alias fmi1GetAlias(fmi1VariableHandle* var);
FMIC_DLLEXPORT bool fmi1GetVariableIsFixed(fmi1VariableHandle* var);
FMIC_DLLEXPORT fmi1DataType fmi1GetVariableDataType(fmi1VariableHandle* var);

FMIC_DLLEXPORT const char* fmi1GetTypesPlatform(fmiHandle* fmu);
FMIC_DLLEXPORT const char* fmi1GetVersion(fmiHandle* fmu);
FMIC_DLLEXPORT fmi1Status fmi1SetDebugLogging(fmiHandle* fmu, fmi1Boolean);

FMIC_DLLEXPORT fmi1Status fmi1GetReal(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Real values[]);
FMIC_DLLEXPORT fmi1Status fmi1GetInteger(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Integer values[]);
FMIC_DLLEXPORT fmi1Status fmi1GetBoolean(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Boolean values[]);
FMIC_DLLEXPORT fmi1Status fmi1GetString(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1String values[]);

FMIC_DLLEXPORT fmi1Status fmi1SetReal(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Real values[]);
FMIC_DLLEXPORT fmi1Status fmi1SetInteger(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer values[]);
FMIC_DLLEXPORT fmi1Status fmi1SetBoolean(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Boolean values[]);
FMIC_DLLEXPORT fmi1Status fmi1SetString(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1String values[]);

FMIC_DLLEXPORT bool fmi1InstantiateSlave(fmiHandle *fmu, fmi1String mimeType, fmi1Real timeOut, fmi1Boolean visible, fmi1Boolean interactive, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished, fmi3Boolean loggingOn);
FMIC_DLLEXPORT fmi1Status fmi1InitializeSlave(fmiHandle* fmu, fmi1Real startTime, fmi1Boolean stopTimeDefined, fmi1Real stopTime);
FMIC_DLLEXPORT fmi1Status fmi1TerminateSlave(fmiHandle* fmu);
FMIC_DLLEXPORT fmi1Status fmi1ResetSlave(fmiHandle* fmu);
FMIC_DLLEXPORT void fmi1FreeSlaveInstance(fmiHandle* fmu);

FMIC_DLLEXPORT fmi1Status fmi1SetRealInputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], const fmi1Real values[]);
FMIC_DLLEXPORT fmi1Status fmi1GetRealOutputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], fmi1Real values[]);
FMIC_DLLEXPORT fmi1Status fmi1CancelStep(fmiHandle* fmu);
FMIC_DLLEXPORT fmi1Status fmi1DoStep(fmiHandle *fmu, fmi1Real currentCommunicationPoint, fmi1Real communicationStepSize, fmi1Boolean newStep);
FMIC_DLLEXPORT fmi1Status fmi1GetStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Status* value);
FMIC_DLLEXPORT fmi1Status fmi1GetRealStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Real* value);
FMIC_DLLEXPORT fmi1Status fmi1GetIntegerStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Integer* value);
FMIC_DLLEXPORT fmi1Status fmi1GetBooleanStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Boolean* value);
FMIC_DLLEXPORT fmi1Status fmi1GetStringStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1String* value);

FMIC_DLLEXPORT const char *fmi1GetModelTypesPlatform(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi1InstantiateModel(fmiHandle *fmu, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished,fmi1Boolean loggingOn);
FMIC_DLLEXPORT void fmi1FreeModelInstance(fmiHandle* fmu);
FMIC_DLLEXPORT fmi1Status fmi1SetTime(fmiHandle* fmu, fmi1Real);
FMIC_DLLEXPORT fmi1Status fmi1SetContinuousStates(fmiHandle* fmu, const fmi1Real[], size_t);
FMIC_DLLEXPORT fmi1Status fmi1CompletedIntegratorStep(fmiHandle* fmu, fmi1Boolean* callEventUpdate);
FMIC_DLLEXPORT fmi1Status fmi1Initialize(fmiHandle *fmu, fmi1Boolean toleranceControlled, fmi1Real relativeTolerance, fmi1EventInfo *eventInfo);
FMIC_DLLEXPORT fmi1Status fmi1GetDerivatives(fmiHandle *fmu, fmi1Real derivatives[], size_t nDerivatives);
FMIC_DLLEXPORT fmi1Status fmi1GetEventIndicators(fmiHandle *fmu, fmi1Real indicators[], size_t nIndicators);
FMIC_DLLEXPORT fmi1Status fmi1EventUpdate(fmiHandle *fmu, fmi1Boolean intermediateResults, fmi1EventInfo *eventInfo);
FMIC_DLLEXPORT fmi1Status fmi1GetContinuousStates(fmiHandle* fmu, fmi1Real[], size_t);
FMIC_DLLEXPORT fmi1Status fmi1GetNominalContinuousStates(fmiHandle* fmu, fmi1Real nominals[], size_t nNominals);
FMIC_DLLEXPORT fmi1Status fmi1GetStateValueReferences(fmiHandle* fmu, fmi1ValueReference valueReferences[], size_t nValueReferences);
FMIC_DLLEXPORT fmi1Status fmi1Terminate(fmiHandle* fmu);

// FMI 2 wrapper functions

FMIC_DLLEXPORT bool fmi2DefaultStartTimeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi2DefaultStopTimeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi2DefaultToleranceDefined(fmiHandle *fmu) ;
FMIC_DLLEXPORT bool fmi2DefaultStepSizeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi2GetDefaultStartTime(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi2GetDefaultStopTime(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi2GetDefaultTolerance(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi2GetDefaultStepSize(fmiHandle *fmu);

FMIC_DLLEXPORT int fmi2GetNumberOfVariables(fmiHandle *fmu);
FMIC_DLLEXPORT fmi2VariableHandle* fmi2GetVariableByIndex(fmiHandle *fmu, int i);
FMIC_DLLEXPORT fmi2VariableHandle* fmi2GetVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr);
FMIC_DLLEXPORT fmi2VariableHandle* fmi2GetVariableByName(fmiHandle *fmu, fmi2String name);
FMIC_DLLEXPORT const char* fmi2GetVariableName(fmi2VariableHandle* var);
FMIC_DLLEXPORT const char* fmi2GetVariableDescription(fmi2VariableHandle* var);
FMIC_DLLEXPORT const char* fmi2GetVariableQuantity(fmi2VariableHandle* var);
FMIC_DLLEXPORT const char* fmi2GetVariableUnit(fmi2VariableHandle* var);
FMIC_DLLEXPORT const char* fmi2GetVariableDisplayUnit(fmi2VariableHandle* var);
FMIC_DLLEXPORT fmi2Real fmi2GetVariableStartReal(fmi2VariableHandle* var);
FMIC_DLLEXPORT long fmi2GetVariableValueReference(fmi2VariableHandle* var);
FMIC_DLLEXPORT fmi2Causality fmi2GetVariableCausality(fmi2VariableHandle* var);
FMIC_DLLEXPORT fmi2Variability fmi2GetVariableVariability(fmi2VariableHandle* var);
FMIC_DLLEXPORT fmi2Initial fmi2GetVariableInitial(fmi2VariableHandle* var);
FMIC_DLLEXPORT bool fmi2GetVariableCanHandleMultipleSetPerTimeInstant(fmi2VariableHandle* var);
FMIC_DLLEXPORT fmi2DataType fmi2GetVariableDataType(fmi2VariableHandle* var);

FMIC_DLLEXPORT const char* fmi2GetTypesPlatform(fmiHandle* fmu);
FMIC_DLLEXPORT const char* fmi2GetVersion(fmiHandle* fmu);
FMIC_DLLEXPORT fmi2Status fmi2SetDebugLogging(fmiHandle* fmu, fmi2Boolean, size_t, const fmi2String[]);
FMIC_DLLEXPORT const char* fmi2GetGuid(fmiHandle *fmu);

FMIC_DLLEXPORT const char* fmi2GetModelIdentifier(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetNeedsExecutionTool(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanHandleVariableCommunicationStepSize(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanInterpolateInputs(fmiHandle* fmu);
FMIC_DLLEXPORT int fmi2GetMaxOutputDerivativeOrder(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanRunAsynchronuously(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanNotUseMemoryManagementFunctions(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanGetAndSetFMUState(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetCanSerializeFMUState(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi2GetProvidesDirectionalDerivative(fmiHandle* fmu);
FMIC_DLLEXPORT int fmi2GetNumberOfContinuousStates(fmiHandle *fmu);
FMIC_DLLEXPORT int fmi2GetNumberOfEventIndicators(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi2GetSupportsCoSimulation(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi2GetSupportsModelExchange(fmiHandle *fmu);

FMIC_DLLEXPORT bool fmi2Instantiate(fmiHandle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn);
FMIC_DLLEXPORT void fmi2FreeInstance(fmiHandle* fmu);

FMIC_DLLEXPORT fmi2Status fmi2SetupExperiment(fmiHandle* fmu, fmi2Boolean, fmi2Real, fmi2Real, fmi2Boolean, fmi2Real);
FMIC_DLLEXPORT fmi2Status fmi2EnterInitializationMode(fmiHandle* fmu);
FMIC_DLLEXPORT fmi2Status fmi2ExitInitializationMode(fmiHandle* fmu);
FMIC_DLLEXPORT fmi2Status fmi2Terminate(fmiHandle* fmu);
FMIC_DLLEXPORT fmi2Status fmi2Reset(fmiHandle* fmu);

FMIC_DLLEXPORT fmi2Status fmi2GetReal(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Real values[]);
FMIC_DLLEXPORT fmi2Status fmi2GetInteger(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Integer values[]);
FMIC_DLLEXPORT fmi2Status fmi2GetBoolean(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Boolean values[]);
FMIC_DLLEXPORT fmi2Status fmi2GetString(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2String values[]);

FMIC_DLLEXPORT fmi2Status fmi2SetReal(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Real values[]);
FMIC_DLLEXPORT fmi2Status fmi2SetInteger(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Integer values[]);
FMIC_DLLEXPORT fmi2Status fmi2SetBoolean(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Boolean values[]);
FMIC_DLLEXPORT fmi2Status fmi2SetString(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2String values[]);

FMIC_DLLEXPORT fmi2Status fmi2GetFMUstate(fmiHandle* fmu,
                                          fmi2FMUstate* FMUstate);
FMIC_DLLEXPORT fmi2Status fmi2SetFMUstate(fmiHandle* fmu,
                                          fmi2FMUstate FMUstate);
FMIC_DLLEXPORT fmi2Status fmi2FreeFMUstate(fmiHandle* fmu,
                                           fmi2FMUstate* FMUstate);
FMIC_DLLEXPORT fmi2Status fmi2SerializedFMUstateSize(fmiHandle* fmu,
                                                     fmi2FMUstate FMUstate,
                                                     size_t* size);
FMIC_DLLEXPORT fmi2Status fmi2SerializeFMUstate(fmiHandle* fmu,
                                                fmi2FMUstate FMUstate,
                                                fmi2Byte serializedState[],
                                                size_t size);
FMIC_DLLEXPORT fmi2Status fmi2DeSerializeFMUstate(fmiHandle* fmu,
                                                  const fmi2Byte serializedState[],
                                                  size_t size,
                                                  fmi2FMUstate* FMUstate);

FMIC_DLLEXPORT fmi2Status fmi2GetDirectionalDerivative(fmiHandle* fmu,
                                                       const fmi2ValueReference unknownReferences[],
                                                       size_t nUnknown,
                                                       const fmi2ValueReference knownReferences[],
                                                       size_t nKnown,
                                                       const fmi2Real dvKnown[],
                                                       fmi2Real dvUnknown[]);

FMIC_DLLEXPORT fmi2Status fmi2EnterEventMode(fmiHandle* fmu);
FMIC_DLLEXPORT fmi2Status fmi2NewDiscreteStates(fmiHandle* fmu, fmi2EventInfo* eventInfo);
FMIC_DLLEXPORT fmi2Status fmi2EnterContinuousTimeMode(fmiHandle* fmu);
FMIC_DLLEXPORT fmi2Status fmi2CompletedIntegratorStep(fmiHandle* fmu,
                                                      fmi2Boolean noSetFMUStatePriorToCurrentPoint,
                                                      fmi2Boolean* enterEventMode,
                                                      fmi2Boolean* terminateSimulation);

FMIC_DLLEXPORT fmi2Status fmi2SetTime(fmiHandle* fmu,
                                      fmi2Real time);
FMIC_DLLEXPORT fmi2Status fmi2SetContinuousStates(fmiHandle* fmu,
                                                  const fmi2Real x[],
                                                  size_t nx);

FMIC_DLLEXPORT fmi2Status fmi2GetDerivatives(fmiHandle* fmu,
                                             fmi2Real derivatives[],
                                             size_t nx);
FMIC_DLLEXPORT fmi2Status fmi2GetEventIndicators(fmiHandle* fmu,
                                                 fmi2Real eventIndicators[],
                                                 size_t ni);
FMIC_DLLEXPORT fmi2Status fmi2GetContinuousStates(fmiHandle* fmu,
                                                  fmi2Real x[],
                                                  size_t nx);
FMIC_DLLEXPORT fmi2Status fmi2GetNominalsOfContinuousStates(fmiHandle* fmu,
                                                            fmi2Real x_nominal[],
                                                            size_t nx);

FMIC_DLLEXPORT fmi2Status fmi2SetRealInputDerivatives(fmiHandle* fmu,
                                                      const fmi2ValueReference vr[],
                                                      size_t nvr,
                                                      const fmi2Integer order[],
                                                      const fmi2Real value[]);
FMIC_DLLEXPORT fmi2Status fmi2GetRealOutputDerivatives (fmiHandle* fmu,
                                                        const fmi2ValueReference vr[],
                                                        size_t nvr,
                                                        const fmi2Integer order[],
                                                        fmi2Real value[]);

FMIC_DLLEXPORT fmi2Status fmi2DoStep(fmiHandle *fmu, fmi2Real, fmi2Real, fmi2Boolean);
FMIC_DLLEXPORT fmi2Status fmi2CancelStep(fmiHandle* fmu);

FMIC_DLLEXPORT fmi2Status fmi2GetStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Status* );
FMIC_DLLEXPORT fmi2Status fmi2GetRealStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Real*   );
FMIC_DLLEXPORT fmi2Status fmi2GetIntegerStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Integer*);
FMIC_DLLEXPORT fmi2Status fmi2GetBooleanStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Boolean*);
FMIC_DLLEXPORT fmi2Status fmi2GetStringStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2String* );


FMIC_DLLEXPORT int fmi3GetNumberOfVariables(fmiHandle *fmu);
FMIC_DLLEXPORT fmi3VariableHandle* fmi3GetVariableByName(fmiHandle *fmu, fmi3String name);
FMIC_DLLEXPORT fmi3VariableHandle* fmi3GetVariableByIndex(fmiHandle *fmu, int i);
FMIC_DLLEXPORT fmi3VariableHandle* fmi3GetVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr);
FMIC_DLLEXPORT const char* fmi3GetVariableName(fmi3VariableHandle* var);
FMIC_DLLEXPORT fmi3Causality fmi3GetVariableCausality(fmi3VariableHandle* var);
FMIC_DLLEXPORT fmi3Variability fmi3GetVariableVariability(fmi3VariableHandle* var);
FMIC_DLLEXPORT fmi3Initial fmi3GetVariableInitial(fmi3VariableHandle* var);
FMIC_DLLEXPORT bool fmi3GetVariableSupportsIntermediateUpdate(fmi3VariableHandle* var);
FMIC_DLLEXPORT fmi3DataType fmi3GetVariableDataType(fmi3VariableHandle* var);
FMIC_DLLEXPORT const char *fmi3GetVariableDescription(fmi3VariableHandle* var);
FMIC_DLLEXPORT const char *fmi3GetVariableQuantity(fmi3VariableHandle* var);
FMIC_DLLEXPORT const char *fmi3GetVariableUnit(fmi3VariableHandle* var);
FMIC_DLLEXPORT char *fmi3GetVariableDisplayUnit(fmi3VariableHandle* var);
FMIC_DLLEXPORT fmi3Float64 fmi3GetVariableStartFloat64(fmi3VariableHandle* var);
FMIC_DLLEXPORT fmi3ValueReference fmi3GetVariableValueReference(fmi3VariableHandle* var);

FMIC_DLLEXPORT const char* fmi3ModelName(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3InstantiationToken(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3Description(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3Author(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3Version(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3Copyright(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3License(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3GenerationTool(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3GenerationDateAndTime(fmiHandle *fmu);
FMIC_DLLEXPORT const char* fmi3VariableNamingConvention(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi3SupportsModelExchange(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi3SupportsScheduledExecution(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi3SupportsCoSimulation(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi3DefaultStartTimeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi3DefaultStopTimeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT bool fmi3DefaultToleranceDefined(fmiHandle *fmu) ;
FMIC_DLLEXPORT bool fmi3DefaultStepSizeDefined(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi3GetDefaultStartTime(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi3GetDefaultStopTime(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi3GetDefaultTolerance(fmiHandle *fmu);
FMIC_DLLEXPORT double fmi3GetDefaultStepSize(fmiHandle *fmu);

FMIC_DLLEXPORT const char* fmi3GetModelIdentifier(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetNeedsExecutionTool(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetCanGetAndSetFMUState(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetCanSerializeFMUState(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetProvidesDirectionalDerivative(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetProvidesAdjointDerivatives(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetProvidesPerElementDependencies(fmiHandle* fmu);
FMIC_DLLEXPORT int fmi3GetMaxOutputDerivativeOrder(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetCanHandleVariableCommunicationStepSize(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetCanReturnEarlyAfterIntermediateUpdate(fmiHandle* fmu);
FMIC_DLLEXPORT double fmi3GetFixedInternalStepSize(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetHasEventMode(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetProvidesIntermediateUpdate(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetProvidesEvaluateDiscreteStates(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetRecommendedIntermediateInputSmoothness(fmiHandle* fmu);
FMIC_DLLEXPORT bool fmi3GetCompletedIntegratorStepNotNeeded(fmiHandle* fmu);

FMIC_DLLEXPORT bool fmi3InstantiateCoSimulation(fmiHandle *fmu,
                             fmi3Boolean                    visible,
                             fmi3Boolean                    loggingOn,
                             fmi3Boolean                    eventModeUsed,
                             fmi3Boolean                    earlyReturnAllowed,
                             const fmi3ValueReference       requiredIntermediateVariables[],
                             size_t                         nRequiredIntermediateVariables,
                             fmi3InstanceEnvironment        instanceEnvironment,
                             fmi3CallbackLogMessage         logMessage,
                             fmi3CallbackIntermediateUpdate intermediateUpdate);

FMIC_DLLEXPORT bool fmi3InstantiateModelExchange(fmiHandle *fmu,
                                                 fmi3Boolean                visible,
                                                 fmi3Boolean                loggingOn,
                                                 fmi3InstanceEnvironment    instanceEnvironment,
                                                 fmi3CallbackLogMessage     logMessage);

FMIC_DLLEXPORT const char* fmi3GetVersion(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3SetDebugLogging(fmiHandle *fmu,
                           fmi3Boolean loggingOn,
                           size_t nCategories,
                           const fmi3String categories[]);

FMIC_DLLEXPORT fmi3Status fmi3GetFloat64(fmiHandle *fmu,
                      const fmi3ValueReference valueReferences[],
                      size_t nValueReferences,
                      fmi3Float64 values[],
                      size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetFloat64(fmiHandle *fmu,
                      const fmi3ValueReference valueReferences[],
                      size_t nValueReferences,
                      fmi3Float64 values[],
                      size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3EnterInitializationMode(fmiHandle *fmu,
                                   fmi3Boolean toleranceDefined,
                                   fmi3Float64 tolerance,
                                   fmi3Float64 startTime,
                                   fmi3Boolean stopTimeDefined,
                                   fmi3Float64 stopTime);

FMIC_DLLEXPORT fmi3Status fmi3ExitInitializationMode(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3Terminate(fmiHandle *fmu);

FMIC_DLLEXPORT void fmi3FreeInstance(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3DoStep(fmiHandle *fmu,
                  fmi3Float64 currentCommunicationPoint,
                  fmi3Float64 communicationStepSize,
                  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                  fmi3Boolean *eventEncountered,
                  fmi3Boolean *terminateSimulation,
                  fmi3Boolean *earlyReturn,
                  fmi3Float64 *lastSuccessfulTime);

FMIC_DLLEXPORT fmi3Status fmi3EnterEventMode(fmiHandle *fmu,
                                                 fmi3Boolean stepEvent,
                                                 fmi3Boolean stateEvent,
                                                 const fmi3Int32 rootsFound[],
                                                 size_t nEventIndicators,
                                                 fmi3Boolean timeEvent);

FMIC_DLLEXPORT fmi3Status fmi3Reset(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3GetFloat32(fmiHandle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             fmi3Float32 values[],
                                             size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetInt8(fmiHandle *fmu,
                                          const fmi3ValueReference valueReferences[],
                                          size_t nValueReferences,
                                          fmi3Int8 values[],
                                          size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetUInt8(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3UInt8 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetInt16(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Int16 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetUInt16(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt16 values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetInt32(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Int32 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetUInt32(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt32 values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetInt64(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Int64 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetUInt64(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3UInt64 values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetBoolean(fmiHandle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             fmi3Boolean values[],
                                             size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetString(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3String values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetBinary(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            size_t valueSizes[],
                                            fmi3Binary values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetClock(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           fmi3Clock values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetFloat32(fmiHandle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             const fmi3Float32 values[],
                                             size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetInt8(fmiHandle *fmu,
                                          const fmi3ValueReference valueReferences[],
                                          size_t nValueReferences,
                                          const fmi3Int8 values[],
                                          size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetUInt8(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3UInt8 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetInt16(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Int16 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetUInt16(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3UInt16 values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetInt32(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Int32 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetUInt32(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3UInt32 values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetInt64(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Int64 values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetUInt64(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3UInt64 values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetBoolean(fmiHandle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             const fmi3Boolean values[],
                                             size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetString(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const fmi3String values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetBinary(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            const size_t valueSizes[],
                                            const fmi3Binary values[],
                                            size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3SetClock(fmiHandle *fmu,
                                           const fmi3ValueReference valueReferences[],
                                           size_t nValueReferences,
                                           const fmi3Clock values[],
                                           size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3GetNumberOfVariableDependencies(fmiHandle *fmu,
                                                                  fmi3ValueReference valueReference,
                                                                  size_t* nDependencies);

FMIC_DLLEXPORT fmi3Status fmi3GetVariableDependencies(fmiHandle *fmu,
                                                          fmi3ValueReference dependent,
                                                          size_t elementIndicesOfDependent[],
                                                          fmi3ValueReference independents[],
                                                          size_t elementIndicesOfIndependents[],
                                                          fmi3DependencyKind dependencyKinds[],
                                                          size_t nDependencies);

FMIC_DLLEXPORT fmi3Status fmi3GetFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);

FMIC_DLLEXPORT fmi3Status fmi3SetFMUState(fmiHandle *fmu, fmi3FMUState  FMUState);

FMIC_DLLEXPORT fmi3Status fmi3FreeFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);

FMIC_DLLEXPORT fmi3Status fmi3SerializedFMUStateSize(fmiHandle *fmu,
                                                         fmi3FMUState  FMUState,
                                                         size_t* size);

FMIC_DLLEXPORT fmi3Status fmi3SerializeFMUState(fmiHandle *fmu,
                                                    fmi3FMUState  FMUState,
                                                    fmi3Byte serializedState[],
                                                    size_t size);

FMIC_DLLEXPORT fmi3Status fmi3DeSerializeFMUState(fmiHandle *fmu,
                                                      const fmi3Byte serializedState[],
                                                      size_t size,
                                                      fmi3FMUState* FMUState);

FMIC_DLLEXPORT fmi3Status fmi3GetDirectionalDerivative(fmiHandle *fmu,
                                                           const fmi3ValueReference unknowns[],
                                                           size_t nUnknowns,
                                                           const fmi3ValueReference knowns[],
                                                           size_t nKnowns,
                                                           const fmi3Float64 seed[],
                                                           size_t nSeed,
                                                           fmi3Float64 sensitivity[],
                                                           size_t nSensitivity);

FMIC_DLLEXPORT fmi3Status fmi3GetAdjointDerivative(fmiHandle *fmu,
                                                       const fmi3ValueReference unknowns[],
                                                       size_t nUnknowns,
                                                       const fmi3ValueReference knowns[],
                                                       size_t nKnowns,
                                                       const fmi3Float64 seed[],
                                                       size_t nSeed,
                                                       fmi3Float64 sensitivity[],
                                                       size_t nSensitivity);

FMIC_DLLEXPORT fmi3Status fmi3EnterConfigurationMode(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3ExitConfigurationMode(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3GetIntervalDecimal(fmiHandle *fmu,
                                                     const fmi3ValueReference valueReferences[],
                                                     size_t nValueReferences,
                                                     fmi3Float64 intervals[],
                                                     fmi3IntervalQualifier qualifiers[]);

FMIC_DLLEXPORT fmi3Status fmi3GetIntervalFraction(fmiHandle *fmu,
                                                      const fmi3ValueReference valueReferences[],
                                                      size_t nValueReferences,
                                                      fmi3UInt64 intervalCounters[],
                                                      fmi3UInt64 resolutions[],
                                                      fmi3IntervalQualifier qualifiers[]);

FMIC_DLLEXPORT fmi3Status fmi3GetShiftDecimal(fmiHandle *fmu,
                                                  const fmi3ValueReference valueReferences[],
                                                  size_t nValueReferences,
                                                  fmi3Float64 shifts[]);

FMIC_DLLEXPORT fmi3Status fmi3GetShiftFraction(fmiHandle *fmu,
                                                   const fmi3ValueReference valueReferences[],
                                                   size_t nValueReferences,
                                                   fmi3UInt64 shiftCounters[],
                                                   fmi3UInt64 resolutions[]);

FMIC_DLLEXPORT fmi3Status fmi3SetIntervalDecimal(fmiHandle *fmu,
                                                     const fmi3ValueReference valueReferences[],
                                                     size_t nValueReferences,
                                                     const fmi3Float64 intervals[]);

FMIC_DLLEXPORT fmi3Status fmi3SetIntervalFraction(fmiHandle *fmu,
                                                      const fmi3ValueReference valueReferences[],
                                                      size_t nValueReferences,
                                                      const fmi3UInt64 intervalCounters[],
                                                      const fmi3UInt64 resolutions[]);

FMIC_DLLEXPORT fmi3Status fmi3EvaluateDiscreteStates(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3UpdateDiscreteStates(fmiHandle *fmu,
                                                       fmi3Boolean* discreteStatesNeedUpdate,
                                                       fmi3Boolean* terminateSimulation,
                                                       fmi3Boolean* nominalsOfContinuousStatesChanged,
                                                       fmi3Boolean* valuesOfContinuousStatesChanged,
                                                       fmi3Boolean* nextEventTimeDefined,
                                                       fmi3Float64* nextEventTime);

FMIC_DLLEXPORT fmi3Status fmi3EnterContinuousTimeMode(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3CompletedIntegratorStep(fmiHandle *fmu,
                                                          fmi3Boolean  noSetFMUStatePriorToCurrentPoint,
                                                          fmi3Boolean* enterEventMode,
                                                          fmi3Boolean* terminateSimulation);

FMIC_DLLEXPORT fmi3Status fmi3SetTime(fmiHandle *fmu, fmi3Float64 time);

FMIC_DLLEXPORT fmi3Status fmi3SetContinuousStates(fmiHandle *fmu,
                                                      const fmi3Float64 continuousStates[],
                                                      size_t nContinuousStates);

FMIC_DLLEXPORT fmi3Status fmi3GetContinuousStateDerivatives(fmiHandle *fmu,
                                                                fmi3Float64 derivatives[],
                                                                size_t nContinuousStates);

FMIC_DLLEXPORT fmi3Status fmi3GetEventIndicators(fmiHandle *fmu,
                                                     fmi3Float64 eventIndicators[],
                                                     size_t nEventIndicators);

FMIC_DLLEXPORT fmi3Status fmi3GetContinuousStates(fmiHandle *fmu,
                                                      fmi3Float64 continuousStates[],
                                                      size_t nContinuousStates);

FMIC_DLLEXPORT fmi3Status fmi3GetNominalsOfContinuousStates(fmiHandle *fmu,
                                                                fmi3Float64 nominals[],
                                                                size_t nContinuousStates);

FMIC_DLLEXPORT fmi3Status fmi3GetNumberOfEventIndicators(fmiHandle *fmu,
                                                             size_t* nEventIndicators);

FMIC_DLLEXPORT fmi3Status fmi3GetNumberOfContinuousStates(fmiHandle *fmu,
                                                              size_t* nContinuousStates);

FMIC_DLLEXPORT fmi3Status fmi3EnterStepMode(fmiHandle *fmu);

FMIC_DLLEXPORT fmi3Status fmi3GetOutputDerivatives(fmiHandle *fmu,
                                                       const fmi3ValueReference valueReferences[],
                                                       size_t nValueReferences,
                                                       const fmi3Int32 orders[],
                                                       fmi3Float64 values[],
                                                       size_t nValues);

FMIC_DLLEXPORT fmi3Status fmi3ActivateModelPartition(fmiHandle *fmu,
                                                         fmi3ValueReference clockReference,
                                                         fmi3Float64 activationTime);

#ifdef __cplusplus
}
#endif


#endif // FMIC_H
