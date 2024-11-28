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

FMI4C_DLLAPI fmiVersion_t fmi4c_getFmiVersion(fmiHandle *fmu);
FMI4C_DLLAPI fmiHandle *fmi4c_loadUnzippedFmu(const char *instanceName, const char *unzipLocation);
FMI4C_DLLAPI fmiHandle* fmi4c_loadFmu(const char *fmufile, const char* instanceName);
FMI4C_DLLAPI void fmi4c_freeFmu(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi4c_getErrorMessages();

// FMI 1 wrapper functions
FMI4C_DLLAPI fmi1Type fmi1_getType(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getModelName(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getModelIdentifier(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getGuid(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getDescription(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getAuthor(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getGenerationTool(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi1_getGenerationDateAndTime(fmiHandle *fmu);
FMI4C_DLLAPI int fmi1_getNumberOfContinuousStates(fmiHandle *fmu);
FMI4C_DLLAPI int fmi1_getNumberOfEventIndicators(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi1_defaultStartTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi1_defaultStopTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi1_defaultToleranceDefined(fmiHandle *fmu) ;
FMI4C_DLLAPI double fmi1_getDefaultStartTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi1_getDefaultStopTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi1_getDefaultTolerance(fmiHandle *fmu);

FMI4C_DLLAPI int fmi1_getNumberOfVariables(fmiHandle *fmu);
FMI4C_DLLAPI fmi1VariableHandle* fmi1_getVariableByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI fmi1VariableHandle* fmi1_getVariableByValueReference(fmiHandle *fmu, fmi1ValueReference vr);
FMI4C_DLLAPI fmi1VariableHandle* fmi1_getVariableByName(fmiHandle *fmu, fmi1String name);
FMI4C_DLLAPI const char* fmi1_getVariableName(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1_getVariableDescription(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1_getVariableQuantity(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1_getVariableUnit(fmi1VariableHandle* var);
FMI4C_DLLAPI const char* fmi1_getVariableDisplayUnit(fmi1VariableHandle* var);
FMI4C_DLLAPI bool fmi1_getVariableRelativeQuantity(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Real fmi1_getVariableMin(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Real fmi1_getVariableMax(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Real fmi1_getVariableNominal(fmi1VariableHandle* var);
FMI4C_DLLAPI bool fmi1_getVariableHasStartValue(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Real fmi1_getVariableStartReal(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Integer fmi1_getVariableStartInteger(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Boolean fmi1_getVariableStartBoolean(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1String fmi1_getVariableStartString(fmi1VariableHandle* var);
FMI4C_DLLAPI long fmi1_getVariableValueReference(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Causality fmi1_getVariableCausality(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Variability fmi1_getVariableVariability(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1Alias fmi1_getAlias(fmi1VariableHandle* var);
FMI4C_DLLAPI bool fmi1_getVariableIsFixed(fmi1VariableHandle* var);
FMI4C_DLLAPI fmi1DataType fmi1_getVariableDataType(fmi1VariableHandle* var);

FMI4C_DLLAPI const char* fmi1_getTypesPlatform(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi1_getVersion(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1_setDebugLogging(fmiHandle* fmu, fmi1Boolean loggingOn);

FMI4C_DLLAPI int fmi1_getNumberOfBaseUnits(fmiHandle *fmu);
FMI4C_DLLAPI fmi1BaseUnitHandle *fmi1_getBaseUnitByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI const char* fmi1_getBaseUnitUnit(fmi1BaseUnitHandle *baseUnit);
FMI4C_DLLAPI int fmi1_getNumberOfDisplayUnits(fmi1BaseUnitHandle *baseUnit);
FMI4C_DLLAPI void fmi1_getDisplayUnitByIndex(fmi1BaseUnitHandle *baseUnit, int id, const char **displayUnit, double *gain, double *offset);

FMI4C_DLLAPI fmi1Status fmi1_getReal(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1_getInteger(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Integer values[]);
FMI4C_DLLAPI fmi1Status fmi1_getBoolean(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1Boolean values[]);
FMI4C_DLLAPI fmi1Status fmi1_getString(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, fmi1String values[]);

FMI4C_DLLAPI fmi1Status fmi1_setReal(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1_setInteger(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer values[]);
FMI4C_DLLAPI fmi1Status fmi1_setBoolean(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Boolean values[]);
FMI4C_DLLAPI fmi1Status fmi1_setString(fmiHandle* fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1String values[]);

FMI4C_DLLAPI bool fmi1_instantiateSlave(fmiHandle *fmu, fmi1String mimeType, fmi1Real timeOut, fmi1Boolean visible, fmi1Boolean interactive, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1StepFinished_t stepFinished, fmi1Boolean loggingOn);
FMI4C_DLLAPI fmi1Status fmi1_initializeSlave(fmiHandle* fmu, fmi1Real startTime, fmi1Boolean stopTimeDefined, fmi1Real stopTime);
FMI4C_DLLAPI fmi1Status fmi1_terminateSlave(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1_resetSlave(fmiHandle* fmu);
FMI4C_DLLAPI void fmi1_freeSlaveInstance(fmiHandle* fmu);

FMI4C_DLLAPI fmi1Status fmi1_setRealInputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], const fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1_getRealOutputDerivatives(fmiHandle *fmu, const fmi1ValueReference valueReferences[], size_t nValueReferences, const fmi1Integer orders[], fmi1Real values[]);
FMI4C_DLLAPI fmi1Status fmi1_cancelStep(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1_doStep(fmiHandle *fmu, fmi1Real currentCommunicationPoint, fmi1Real communicationStepSize, fmi1Boolean newStep);
FMI4C_DLLAPI fmi1Status fmi1_getStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Status* value);
FMI4C_DLLAPI fmi1Status fmi1_getRealStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Real* value);
FMI4C_DLLAPI fmi1Status fmi1_getIntegerStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Integer* value);
FMI4C_DLLAPI fmi1Status fmi1_getBooleanStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1Boolean* value);
FMI4C_DLLAPI fmi1Status fmi1_getStringStatus(fmiHandle* fmu, const fmi1StatusKind statusKind, fmi1String* value);

FMI4C_DLLAPI const char *fmi1_getModelTypesPlatform(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi1_instantiateModel(fmiHandle *fmu, fmi1CallbackLogger_t logger, fmi1CallbackAllocateMemory_t allocateMemory, fmi1CallbackFreeMemory_t freeMemory, fmi1Boolean loggingOn);
FMI4C_DLLAPI void fmi1_freeModelInstance(fmiHandle* fmu);
FMI4C_DLLAPI fmi1Status fmi1_setTime(fmiHandle* fmu, fmi1Real time);
FMI4C_DLLAPI fmi1Status fmi1_setContinuousStates(fmiHandle *fmu, const fmi1Real values[], size_t nStates);
FMI4C_DLLAPI fmi1Status fmi1_completedIntegratorStep(fmiHandle* fmu, fmi1Boolean* callEventUpdate);
FMI4C_DLLAPI fmi1Status fmi1_initialize(fmiHandle *fmu, fmi1Boolean toleranceControlled, fmi1Real relativeTolerance, fmi1EventInfo *eventInfo);
FMI4C_DLLAPI fmi1Status fmi1_getDerivatives(fmiHandle *fmu, fmi1Real derivatives[], size_t nDerivatives);
FMI4C_DLLAPI fmi1Status fmi1_getEventIndicators(fmiHandle *fmu, fmi1Real indicators[], size_t nIndicators);
FMI4C_DLLAPI fmi1Status fmi1_eventUpdate(fmiHandle *fmu, fmi1Boolean intermediateResults, fmi1EventInfo *eventInfo);
FMI4C_DLLAPI fmi1Status fmi1_getContinuousStates(fmiHandle *fmu, fmi1Real states[], size_t nStates);
FMI4C_DLLAPI fmi1Status fmi1_getNominalContinuousStates(fmiHandle* fmu, fmi1Real nominals[], size_t nNominals);
FMI4C_DLLAPI fmi1Status fmi1_getStateValueReferences(fmiHandle* fmu, fmi1ValueReference valueReferences[], size_t nValueReferences);
FMI4C_DLLAPI fmi1Status fmi1_terminate(fmiHandle* fmu);

// FMI 2 wrapper functions

FMI4C_DLLAPI bool fmi2_defaultStartTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2_defaultStopTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2_defaultToleranceDefined(fmiHandle *fmu) ;
FMI4C_DLLAPI bool fmi2_defaultStepSizeDefined(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2_getDefaultStartTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2_getDefaultStopTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2_getDefaultTolerance(fmiHandle *fmu);
FMI4C_DLLAPI double fmi2_getDefaultStepSize(fmiHandle *fmu);

FMI4C_DLLAPI int fmi2_getNumberOfVariables(fmiHandle *fmu);
FMI4C_DLLAPI fmi2VariableHandle* fmi2_getVariableByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI fmi2VariableHandle* fmi2_getVariableByValueReference(fmiHandle *fmu, fmi2ValueReference vr);
FMI4C_DLLAPI fmi2VariableHandle* fmi2_getVariableByName(fmiHandle *fmu, fmi2String name);
FMI4C_DLLAPI const char* fmi2_getVariableName(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2_getVariableDescription(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2_getFmiVersion(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi2_getAuthor(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getModelName(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getModelDescription(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getCopyright(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getLicense(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getGenerationTool(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getGenerationDateAndTime(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi2_getVariableNamingConvention(fmiHandle *fmu);
FMI4C_DLLAPI int fmi2_getVariableDerivativeIndex(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2_getVariableQuantity(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2_getVariableUnit(fmi2VariableHandle* var);
FMI4C_DLLAPI const char* fmi2_getVariableDisplayUnit(fmi2VariableHandle* var);
FMI4C_DLLAPI bool fmi2_getVariableRelativeQuantity(fmi2VariableHandle *var);
FMI4C_DLLAPI fmi2Real fmi2_getVariableMin(fmi2VariableHandle *var);
FMI4C_DLLAPI fmi2Real fmi2_getVariableMax(fmi2VariableHandle *var);
FMI4C_DLLAPI fmi2Real fmi2_getVariableNominal(fmi2VariableHandle *var);
FMI4C_DLLAPI bool fmi2_getVariableUnbounded(fmi2VariableHandle *var);
FMI4C_DLLAPI bool fmi2_getVariableHasStartValue(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Real fmi2_getVariableStartReal(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Integer fmi2_getVariableStartInteger(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Boolean fmi2_getVariableStartBoolean(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2String fmi2_getVariableStartString(fmi2VariableHandle* var);
FMI4C_DLLAPI long fmi2_getVariableValueReference(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Causality fmi2_getVariableCausality(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Variability fmi2_getVariableVariability(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2Initial fmi2_getVariableInitial(fmi2VariableHandle* var);
FMI4C_DLLAPI bool fmi2_getVariableCanHandleMultipleSetPerTimeInstant(fmi2VariableHandle* var);
FMI4C_DLLAPI fmi2DataType fmi2_getVariableDataType(fmi2VariableHandle* var);

FMI4C_DLLAPI const char* fmi2_getTypesPlatform(fmiHandle* fmu);
FMI4C_DLLAPI const char* fmi2_getVersion(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2_setDebugLogging(fmiHandle *fmu, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[]);
FMI4C_DLLAPI const char* fmi2_getGuid(fmiHandle *fmu);

FMI4C_DLLAPI const char* fmi2cs_getModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanHandleVariableCommunicationStepSize(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanInterpolateInputs(fmiHandle* fmu);
FMI4C_DLLAPI int fmi2cs_getMaxOutputDerivativeOrder(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanRunAsynchronuously(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanNotUseMemoryManagementFunctions(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2cs_getProvidesDirectionalDerivative(fmiHandle* fmu);

FMI4C_DLLAPI const char* fmi2me_getModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getCompletedIntegratorStepNotNeeded(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getCanNotUseMemoryManagementFunctions(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi2me_getProvidesDirectionalDerivative(fmiHandle* fmu);

FMI4C_DLLAPI int fmi2_getNumberOfContinuousStates(fmiHandle *fmu);
FMI4C_DLLAPI int fmi2_getNumberOfEventIndicators(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2_getSupportsCoSimulation(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi2_getSupportsModelExchange(fmiHandle *fmu);

FMI4C_DLLAPI bool fmi2_instantiate(fmiHandle *fmu, fmi2Type type, fmi2CallbackLogger logger, fmi2CallbackAllocateMemory allocateMemory, fmi2CallbackFreeMemory freeMemory, fmi2StepFinished stepFinished, fmi2ComponentEnvironment componentEnvironment, fmi2Boolean visible, fmi2Boolean loggingOn);
FMI4C_DLLAPI void fmi2_freeInstance(fmiHandle* fmu);

FMI4C_DLLAPI fmi2Status fmi2_setupExperiment(fmiHandle *fmu, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime);
FMI4C_DLLAPI fmi2Status fmi2_enterInitializationMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2_exitInitializationMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2_terminate(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2_reset(fmiHandle* fmu);

FMI4C_DLLAPI int fmi2_getNumberOfUnits(fmiHandle *fmu);
FMI4C_DLLAPI fmi2UnitHandle *fmi2_getUnitByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI const char* fmi2_getUnitName(fmi2UnitHandle *unit);
FMI4C_DLLAPI bool fmi2_hasBaseUnit(fmi2UnitHandle *unit);
FMI4C_DLLAPI void fmi2_getBaseUnit(fmi2UnitHandle *unit, double *factor, double *offset, int *kg, int *m, int *s, int *A, int *K, int *mol, int *cd, int *rad);
FMI4C_DLLAPI int fmi2_getNumberOfDisplayUnits(fmi2UnitHandle *unit);
FMI4C_DLLAPI void fmi2_getDisplayUnitByIndex(fmi2UnitHandle *unit, int id, const char **name, double *factor, double *offset);

FMI4C_DLLAPI fmi2Status fmi2_getReal(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Real values[]);
FMI4C_DLLAPI fmi2Status fmi2_getInteger(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Integer values[]);
FMI4C_DLLAPI fmi2Status fmi2_getBoolean(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2Boolean values[]);
FMI4C_DLLAPI fmi2Status fmi2_getString(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, fmi2String values[]);

FMI4C_DLLAPI fmi2Status fmi2_setReal(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Real values[]);
FMI4C_DLLAPI fmi2Status fmi2_setInteger(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Integer values[]);
FMI4C_DLLAPI fmi2Status fmi2_setBoolean(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2Boolean values[]);
FMI4C_DLLAPI fmi2Status fmi2_setString(fmiHandle* fmu, const fmi2ValueReference valueReferences[], size_t nValueReferences, const fmi2String values[]);

FMI4C_DLLAPI fmi2Status fmi2_getFMUstate(fmiHandle* fmu,
                                         fmi2FMUstate* FMUstate);
FMI4C_DLLAPI fmi2Status fmi2_setFMUstate(fmiHandle* fmu,
                                         fmi2FMUstate FMUstate);
FMI4C_DLLAPI fmi2Status fmi2_freeFMUstate(fmiHandle* fmu,
                                          fmi2FMUstate* FMUstate);
FMI4C_DLLAPI fmi2Status fmi2_serializedFMUstateSize(fmiHandle* fmu,
                                                    fmi2FMUstate FMUstate,
                                                    size_t* size);
FMI4C_DLLAPI fmi2Status fmi2_serializeFMUstate(fmiHandle* fmu,
                                               fmi2FMUstate FMUstate,
                                               fmi2Byte serializedState[],
                                               size_t size);
FMI4C_DLLAPI fmi2Status fmi2_deSerializeFMUstate(fmiHandle* fmu,
                                                 const fmi2Byte serializedState[],
                                                 size_t size,
                                                 fmi2FMUstate* FMUstate);

FMI4C_DLLAPI fmi2Status fmi2_getDirectionalDerivative(fmiHandle* fmu,
                                                      const fmi2ValueReference unknownReferences[],
                                                      size_t nUnknown,
                                                      const fmi2ValueReference knownReferences[],
                                                      size_t nKnown,
                                                      const fmi2Real dvKnown[],
                                                      fmi2Real dvUnknown[]);

FMI4C_DLLAPI fmi2Status fmi2_enterEventMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2_newDiscreteStates(fmiHandle* fmu, fmi2EventInfo* eventInfo);
FMI4C_DLLAPI fmi2Status fmi2_enterContinuousTimeMode(fmiHandle* fmu);
FMI4C_DLLAPI fmi2Status fmi2_completedIntegratorStep(fmiHandle* fmu,
                                                     fmi2Boolean noSetFMUStatePriorToCurrentPoint,
                                                     fmi2Boolean* enterEventMode,
                                                     fmi2Boolean* terminateSimulation);

FMI4C_DLLAPI fmi2Status fmi2_setTime(fmiHandle* fmu,
                                     fmi2Real time);
FMI4C_DLLAPI fmi2Status fmi2_setContinuousStates(fmiHandle* fmu,
                                                 const fmi2Real x[],
                                                 size_t nx);

FMI4C_DLLAPI fmi2Status fmi2_getDerivatives(fmiHandle* fmu,
                                            fmi2Real derivatives[],
                                            size_t nx);
FMI4C_DLLAPI fmi2Status fmi2_getEventIndicators(fmiHandle* fmu,
                                                fmi2Real eventIndicators[],
                                                size_t ni);
FMI4C_DLLAPI fmi2Status fmi2_getContinuousStates(fmiHandle* fmu,
                                                 fmi2Real x[],
                                                 size_t nx);
FMI4C_DLLAPI fmi2Status fmi2_getNominalsOfContinuousStates(fmiHandle* fmu,
                                                           fmi2Real x_nominal[],
                                                           size_t nx);

FMI4C_DLLAPI fmi2Status fmi2_setRealInputDerivatives(fmiHandle* fmu,
                                                     const fmi2ValueReference vr[],
                                                     size_t nvr,
                                                     const fmi2Integer order[],
                                                     const fmi2Real value[]);
FMI4C_DLLAPI fmi2Status fmi2_getRealOutputDerivatives (fmiHandle* fmu,
                                                      const fmi2ValueReference vr[],
                                                      size_t nvr,
                                                      const fmi2Integer order[],
                                                      fmi2Real value[]);

FMI4C_DLLAPI fmi2Status fmi2_doStep(fmiHandle *fmu, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint);
FMI4C_DLLAPI fmi2Status fmi2_cancelStep(fmiHandle* fmu);

FMI4C_DLLAPI fmi2Status fmi2_getStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Status* value);
FMI4C_DLLAPI fmi2Status fmi2_getRealStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Real* value);
FMI4C_DLLAPI fmi2Status fmi2_getIntegerStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Integer* value);
FMI4C_DLLAPI fmi2Status fmi2_getBooleanStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2Boolean* value);
FMI4C_DLLAPI fmi2Status fmi2_getStringStatus(fmiHandle* fmu, const fmi2StatusKind s, fmi2String* value);


FMI4C_DLLAPI int fmi3_getNumberOfVariables(fmiHandle *fmu);
FMI4C_DLLAPI fmi3VariableHandle* fmi3_getVariableByName(fmiHandle *fmu, fmi3String name);
FMI4C_DLLAPI fmi3VariableHandle* fmi3_getVariableByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI fmi3VariableHandle* fmi3_getVariableByValueReference(fmiHandle *fmu, fmi3ValueReference vr);
FMI4C_DLLAPI const char* fmi3_getVariableName(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Causality fmi3_getVariableCausality(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Variability fmi3_getVariableVariability(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Initial fmi3_getVariableInitial(fmi3VariableHandle* var);
FMI4C_DLLAPI bool fmi3_getVariableSupportsIntermediateUpdate(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3DataType fmi3_getVariableDataType(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3_getVariableDescription(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3_getVariableQuantity(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3_getVariableUnit(fmi3VariableHandle* var);
FMI4C_DLLAPI const char *fmi3_getVariableDisplayUnit(fmi3VariableHandle* var);
FMI4C_DLLAPI bool fmi3_getVariableHasStartValue(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Float64 fmi3_getVariableStartFloat64(fmi3VariableHandle* var);
FMI4C_DLLAPI fmi3Float32 fmi3_getVariableStartFloat32(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int64 fmi3_getVariableStartInt64(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int32 fmi3_getVariableStartInt32(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int16 fmi3_getVariableStartInt16(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Int8 fmi3_getVariableStartInt8(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt64 fmi3_getVariableStartUInt64(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt32 fmi3_getVariableStartUInt32(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt16 fmi3_getVariableStartUInt16(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3UInt8 fmi3_getVariableStartUInt8(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Boolean fmi3_getVariableStartBoolean(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3String fmi3_getVariableStartString(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3Binary fmi3_getVariableStartBinary(fmi3VariableHandle *var);
FMI4C_DLLAPI fmi3ValueReference fmi3_getVariableValueReference(fmi3VariableHandle* var);

FMI4C_DLLAPI const char* fmi3_modelName(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_instantiationToken(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_description(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_author(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_version(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_copyright(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_license(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_generationTool(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_generationDateAndTime(fmiHandle *fmu);
FMI4C_DLLAPI const char* fmi3_variableNamingConvention(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3_supportsModelExchange(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3_supportsScheduledExecution(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3_supportsCoSimulation(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3_defaultStartTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3_defaultStopTimeDefined(fmiHandle *fmu);
FMI4C_DLLAPI bool fmi3_defaultToleranceDefined(fmiHandle *fmu) ;
FMI4C_DLLAPI bool fmi3_defaultStepSizeDefined(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3_getDefaultStartTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3_getDefaultStopTime(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3_getDefaultTolerance(fmiHandle *fmu);
FMI4C_DLLAPI double fmi3_getDefaultStepSize(fmiHandle *fmu);

FMI4C_DLLAPI int fmi3_getNumberOfUnits(fmiHandle *fmu);
FMI4C_DLLAPI fmi3UnitHandle *fmi3_getUnitByIndex(fmiHandle *fmu, int i);
FMI4C_DLLAPI const char* fmi3_getUnitName(fmi3UnitHandle *unit);
FMI4C_DLLAPI bool fmi3_hasBaseUnit(fmi3UnitHandle *unit);
FMI4C_DLLAPI void fmi3_getBaseUnit(fmi3UnitHandle *unit, double *factor, double *offset, int *kg, int *m, int *s, int *A, int *K, int *mol, int *cd, int *rad);
FMI4C_DLLAPI int fmi3_getNumberOfDisplayUnits(fmi3UnitHandle *unit);
FMI4C_DLLAPI void fmi3_getDisplayUnitByIndex(fmi3UnitHandle *unit, int id, const char **name, double *factor, double *offset, bool *inverse);

FMI4C_DLLAPI void fmi3_getFloat64Type(fmiHandle* fmu,
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
FMI4C_DLLAPI void fmi3_getFloat32Type(fmiHandle* fmu,
                                      const char* name,
                                      const char** description,
                                      const char** quantity,
                                      const char** unit,
                                      const char** displayUnit,
                                      bool *relativeQuantity,
                                      bool *unbounded,
                                      float *min,
                                      float *max,
                                      float *nominal);
FMI4C_DLLAPI void fmi3_getInt64Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    int64_t* min,
                                    int64_t* max);
FMI4C_DLLAPI void fmi3_getInt32Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    int32_t* min,
                                    int32_t* max);
FMI4C_DLLAPI void fmi3_getInt16Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    int16_t* min,
                                    int16_t* max);
FMI4C_DLLAPI void fmi3_getInt8Type(fmiHandle *fmu,
                                   const char *name,
                                   const char** description,
                                   const char** quantity,
                                   int8_t* min,
                                   int8_t* max);
FMI4C_DLLAPI void fmi3_getUInt64Type(fmiHandle *fmu,
                                     const char *name,
                                     const char** description,
                                     const char** quantity,
                                     uint64_t* min,
                                     uint64_t* max);
FMI4C_DLLAPI void fmi3_getUInt32Type(fmiHandle *fmu,
                                     const char *name,
                                     const char** description,
                                     const char** quantity,
                                     uint32_t* min,
                                     uint32_t* max);
FMI4C_DLLAPI void fmi3_getUInt16Type(fmiHandle *fmu,
                                     const char *name,
                                     const char** description,
                                     const char** quantity,
                                     uint16_t* min,
                                     uint16_t* max);
FMI4C_DLLAPI void fmi3_getUInt8Type(fmiHandle *fmu,
                                    const char *name,
                                    const char** description,
                                    const char** quantity,
                                    uint8_t* min,
                                    uint8_t* max);
FMI4C_DLLAPI void fmi3_getBooleanType(fmiHandle *fmu,
                                      const char *name,
                                      const char **description);
FMI4C_DLLAPI void fmi3_getStringType(fmiHandle *fmu,
                                     const char *name,
                                     const char **description);
FMI4C_DLLAPI void fmi3_getBinaryType(fmiHandle *fmu,
                                     const char *name,
                                     const char **description,
                                     const char **mimeType,
                                     uint32_t *maxSize);
FMI4C_DLLAPI void fmi3_getEnumerationType(fmiHandle *fmu,
                                          const char *name,
                                          const char **description,
                                          const char **quantity,
                                          int64_t *min,
                                          int64_t *max,
                                          int *numberOfItems);
FMI4C_DLLAPI void fmi3_getEnumerationItem(fmiHandle *fmu,
                                          const char *typeName,
                                          int itemId,
                                          const char **itemName,
                                          int64_t *value,
                                          const char **description);
FMI4C_DLLAPI void fmi3_getClockType(fmiHandle *fmu,
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

FMI4C_DLLAPI int fmi3_getNumberOfLogCategories(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3_getLogCategory(fmiHandle *fmu, int id, const char **name, const char **description);

FMI4C_DLLAPI int fmi3_getNumberOfModelStructureOutputs(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3_getModelStructureOutput(fmiHandle *fmu,
                                               int id,
                                               fmi3ValueReference *vr,
                                               int *numberOfDependencies,
                                               bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureOutputDependency(fmiHandle *fmu,
                                                                       int outputId,
                                                                       int dependencyId,
                                                                       bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureOutputDependencyKind(fmiHandle *fmu,
                                                                           int outputId,
                                                                           int dependencyId,
                                                                           bool *ok);
FMI4C_DLLAPI int fmi3_getNumberOfModelStructureContinuousStateDerivatives(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3_getModelStructureContinuousStateDerivative(fmiHandle *fmu,
                                                                  int id,
                                                                  fmi3ValueReference *vr,
                                                                  int *numberOfDependencies,
                                                                  bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureContinuousStateDerivativeDependency(fmiHandle *fmu,
                                                                                          int derId,
                                                                                          int dependencyId,
                                                                                          bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureContinuousStateDerivativeDependencyKind(fmiHandle *fmu,
                                                                                              int derId,
                                                                                              int dependencyId,
                                                                                              bool *ok);
FMI4C_DLLAPI int fmi3_getNumberOfModelStructureClockedStates(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3_getModelStructureClockedState(fmiHandle *fmu,
                                                     int id,
                                                     fmi3ValueReference *vr,
                                                     int *numberOfDependencies,
                                                     bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureClockedStateDependency(fmiHandle *fmu,
                                                                             int clockId,
                                                                             int dependencyId,
                                                                             bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureClockedStateDependencyKind(fmiHandle *fmu,
                                                                                 int clockId,
                                                                                 int dependencyId,
                                                                                 bool *ok);
FMI4C_DLLAPI int fmi3_getNumberOfModelStructureInitialUnknowns(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3_getModelStructureInitialUnknown(fmiHandle *fmu,
                                                       int id,
                                                       fmi3ValueReference *vr,
                                                       int *numberOfDependencies,
                                                       bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureInitialUnknownDependency(fmiHandle *fmu,
                                                                               int unknownId,
                                                                               int dependencyId,
                                                                               bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3_modelStructureGetInitialUnknownDependencyKind(fmiHandle *fmu,
                                                                                   int unknownId,
                                                                                   int dependencyId,
                                                                                   bool *ok);
FMI4C_DLLAPI int fmi3_getNumberOfModelStructureEventIndicators(fmiHandle *fmu);
FMI4C_DLLAPI void fmi3_getModelStructureEventIndicator(fmiHandle *fmu,
                                                       int id,
                                                       fmi3ValueReference *vr,
                                                       int *numberOfDependencies,
                                                       bool *dependencyKindsDefined);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureEventIndicatorDependency(fmiHandle *fmu,
                                                                               int indicatorId,
                                                                               int dependencyId,
                                                                               bool *ok);
FMI4C_DLLAPI fmi3ValueReference fmi3_getModelStructureEventIndicatorDependencyKind(fmiHandle *fmu,
                                                                                   int indicatorId,
                                                                                   int dependencyId,
                                                                                   bool *ok);

FMI4C_DLLAPI const char* fmi3cs_getModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getProvidesDirectionalDerivative(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getProvidesAdjointDerivatives(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getProvidesPerElementDependencies(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getProvidesIntermediateUpdate(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getProvidesEvaluateDiscreteStates(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getHasEventMode(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getRecommendedIntermediateInputSmoothness(fmiHandle* fmu);
FMI4C_DLLAPI int fmi3cs_getMaxOutputDerivativeOrder(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getCanHandleVariableCommunicationStepSize(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3cs_getCanReturnEarlyAfterIntermediateUpdate(fmiHandle* fmu);
FMI4C_DLLAPI double fmi3cs_getFixedInternalStepSize(fmiHandle* fmu);

FMI4C_DLLAPI const char* fmi3me_getModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getProvidesDirectionalDerivative(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getProvidesAdjointDerivatives(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getProvidesPerElementDependencies(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getProvidesEvaluateDiscreteStates(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3me_getNeedsCompletedIntegratorStep(fmiHandle* fmu);

FMI4C_DLLAPI const char* fmi3se_getModelIdentifier(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getNeedsExecutionTool(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getCanBeInstantiatedOnlyOncePerProcess(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getCanGetAndSetFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getCanSerializeFMUState(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getProvidesDirectionalDerivative(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getProvidesAdjointDerivatives(fmiHandle* fmu);
FMI4C_DLLAPI bool fmi3se_getProvidesPerElementDependencies(fmiHandle* fmu);

FMI4C_DLLAPI bool fmi3_instantiateCoSimulation(fmiHandle *fmu,
                                              fmi3Boolean                    visible,
                                              fmi3Boolean                    loggingOn,
                                              fmi3Boolean                    eventModeUsed,
                                              fmi3Boolean                    earlyReturnAllowed,
                                              const fmi3ValueReference       requiredIntermediateVariables[],
                                              size_t                         nRequiredIntermediateVariables,
                                              fmi3InstanceEnvironment        instanceEnvironment,
                                              fmi3LogMessageCallback         logMessage,
                                              fmi3IntermediateUpdateCallback intermediateUpdate);

FMI4C_DLLAPI bool fmi3_instantiateModelExchange(fmiHandle *fmu,
                                               fmi3Boolean                visible,
                                               fmi3Boolean                loggingOn,
                                               fmi3InstanceEnvironment    instanceEnvironment,
                                               fmi3LogMessageCallback     logMessage);

FMI4C_DLLAPI const char* fmi3_getVersion(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_setDebugLogging(fmiHandle *fmu,
                                            fmi3Boolean loggingOn,
                                            size_t nCategories,
                                            const fmi3String categories[]);

FMI4C_DLLAPI fmi3Status fmi3_getFloat64(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Float64 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setFloat64(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       const fmi3Float64 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_enterInitializationMode(fmiHandle *fmu,
                                                    fmi3Boolean toleranceDefined,
                                                    fmi3Float64 tolerance,
                                                    fmi3Float64 startTime,
                                                    fmi3Boolean stopTimeDefined,
                                                    fmi3Float64 stopTime);

FMI4C_DLLAPI fmi3Status fmi3_exitInitializationMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_terminate(fmiHandle *fmu);

FMI4C_DLLAPI void fmi3_freeInstance(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_doStep(fmiHandle *fmu,
                                   fmi3Float64 currentCommunicationPoint,
                                   fmi3Float64 communicationStepSize,
                                   fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                   fmi3Boolean *eventEncountered,
                                   fmi3Boolean *terminateSimulation,
                                   fmi3Boolean *earlyReturn,
                                   fmi3Float64 *lastSuccessfulTime);

FMI4C_DLLAPI fmi3Status fmi3_enterEventMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_reset(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_getFloat32(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Float32 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getInt8(fmiHandle *fmu,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    fmi3Int8 values[],
                                    size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getUInt8(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3UInt8 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getInt16(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Int16 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getUInt16(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt16 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getInt32(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Int32 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getUInt32(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt32 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getInt64(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Int64 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getUInt64(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3UInt64 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getBoolean(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       fmi3Boolean values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getString(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      fmi3String values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getBinary(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      size_t valueSizes[],
                                      fmi3Binary values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_getClock(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     fmi3Clock values[]);

FMI4C_DLLAPI fmi3Status fmi3_setFloat32(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       const fmi3Float32 values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setInt8(fmiHandle *fmu,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Int8 values[],
                                    size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setUInt8(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3UInt8 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setInt16(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Int16 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setUInt16(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt16 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setInt32(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Int32 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setUInt32(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt32 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setInt64(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Int64 values[],
                                     size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setUInt64(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3UInt64 values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setBoolean(fmiHandle *fmu,
                                       const fmi3ValueReference valueReferences[],
                                       size_t nValueReferences,
                                       const fmi3Boolean values[],
                                       size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setString(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const fmi3String values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setBinary(fmiHandle *fmu,
                                      const fmi3ValueReference valueReferences[],
                                      size_t nValueReferences,
                                      const size_t valueSizes[],
                                      const fmi3Binary values[],
                                      size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_setClock(fmiHandle *fmu,
                                     const fmi3ValueReference valueReferences[],
                                     size_t nValueReferences,
                                     const fmi3Clock values[]);

FMI4C_DLLAPI fmi3Status fmi3_getNumberOfVariableDependencies(fmiHandle *fmu,
                                                            fmi3ValueReference valueReference,
                                                            size_t* nDependencies);

FMI4C_DLLAPI fmi3Status fmi3_getVariableDependencies(fmiHandle *fmu,
                                                    fmi3ValueReference dependent,
                                                    size_t elementIndicesOfDependent[],
                                                    fmi3ValueReference independents[],
                                                    size_t elementIndicesOfIndependents[],
                                                    fmi3DependencyKind dependencyKinds[],
                                                    size_t nDependencies);

FMI4C_DLLAPI fmi3Status fmi3_getFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);

FMI4C_DLLAPI fmi3Status fmi3_setFMUState(fmiHandle *fmu, fmi3FMUState  FMUState);

FMI4C_DLLAPI fmi3Status fmi3_freeFMUState(fmiHandle *fmu, fmi3FMUState* FMUState);

FMI4C_DLLAPI fmi3Status fmi3_serializedFMUStateSize(fmiHandle *fmu,
                                                   fmi3FMUState  FMUState,
                                                   size_t* size);

FMI4C_DLLAPI fmi3Status fmi3_serializeFMUState(fmiHandle *fmu,
                                              fmi3FMUState  FMUState,
                                              fmi3Byte serializedState[],
                                              size_t size);

FMI4C_DLLAPI fmi3Status fmi3_deserializeFMUState(fmiHandle *fmu,
                                                const fmi3Byte serializedState[],
                                                size_t size,
                                                fmi3FMUState* FMUState);

FMI4C_DLLAPI fmi3Status fmi3_getDirectionalDerivative(fmiHandle *fmu,
                                                     const fmi3ValueReference unknowns[],
                                                     size_t nUnknowns,
                                                     const fmi3ValueReference knowns[],
                                                     size_t nKnowns,
                                                     const fmi3Float64 seed[],
                                                     size_t nSeed,
                                                     fmi3Float64 sensitivity[],
                                                     size_t nSensitivity);

FMI4C_DLLAPI fmi3Status fmi3_getAdjointDerivative(fmiHandle *fmu,
                                                 const fmi3ValueReference unknowns[],
                                                 size_t nUnknowns,
                                                 const fmi3ValueReference knowns[],
                                                 size_t nKnowns,
                                                 const fmi3Float64 seed[],
                                                 size_t nSeed,
                                                 fmi3Float64 sensitivity[],
                                                 size_t nSensitivity);

FMI4C_DLLAPI fmi3Status fmi3_enterConfigurationMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_exitConfigurationMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_getIntervalDecimal(fmiHandle *fmu,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               fmi3Float64 intervals[],
                                               fmi3IntervalQualifier qualifiers[]);

FMI4C_DLLAPI fmi3Status fmi3_getIntervalFraction(fmiHandle *fmu,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                fmi3UInt64 intervalCounters[],
                                                fmi3UInt64 resolutions[],
                                                fmi3IntervalQualifier qualifiers[]);

FMI4C_DLLAPI fmi3Status fmi3_getShiftDecimal(fmiHandle *fmu,
                                            const fmi3ValueReference valueReferences[],
                                            size_t nValueReferences,
                                            fmi3Float64 shifts[]);

FMI4C_DLLAPI fmi3Status fmi3_getShiftFraction(fmiHandle *fmu,
                                             const fmi3ValueReference valueReferences[],
                                             size_t nValueReferences,
                                             fmi3UInt64 shiftCounters[],
                                             fmi3UInt64 resolutions[]);

FMI4C_DLLAPI fmi3Status fmi3_setIntervalDecimal(fmiHandle *fmu,
                                               const fmi3ValueReference valueReferences[],
                                               size_t nValueReferences,
                                               const fmi3Float64 intervals[]);

FMI4C_DLLAPI fmi3Status fmi3_setIntervalFraction(fmiHandle *fmu,
                                                const fmi3ValueReference valueReferences[],
                                                size_t nValueReferences,
                                                const fmi3UInt64 intervalCounters[],
                                                const fmi3UInt64 resolutions[]);

FMI4C_DLLAPI fmi3Status fmi3_evaluateDiscreteStates(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_updateDiscreteStates(fmiHandle *fmu,
                                                 fmi3Boolean* discreteStatesNeedUpdate,
                                                 fmi3Boolean* terminateSimulation,
                                                 fmi3Boolean* nominalsOfContinuousStatesChanged,
                                                 fmi3Boolean* valuesOfContinuousStatesChanged,
                                                 fmi3Boolean* nextEventTimeDefined,
                                                 fmi3Float64* nextEventTime);

FMI4C_DLLAPI fmi3Status fmi3_enterContinuousTimeMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_completedIntegratorStep(fmiHandle *fmu,
                                                    fmi3Boolean  noSetFMUStatePriorToCurrentPoint,
                                                    fmi3Boolean* enterEventMode,
                                                    fmi3Boolean* terminateSimulation);

FMI4C_DLLAPI fmi3Status fmi3_setTime(fmiHandle *fmu, fmi3Float64 time);

FMI4C_DLLAPI fmi3Status fmi3_setContinuousStates(fmiHandle *fmu,
                                                const fmi3Float64 continuousStates[],
                                                size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3_getContinuousStateDerivatives(fmiHandle *fmu,
                                                          fmi3Float64 derivatives[],
                                                          size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3_getEventIndicators(fmiHandle *fmu,
                                               fmi3Float64 eventIndicators[],
                                               size_t nEventIndicators);

FMI4C_DLLAPI fmi3Status fmi3_getContinuousStates(fmiHandle *fmu,
                                                fmi3Float64 continuousStates[],
                                                size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3_getNominalsOfContinuousStates(fmiHandle *fmu,
                                                          fmi3Float64 nominals[],
                                                          size_t nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3_getNumberOfEventIndicators(fmiHandle *fmu,
                                                       size_t* nEventIndicators);

FMI4C_DLLAPI fmi3Status fmi3_getNumberOfContinuousStates(fmiHandle *fmu,
                                                        size_t* nContinuousStates);

FMI4C_DLLAPI fmi3Status fmi3_enterStepMode(fmiHandle *fmu);

FMI4C_DLLAPI fmi3Status fmi3_getOutputDerivatives(fmiHandle *fmu,
                                                 const fmi3ValueReference valueReferences[],
                                                 size_t nValueReferences,
                                                 const fmi3Int32 orders[],
                                                 fmi3Float64 values[],
                                                 size_t nValues);

FMI4C_DLLAPI fmi3Status fmi3_activateModelPartition(fmiHandle *fmu,
                                                   fmi3ValueReference clockReference,
                                                   fmi3Float64 activationTime);

#ifdef __cplusplus
}
#endif


#endif // FMIC_H
