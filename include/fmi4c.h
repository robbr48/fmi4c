#ifndef FMIC_H
#define FMIC_H

#include "fmi4c_types.h"
#include "fmi4c_types_fmi1.h"
#include "fmi4c_types_fmi2.h"
#include "fmi4c_types_fmi3.h"
#include "fmi4c_functions_fmi1.h"
#include "fmi4c_functions_fmi2.h"
#include "fmi4c_functions_fmi3.h"
#ifndef FMI4C_H_INTERNAL_INCLUDE
#include "fmi4c_public.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif

#undef fmiVersion

#ifdef _WIN32
#if defined(FMI4C_STATIC)
#define FMI4C_DLLAPI
#elif defined(FMI4C_DLLEXPORT)
#define FMI4C_DLLAPI __declspec(dllexport)
#else
#define FMI4C_DLLAPI __declspec(dllimport)
#endif
#else
#define FMI4C_DLLAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

// FMU access functions

FMI4C_DLLAPI fmiVersion_t getFmiVersion(fmiHandle *fmu);
FMI4C_DLLAPI fmiHandle* loadFmu(const char *fmufile, const char* instanceName);
FMI4C_DLLAPI void freeFmu(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi4c_getErrorMessages();

// FMI 1 wrapper functions
FMI4C_DLLAPI fmi1Type fmi1GetType(fmiHandle *fmu);
FMI4C_DLLAPI int fmi1GetNumberOfContinuousStates(fmiHandle *fmu);
FMI4C_DLLAPI int fmi1GetNumberOfEventIndicators(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi1DefaultStartTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi1DefaultStopTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi1DefaultToleranceDefined(fmiHandle *fmu) ;
FMI4C_DLLAPI double fmi1GetDefaultStartTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi1GetDefaultStopTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi1GetDefaultTolerance(fmiHandle *fmu);

FMI4C_DLLAPI int fmi1GetNumberOfVariables(fmiHandle *fmu);
FMI4C_DLLAPI fmi1VariableHandle* fmi1GetVariableByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI fmi1VariableHandle* fmi1GetVariableByValueReference(fmiHandle *fmu, fmi1ValueReference vr);
FMI4C_DLLAPI fmi1VariableHandle* fmi1GetVariableByName(fmiHandle *fmu, fmi1String name);
FMI4C_DLLAPI const char* fmi1GetVariableName(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1GetVariableDescription(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1GetVariableQuantity(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1GetVariableUnit(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1GetVariableDisplayUnit(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Real fmi1GetVariableStartReal(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Integer fmi1GetVariableStartInteger(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Boolean fmi1GetVariableStartBoolean(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1String fmi1GetVariableStartString(fmi1VariableHandle* var);
FMI4C_DLLAPI long fmi1GetVariableValueReference(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Causality fmi1GetVariableCausality(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Variability fmi1GetVariableVariability(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Alias fmi1GetAlias(fmi1VariableHandle* var);
FMI4C_DLLAPI bool fmi1GetVariableIsFixed(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1DataType fmi1GetVariableDataType(fmi1VariableHandle* var);

FMI4C_DLLAPI const char* fmi1GetTypesPlatform(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi1GetVersion(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1SetDebugLogging(fmiHandle* fmu, fmi1Boolean);

FMI4C_DLLAPI fmi1Status fmi1GetReal(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1GetInteger(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Integer values[]);
FMI4C_DLLAPI fmi1Status fmi1GetBoolean(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Boolean values[]);
FMI4C_DLLAPI fmi1Status fmi1GetString(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1String values[]);

FMI4C_DLLAPI fmi1Status fmi1SetReal(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1SetInteger(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer values[]);
FMI4C_DLLAPI fmi1Status fmi1SetBoolean(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Boolean values[]);
FMI4C_DLLAPI fmi1Status fmi1SetString(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1String values[]);

FMI4C_DLLAPI bool fmi1InstantiateSlave(fmiHandle *fmu, fmi1String mimeType, fmi1Real timeOut, fmi1Boolean visible, fmi1Boolean interactive, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished, fmi3Boolean loggingOn);
FMI4C_DLLAPI fmi1Status fmi1InitializeSlave(fmiHandle* fmu, fmi1Real startTime, fmi1Boolean stopTimeDefined, fmi1Real stopTime);
FMI4C_DLLAPI fmi1Status fmi1TerminateSlave(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1ResetSlave(fmiHandle* fmu);
FMI4C_DLLAPI void fmi1FreeSlaveInstance(fmiHandle* fmu);

FMI4C_DLLAPI fmi1Status fmi1SetRealInputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], const fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1GetRealOutputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1CancelStep(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1DoStep(fmiHandle *fmu, fmi1Real currentCommunicationPoint, fmi1Real communicationStepSize, fmi1Boolean newStep);
FMI4C_DLLAPI fmi1Status fmi1GetStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Status* value);
FMI4C_DLLAPI fmi1Status fmi1GetRealStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Real* value);
FMI4C_DLLAPI fmi1Status fmi1GetIntegerStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Integer* value);
FMI4C_DLLAPI fmi1Status fmi1GetBooleanStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Boolean* value);
FMI4C_DLLAPI fmi1Status fmi1GetStringStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1String* value);

FMI4C_DLLAPI const char *fmi1GetModelTypesPlatform(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi1InstantiateModel(fmiHandle *fmu, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1Boolean loggingOn);
FMI4C_DLLAPI void fmi1FreeModelInstance(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1SetTime(fmiHandle* fmu, fmi1Real);
FMI4C_DLLAPI fmi1Status fmi1SetContinuousStates(fmiHandle* fmu, const fmi1Real[], size_t);
FMI4C_DLLAPI fmi1Status fmi1CompletedIntegratorStep(fmiHandle* fmu, fmi1Boolean* callEventUpdate);
FMI4C_DLLAPI fmi1Status fmi1Initialize(fmiHandle *fmu, fmi1Boolean toleranceControlled, fmi1Real relativeTolerance, fmi1EventInfo *eventInfo);
FMI4C_DLLAPI fmi1Status fmi1GetDerivatives(fmiHandle *fmu, fmi1Real derivatives[], size_t nDerivatives);
FMI4C_DLLAPI fmi1Status fmi1GetEventIndicators(fmiHandle *fmu, fmi1Real indicators[], size_t nIndicators);
FMI4C_DLLAPI fmi1Status fmi1EventUpdate(fmiHandle *fmu, fmi1Boolean intermediateResults, fmi1EventInfo *eventInfo);
FMI4C_DLLAPI fmi1Status fmi1GetContinuousStates(fmiHandle* fmu, fmi1Real[], size_t);
FMI4C_DLLAPI fmi1Status fmi1GetNominalContinuousStates(fmiHandle* fmu, fmi1Real nominals[], size_t nNominals);
FMI4C_DLLAPI fmi1Status fmi1GetStateValueReferences(fmiHandle* fmu, fmi1ValueReference valueReferences[], size_t nValueReferences);
FMI4C_DLLAPI fmi1Status fmi1Terminate(fmiHandle* fmu);

// FMI 2 wrapper functions

FMI4C_DLLAPI bool fmi2DefaultStartTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2DefaultStopTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2DefaultToleranceDefined(fmiHandle *fmu) ;
FMI4C_DLLAPI bool fmi2DefaultStepSizeDefined(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2GetDefaultStartTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2GetDefaultStopTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2GetDefaultTolerance(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2GetDefaultStepSize(fmiHandle *fmu);

FMI4C_DLLAPI int fmi2GetNumberOfVariables(fmiHandle *fmu);
FMI4C_DLLAPI fmi2VariableHandle* fmi2GetVariableByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI fmi2VariableHandle* fmi2GetVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr);
FMI4C_DLLAPI fmi2VariableHandle* fmi2GetVariableByName(fmiHandle *fmu, fmi2String name);
FMI4C_DLLAPI const char* fmi2GetVariableName(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2GetVariableDescription(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2GetVariableQuantity(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2GetVariableUnit(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2GetVariableDisplayUnit(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Real fmi2GetVariableStartReal(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Integer fmi2GetVariableStartInteger(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Boolean fmi2GetVariableStartBoolean(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2String fmi2GetVariableStartString(fmi2VariableHandle* var);
FMI4C_DLLAPI long fmi2GetVariableValueReference(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Causality fmi2GetVariableCausality(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Variability fmi2GetVariableVariability(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Initial fmi2GetVariableInitial(fmi2VariableHandle* var);
FMI4C_DLLAPI bool fmi2GetVariableCanHandleMultipleSetPerTimeInstant(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2DataType fmi2GetVariableDataType(fmi2VariableHandle* var);

FMI4C_DLLAPI const char* fmi2GetTypesPlatform(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi2GetVersion(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2SetDebugLogging(fmiHandle* fmu, fmi2Boolean, size_t, const fmi2String[]);
FMI4C_DLLAPI const char* fmi2GetGuid(fmiHandle *fmu);

FMI4C_DLLAPI const char* fmi2csGetModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmics2GetCanHandleVariableCommunicationStepSize(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetCanInterpolateInputs(fmiHandle* fmu);
FMI4C_DLLAPI int fmi2csGetMaxOutputDerivativeOrder(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetCanRunAsynchronuously(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetCanNotUseMemoryManagementFunctions(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2csGetProvidesDirectionalDerivative(fmiHandle* fmu);

FMI4C_DLLAPI const char* fmi2meGetModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetCompletedIntegratorStepNotNeeded(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetCanNotUseMemoryManagementFunctions(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2meGetProvidesDirectionalDerivative(fmiHandle* fmu);

FMI4C_DLLAPI int fmi2GetNumberOfContinuousStates(fmiHandle *fmu);
FMI4C_DLLAPI int fmi2GetNumberOfEventIndicators(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2GetSupportsCoSimulation(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2GetSupportsModelExchange(fmiHandle *fmu);

FMI4C_DLLAPI bool fmi2Instantiate(fmiHandle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn);
FMI4C_DLLAPI void fmi2FreeInstance(fmiHandle* fmu);

FMI4C_DLLAPI fmi2Status fmi2SetupExperiment(fmiHandle* fmu, fmi2Boolean, fmi2Real, fmi2Real, fmi2Boolean, fmi2Real);
FMI4C_DLLAPI fmi2Status fmi2EnterInitializationMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2ExitInitializationMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2Terminate(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2Reset(fmiHandle* fmu);

FMI4C_DLLAPI fmi2Status fmi2GetReal(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Real values[]);
FMI4C_DLLAPI fmi2Status fmi2GetInteger(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Integer values[]);
FMI4C_DLLAPI fmi2Status fmi2GetBoolean(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Boolean values[]);
FMI4C_DLLAPI fmi2Status fmi2GetString(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2String values[]);

FMI4C_DLLAPI fmi2Status fmi2SetReal(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Real values[]);
FMI4C_DLLAPI fmi2Status fmi2SetInteger(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Integer values[]);
FMI4C_DLLAPI fmi2Status fmi2SetBoolean(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Boolean values[]);
FMI4C_DLLAPI fmi2Status fmi2SetString(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2String values[]);

FMI4C_DLLAPI fmi2Status fmi2GetFMUstate(fmiHandle* fmu,
                                        fmi2FMUstate* FMUstate);
FMI4C_DLLAPI fmi2Status fmi2SetFMUstate(fmiHandle* fmu,
                                        fmi2FMUstate FMUstate);
FMI4C_DLLAPI fmi2Status fmi2FreeFMUstate(fmiHandle* fmu,
                                         fmi2FMUstate* FMUstate);
FMI4C_DLLAPI fmi2Status fmi2SerializedFMUstateSize(fmiHandle* fmu,
                                                   fmi2FMUstate FMUstate,
                                                   size_t* size);
FMI4C_DLLAPI fmi2Status fmi2SerializeFMUstate(fmiHandle* fmu,
                                              fmi2FMUstate FMUstate,
                                              fmi2Byte serializedState[],
                                              size_t size);
FMI4C_DLLAPI fmi2Status fmi2DeSerializeFMUstate(fmiHandle* fmu,
                                                const fmi2Byte serializedState[],
                                                size_t size,
                                                fmi2FMUstate* FMUstate);

FMI4C_DLLAPI fmi2Status fmi2GetDirectionalDerivative(fmiHandle* fmu,
                                                     const fmi2ValueReference unknownReferences[],
                                                     size_t nUnknown,
                                                     const fmi2ValueReference knownReferences[],
                                                     size_t nKnown,
                                                     const fmi2Real dvKnown[],
                                                     fmi2Real dvUnknown[]);

FMI4C_DLLAPI fmi2Status fmi2EnterEventMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2NewDiscreteStates(fmiHandle* fmu, fmi2EventInfo* eventInfo);
FMI4C_DLLAPI fmi2Status fmi2EnterContinuousTimeMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2CompletedIntegratorStep(fmiHandle* fmu,
                                                    fmi2Boolean noSetFMUStatePriorToCurrentPoint,
                                                    fmi2Boolean* enterEventMode,
                                                    fmi2Boolean* terminateSimulation);

FMI4C_DLLAPI fmi2Status fmi2SetTime(fmiHandle* fmu,
                                    fmi2Real time);
FMI4C_DLLAPI fmi2Status fmi2SetContinuousStates(fmiHandle* fmu,
                                                const fmi2Real x[],
                                                size_t nx);

FMI4C_DLLAPI fmi2Status fmi2GetDerivatives(fmiHandle* fmu,
                                           fmi2Real derivatives[],
                                           size_t nx);
FMI4C_DLLAPI fmi2Status fmi2GetEventIndicators(fmiHandle* fmu,
                                               fmi2Real eventIndicators[],
                                               size_t ni);
FMI4C_DLLAPI fmi2Status fmi2GetContinuousStates(fmiHandle* fmu,
                                                fmi2Real x[],
                                                size_t nx);
FMI4C_DLLAPI fmi2Status fmi2GetNominalsOfContinuousStates(fmiHandle* fmu,
                                                          fmi2Real x_nominal[],
                                                          size_t nx);

FMI4C_DLLAPI fmi2Status fmi2SetRealInputDerivatives(fmiHandle* fmu,
                                                    const fmi2ValueReference vr[],
                                                    size_t nvr,
                                                    const fmi2Integer order[],
                                                    const fmi2Real value[]);
FMI4C_DLLAPI fmi2Status fmi2GetRealOutputDerivatives (fmiHandle* fmu,
                                                      const fmi2ValueReference vr[],
                                                      size_t nvr,
                                                      const fmi2Integer order[],
                                                      fmi2Real value[]);

FMI4C_DLLAPI fmi2Status fmi2DoStep(fmiHandle *fmu, fmi2Real, fmi2Real, fmi2Boolean);
FMI4C_DLLAPI fmi2Status fmi2CancelStep(fmiHandle* fmu);

FMI4C_DLLAPI fmi2Status fmi2GetStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Status* );
FMI4C_DLLAPI fmi2Status fmi2GetRealStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Real*   );
FMI4C_DLLAPI fmi2Status fmi2GetIntegerStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Integer*);
FMI4C_DLLAPI fmi2Status fmi2GetBooleanStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2Boolean*);
FMI4C_DLLAPI fmi2Status fmi2GetStringStatus(fmiHandle* fmu, const fmi2StatusKind, fmi2String* );


FMI4C_DLLAPI int fmi3GetNumberOfVariables(fmiHandle *fmu);
FMI4C_DLLAPI fmi3VariableHandle* fmi3GetVariableByName(fmiHandle *fmu, fmi3String name);
FMI4C_DLLAPI fmi3VariableHandle* fmi3GetVariableByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI fmi3VariableHandle* fmi3GetVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr);
FMI4C_DLLAPI const char* fmi3GetVariableName(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Causality fmi3GetVariableCausality(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Variability fmi3GetVariableVariability(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Initial fmi3GetVariableInitial(fmi3VariableHandle* var);
FMI4C_DLLAPI bool fmi3GetVariableSupportsIntermediateUpdate(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3DataType fmi3GetVariableDataType(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3GetVariableDescription(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3GetVariableQuantity(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3GetVariableUnit(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3GetVariableDisplayUnit(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Float64 fmi3GetVariableStartFloat64(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Float32 fmi3GetVariableStartFloat32(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int64 fmi3GetVariableStartInt64(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int32 fmi3GetVariableStartInt32(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int16 fmi3GetVariableStartInt16(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int8 fmi3GetVariableStartInt8(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt64 fmi3GetVariableStartUInt64(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt32 fmi3GetVariableStartUInt32(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt16 fmi3GetVariableStartUInt16(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt8 fmi3GetVariableStartUInt8(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Boolean fmi3GetVariableStartBoolean(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3String fmi3GetVariableStartString(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Binary fmi3GetVariableStartBinary(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3ValueReference fmi3GetVariableValueReference(fmi3VariableHandle* var);

FMI4C_DLLAPI const char* fmi3ModelName(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3InstantiationToken(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3Description(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3Author(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3Version(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3Copyright(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3License(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3GenerationTool(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3GenerationDateAndTime(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3VariableNamingConvention(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3SupportsModelExchange(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3SupportsScheduledExecution(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3SupportsCoSimulation(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3DefaultStartTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3DefaultStopTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3DefaultToleranceDefined(fmiHandle *fmu) ;
FMI4C_DLLAPI bool fmi3DefaultStepSizeDefined(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3GetDefaultStartTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3GetDefaultStopTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3GetDefaultTolerance(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3GetDefaultStepSize(fmiHandle *fmu);

FMI4C_DLLAPI int fmi3GetNumberOfUnits(fmiHandle *fmu);
FMI4C_DLLAPI fmi3UnitHandle *fmi3GetUnitByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI const char* fmi3GetUnitName(fmi3UnitHandle *unit);
FMI4C_DLLAPI bool fmi3HasBaseUnit(fmi3UnitHandle *unit);
FMI4C_DLLAPI void fmi3GetBaseUnit(fmi3UnitHandle *unit, double *factor, double *offset, int *kg, int *m, int *s, int *A, int *K, int *mol, int *cd, int *rad);
FMI4C_DLLAPI int fmi3GetNumberOfDisplayUnits(fmi3UnitHandle *unit);
FMI4C_DLLAPI void fmi3GetDisplayUnitByIndex(fmi3UnitHandle *unit, int id, const char **name, double *factor, double *offset, bool *inverse);

FMI4C_DLLAPI void fmi3GetFloat64Type(fmiHandle* fmu,
                                     const char* name,
                                     const char** description,
                                     const char** quantity,
                                     const char** unit,
                                     const char** displayUnit,
                                     bool* relativeQuantity,
                                     bool* unbounded,
                                     double* min,
                                     double* max,
                                     double* nominal);
FMI4C_DLLAPI void fmi3GetFloat32Type(fmiHandle* fmu,
                                     const char* name,
                                     const char** description,
                                     const char** quantity,
                                     const char** unit,
                                     const char** displayUnit,
                                     bool* relativeQuantity,
                                     bool* unbounded,
                                     double* min,
                                     double* max,
                                     double* nominal);
FMI4C_DLLAPI void fmi3GetInt64Type(fmiHandle *fmu,
                                   const char *name,
                                   const char** description,
                                   const char** quantity,
                                   double* min,
                                   double* max);
FMI4C_DLLAPI void fmi3GetInt32Type(fmiHandle *fmu,
                                   const char *name,
                                   const char** description,
                                   const char** quantity,
                                   double* min,
                                   double* max);
FMI4C_DLLAPI void fmi3GetInt16Type(fmiHandle *fmu,
                                   const char *name,
                                   const char** description,
                                   const char** quantity,
                                   double* min,
                                   double* max);
FMI4C_DLLAPI void fmi3GetInt8Type(fmiHandle *fmu,
                                  const char *name,
                                  const char** description,
                                  const char** quantity,
                                  double* min,
                                  double* max);
FMI4C_DLLAPI void fmi3GetUInt64Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    double* min,
                                    double* max);
FMI4C_DLLAPI void fmi3GetUInt32Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    double* min,
                                    double* max);
FMI4C_DLLAPI void fmi3GetUInt16Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    double* min,
                                    double* max);
FMI4C_DLLAPI void fmi3GetUInt8Type(fmiHandle *fmu,
                                   const char *name,
                                   const char** description,
                                   const char** quantity,
                                   double* min,
                                   double* max);
FMI4C_DLLAPI void fmi3GetBooleanType(fmiHandle *fmu,
                                     const char *name,
                                     const char **description);
FMI4C_DLLAPI void fmi3GetStringType(fmiHandle *fmu,
                                    const char *name,
                                    const char **description);
FMI4C_DLLAPI void fmi3GetBinaryType(fmiHandle *fmu,
                                    const char *name,
                                    const char **description,
                                    const char **mimeType,
                                    uint32_t *maxSize);
FMI4C_DLLAPI void fmi3GetEnumerationType(fmiHandle *fmu,
                                         const char *name,
                                         const char **description,
                                         const char **quantity,
                                         int64_t *min,
                                         int64_t *max,
                                         int *numberOfItems);
FMI4C_DLLAPI void fmi3GetEnumerationItem(fmiHandle *fmu,
                                         const char *typeName,
                                         int itemId,
                                         const char **itemName,
                                         int64_t *value,
                                         const char **description);
FMI4C_DLLAPI void fmi3GetClockType(fmiHandle *fmu,
                                   const char *name,
                                   const char **description,
                                   bool *canBeDeactivated,
                                   uint32_t *priority,
                                   fmi3IntervalVariability *intervalVariability,
                                   float *intervalDecimal,
                                   float *shiftDecimal,
                                   bool *supportsFraction,
                                   uint64_t *resolution,
                                   uint64_t *intervalCounter,
                                   uint64_t *shiftCounter);

FMI4C_DLLAPI int fmi3GetNumberOfLogCategories(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3GetLogCategory(fmiHandle *fmu, int id, const char **name, const char **description);

FMI4C_DLLAPI int fmi3GetNumberOfModelStructureOutputs(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3GetModelStructureOutput(fmiHandle *fmu,
                                              int id,
                                              fmi3ValueReference *vr,
                                              int *numberOfDependencies,
                                              bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureOutputDependency(fmiHandle *fmu,
                                                                      int outputId,
                                                                      int dependencyId,
                                                                      bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureOutputDependencyKind(fmiHandle *fmu,
                                                                          int outputId,
                                                                          int dependencyId,
                                                                          bool *ok);
FMI4C_DLLAPI int fmi3GetNumberOfModelStructureContinuousStateDerivatives(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3GetModelStructureContinuousStateDerivative(fmiHandle *fmu,
                                                                 int id,
                                                                 fmi3ValueReference *vr,
                                                                 int *numberOfDependencies,
                                                                 bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureContinuousStateDerivativeDependency(fmiHandle *fmu,
                                                                                         int derId,
                                                                                         int dependencyId,
                                                                                         bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureContinuousStateDerivativeDependencyKind(fmiHandle *fmu,
                                                                                             int derId,
                                                                                             int dependencyId,
                                                                                             bool *ok);
FMI4C_DLLAPI int fmi3GetNumberOfModelStructureClockedStates(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3GetModelStructureClockedState(fmiHandle *fmu,
                                                    int id,
                                                    fmi3ValueReference *vr,
                                                    int *numberOfDependencies,
                                                    bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureClockedStateDependency(fmiHandle *fmu,
                                                                            int clockId,
                                                                            int dependencyId,
                                                                            bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureClockedStateDependencyKind(fmiHandle *fmu,
                                                                                int clockId,
                                                                                int dependencyId,
                                                                                bool *ok);
FMI4C_DLLAPI int fmi3GetNumberOfModelStructureInitialUnknowns(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3GetModelStructureInitialUnknown(fmiHandle *fmu,
                                                      int id,
                                                      fmi3ValueReference *vr,
                                                      int *numberOfDependencies,
                                                      bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureInitialUnknownDependency(fmiHandle *fmu,
                                                                              int unknownId,
                                                                              int dependencyId,
                                                                              bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3ModelStructureGetInitialUnknownDependencyKind(fmiHandle *fmu,
                                                                                  int unknownId,
                                                                                  int dependencyId,
                                                                                  bool *ok);
FMI4C_DLLAPI int fmi3GetNumberOfModelStructureEventIndicators(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3GetModelStructureEventIndicator(fmiHandle *fmu,
                                                      int id,
                                                      fmi3ValueReference *vr,
                                                      int *numberOfDependencies,
                                                      bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureEventIndicatorDependency(fmiHandle *fmu,
                                                                              int indicatorId,
                                                                              int dependencyId,
                                                                              bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3GetModelStructureEventIndicatorDependencyKind(fmiHandle *fmu,
                                                                                  int indicatorId,
                                                                                  int dependencyId,
                                                                                  bool *ok);

FMI4C_DLLAPI const char* fmi3csGetModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetProvidesDirectionalDerivative(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetProvidesAdjointDerivatives(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetProvidesPerElementDependencies(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetProvidesIntermediateUpdate(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetProvidesEvaluateDiscreteStates(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetHasEventMode(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3csGetRecommendedIntermediateInputSmoothness(fmiHandle* fmu);

FMI4C_DLLAPI const char* fmi3meGetModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetProvidesDirectionalDerivative(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetProvidesAdjointDerivatives(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetProvidesPerElementDependencies(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetProvidesEvaluateDiscreteStates(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3meGetNeedsCompletedIntegratorStep(fmiHandle* fmu);

FMI4C_DLLAPI const char* fmi3seGetModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetProvidesDirectionalDerivative(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetProvidesAdjointDerivatives(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3seGetProvidesPerElementDependencies(fmiHandle* fmu);

FMI4C_DLLAPI int fmi3GetMaxOutputDerivativeOrder(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3GetCanHandleVariableCommunicationStepSize(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3GetCanReturnEarlyAfterIntermediateUpdate(fmiHandle* fmu);
FMI4C_DLLAPI double fmi3GetFixedInternalStepSize(fmiHandle* fmu);

FMI4C_DLLAPI bool fmi3InstantiateCoSimulation(fmiHandle *fmu,
                                              fmi3Boolean                    visible,
                                              fmi3Boolean                    loggingOn,
                                              fmi3Boolean                    eventModeUsed,
                                              fmi3Boolean                    earlyReturnAllowed,
                                              const fmi3ValueReference       requiredIntermediateVariables[],
                                              size_t                         nRequiredIntermediateVariables,
                                              fmi3InstanceEnvironment        instanceEnvironment,
                                              fmi3LogMessageCallback         logMessage,
                                              fmi3IntermediateUpdateCallback intermediateUpdate);

FMI4C_DLLAPI bool fmi3InstantiateModelExchange(fmiHandle *fmu,
                                               fmi3Boolean                visible,
                                               fmi3Boolean                loggingOn,
                                               fmi3InstanceEnvironment    instanceEnvironment,
                                               fmi3LogMessageCallback     logMessage);

FMI4C_DLLAPI const char* fmi3GetVersion(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3SetDebugLogging(fmiHandle *fmu,
                                            fmi3Boolean loggingOn,
                                            size_t nCategories,
                                            const fmi3String categories[]);

FMI4C_DLLAPI fmi3Status fmi3GetFloat64(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Float64 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetFloat64(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Float64 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3EnterInitializationMode(fmiHandle *fmu,
                                                    fmi3Boolean toleranceDefined,
                                                    fmi3Float64 tolerance,
                                                    fmi3Float64 startTime,
                                                    fmi3Boolean stopTimeDefined,
                                                    fmi3Float64 stopTime);

FMI4C_DLLAPI fmi3Status fmi3ExitInitializationMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3Terminate(fmiHandle *fmu);

FMI4C_DLLAPI void fmi3FreeInstance(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3DoStep(fmiHandle *fmu,
                                   fmi3Float64 currentCommunicationPoint,
                                   fmi3Float64 communicationStepSize,
                                   fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                   fmi3Boolean *eventEncountered,
                                   fmi3Boolean *terminateSimulation,
                                   fmi3Boolean *earlyReturn,
                                   fmi3Float64 *lastSuccessfulTime);

FMI4C_DLLAPI fmi3Status fmi3EnterEventMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3Reset(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3GetFloat32(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Float32 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetInt8(fmiHandle *fmu,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    fmi3Int8 values[],
                                    size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetUInt8(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3UInt8 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetInt16(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Int16 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetUInt16(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt16 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetInt32(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Int32 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetUInt32(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt32 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetInt64(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Int64 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetUInt64(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt64 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetBoolean(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Boolean values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetString(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3String values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetBinary(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      size_t valueSizes[],
                                      fmi3Binary values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3GetClock(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Clock values[]);

FMI4C_DLLAPI fmi3Status fmi3SetFloat32(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       const fmi3Float32 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetInt8(fmiHandle *fmu,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Int8 values[],
                                    size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetUInt8(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3UInt8 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetInt16(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Int16 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetUInt16(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt16 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetInt32(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Int32 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetUInt32(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt32 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetInt64(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Int64 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetUInt64(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt64 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetBoolean(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       const fmi3Boolean values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetString(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3String values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetBinary(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const size_t valueSizes[],
                                      const fmi3Binary values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3SetClock(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Clock values[]);

FMI4C_DLLAPI fmi3Status fmi3GetNumberOfVariableDependencies(fmiHandle *fmu,
                                                            fmi3ValueReference valueReference,
                                                            size_t* nDependencies);

FMI4C_DLLAPI fmi3Status fmi3GetVariableDependencies(fmiHandle *fmu,
                                                    fmi3ValueReference dependent,
                                                    size_t elementIndicesOfDependent[],
                                                    fmi3ValueReference independents[],
                                                    size_t elementIndicesOfIndependents[],
                                                    fmi3DependencyKind dependencyKinds[],
                                                    size_t nDependencies);

FMI4C_DLLAPI fmi3Status fmi3GetFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);

FMI4C_DLLAPI fmi3Status fmi3SetFMUState(fmiHandle *fmu, fmi3FMUState  FMUState);

FMI4C_DLLAPI fmi3Status fmi3FreeFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);

FMI4C_DLLAPI fmi3Status fmi3SerializedFMUStateSize(fmiHandle *fmu,
                                                   fmi3FMUState  FMUState,
                                                   size_t* size);

FMI4C_DLLAPI fmi3Status fmi3SerializeFMUState(fmiHandle *fmu,
                                              fmi3FMUState  FMUState,
                                              fmi3Byte serializedState[],
                                              size_t size);

FMI4C_DLLAPI fmi3Status fmi3DeserializeFMUState(fmiHandle *fmu,
                                                const fmi3Byte serializedState[],
                                                size_t size,
                                                fmi3FMUState* FMUState);

FMI4C_DLLAPI fmi3Status fmi3GetDirectionalDerivative(fmiHandle *fmu,
                                                     const fmi3ValueReference unknowns[],
                                                     size_t nUnknowns,
                                                     const fmi3ValueReference knowns[],
                                                     size_t nKnowns,
                                                     const fmi3Float64 seed[],
                                                     size_t nSeed,
                                                     fmi3Float64 sensitivity[],
                                                     size_t nSensitivity);

FMI4C_DLLAPI fmi3Status fmi3GetAdjointDerivative(fmiHandle *fmu,
                                                 const fmi3ValueReference unknowns[],
                                                 size_t nUnknowns,
                                                 const fmi3ValueReference knowns[],
                                                 size_t nKnowns,
                                                 const fmi3Float64 seed[],
                                                 size_t nSeed,
                                                 fmi3Float64 sensitivity[],
                                                 size_t nSensitivity);

FMI4C_DLLAPI fmi3Status fmi3EnterConfigurationMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3ExitConfigurationMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3GetIntervalDecimal(fmiHandle *fmu,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               fmi3Float64 intervals[],
                                               fmi3IntervalQualifier qualifiers[]);

FMI4C_DLLAPI fmi3Status fmi3GetIntervalFraction(fmiHandle *fmu,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                fmi3UInt64 intervalCounters[],
                                                fmi3UInt64 resolutions[],
                                                fmi3IntervalQualifier qualifiers[]);

FMI4C_DLLAPI fmi3Status fmi3GetShiftDecimal(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3Float64 shifts[]);

FMI4C_DLLAPI fmi3Status fmi3GetShiftFraction(fmiHandle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             fmi3UInt64 shiftCounters[],
                                             fmi3UInt64 resolutions[]);

FMI4C_DLLAPI fmi3Status fmi3SetIntervalDecimal(fmiHandle *fmu,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               const fmi3Float64 intervals[]);

FMI4C_DLLAPI fmi3Status fmi3SetIntervalFraction(fmiHandle *fmu,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                const fmi3UInt64 intervalCounters[],
                                                const fmi3UInt64 resolutions[]);

FMI4C_DLLAPI fmi3Status fmi3EvaluateDiscreteStates(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3UpdateDiscreteStates(fmiHandle *fmu,
                                                 fmi3Boolean* discreteStatesNeedUpdate,
                                                 fmi3Boolean* terminateSimulation,
                                                 fmi3Boolean* nominalsOfContinuousStatesChanged,
                                                 fmi3Boolean* valuesOfContinuousStatesChanged,
                                                 fmi3Boolean* nextEventTimeDefined,
                                                 fmi3Float64* nextEventTime);

FMI4C_DLLAPI fmi3Status fmi3EnterContinuousTimeMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3CompletedIntegratorStep(fmiHandle *fmu,
                                                    fmi3Boolean  noSetFMUStatePriorToCurrentPoint,
                                                    fmi3Boolean* enterEventMode,
                                                    fmi3Boolean* terminateSimulation);

FMI4C_DLLAPI fmi3Status fmi3SetTime(fmiHandle *fmu, fmi3Float64 time);

FMI4C_DLLAPI fmi3Status fmi3SetContinuousStates(fmiHandle *fmu,
                                                const fmi3Float64 continuousStates[],
                                                size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3GetContinuousStateDerivatives(fmiHandle *fmu,
                                                          fmi3Float64 derivatives[],
                                                          size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3GetEventIndicators(fmiHandle *fmu,
                                               fmi3Float64 eventIndicators[],
                                               size_t nEventIndicators);

FMI4C_DLLAPI fmi3Status fmi3GetContinuousStates(fmiHandle *fmu,
                                                fmi3Float64 continuousStates[],
                                                size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3GetNominalsOfContinuousStates(fmiHandle *fmu,
                                                          fmi3Float64 nominals[],
                                                          size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3GetNumberOfEventIndicators(fmiHandle *fmu,
                                                       size_t* nEventIndicators);

FMI4C_DLLAPI fmi3Status fmi3GetNumberOfContinuousStates(fmiHandle *fmu,
                                                        size_t* nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3EnterStepMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3GetOutputDerivatives(fmiHandle *fmu,
                                                 const fmi3ValueReference valueReferences[],
                                                 size_t nValueReferences,
                                                 const fmi3Int32 orders[],
                                                 fmi3Float64 values[],
                                                 size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3ActivateModelPartition(fmiHandle *fmu,
                                                   fmi3ValueReference clockReference,
                                                   fmi3Float64 activationTime);

#ifdef __cplusplus
}
#endif


#endif // FMIC_H
