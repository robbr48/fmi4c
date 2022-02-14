#ifndef FMIC_PRIVATE_H
#define FMIC_PRIVATE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "fmi4c_types.h"
#include "fmi4c_types_fmi1.h"
#include "fmi4c_types_fmi2.h"
#include "fmi4c_types_fmi3.h"
#include "fmi4c_functions_fmi1.h"
#include "fmi4c_functions_fmi2.h"
#include "fmi4c_functions_fmi3.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef DEBUG
#define TRACEFUNC printf("In function: %s\n",__func__);
#else
#define TRACEFUNC
#endif

#ifdef _WIN32
#define LOADFUNCTION2(FUNCNAME) (FUNCNAME ## _t)GetProcAddress(dll, getFunctionName(fmu->fmi1.modelName, #FUNCNAME));
#define LOADFUNCTION(FUNCNAME) (FUNCNAME ## _t)GetProcAddress(dll, #FUNCNAME);
#else
#define LOADFUNCTION2(FUNCNAME) (FUNCNAME ## _t)dlsym(dll, getFunctionName(fmu->modelName, #FUNCNAME));
#define LOADFUNCTION(FUNCNAME) (FUNCNAME ## _t)dlsym(dll, #FUNCNAME);
#endif

#define CHECKFUNCTION2(FMIVERSION, FUNCNAME) ({ \
    if(fmu->fmi##FMIVERSION.FUNCNAME == NULL) { \
        printf("Failed to load function \"%s\"\n",#FUNCNAME); \
        return NULL; \
    } \
})


#define CHECKFUNCTION(FUNCNAME) ({ \
    if(fmu->FUNCNAME == NULL) { \
        printf("Failed to load function \"%s\"\n",#FUNCNAME); \
        return NULL; \
    } \
})

extern const char* fmi4cErrorMessage;

typedef struct {
    fmi1DataType datatype;
    const char *name;
    const char *description;
    int64_t valueReference;
    fmi1Real startReal;
    fmi1Integer startInteger;
    fmi1Boolean startBoolean;
    fmi1String startString;
    fmi1Causality causality;
    fmi1Variability variability;
    fmi1Alias alias;
    bool fixed;
} fmi1VariableHandle;

typedef struct {
    fmi2DataType datatype;
    const char *name;
    const char *description;
    const char *quantity;
    const char *unit;
    const char *displayUnit;
    fmi2Real startReal;
    fmi2Integer startInteger;
    fmi2Boolean startBoolean;
    fmi2String startString;
    int64_t valueReference;
    fmi2Causality causality;
    fmi2Variability variability;
    fmi2Initial initial;
    bool canHandleMultipleSetPerTimeInstant;
    fmi2ValueReference derivative;
} fmi2VariableHandle;

typedef struct {
    fmi3DataType datatype;
    const char *name;
    const char *description;
    const char *quantity;
    const char *unit;
    const char *displayUnit;
    bool relativeQuantity;
    bool unbounded;
    double min;
    double max;
    double nominal;
    fmi3Float64 startFloat64;
    fmi3Float32 startFloat32;
    fmi3Int64 startInt64;
    fmi3Int32 startInt32;
    fmi3Int16 startInt16;
    fmi3Int8 startInt8;
    fmi3UInt64 startUInt64;
    fmi3UInt32 startUInt32;
    fmi3UInt16 startUInt16;
    fmi3UInt8 startUInt8;
    fmi3Boolean startBoolean;
    fmi3Binary startBinary;
    fmi3Clock startClock;
    fmi3String startString;
    fmi3Int64 startEnumeration;
    unsigned int derivative;
    bool reInit;
    int64_t valueReference;
    fmi3Causality causality;
    fmi3Variability variability;
    fmi3Initial initial;
    bool canHandleMultipleSetPerTimeInstant;
    bool intermediateUpdate;
    unsigned int previous;
    const char* declaredType;
    const char *mimeType;
    int maxSize;
    bool canBeDeactivated;
    int priority;
    fmi3IntervalVariability intervalVariability;
    double intervalDecimal;
    double shiftDecimal;
    bool supportsFraction;
    int64_t resolution;
    int64_t intervalCounter;
    int64_t shiftCounter;

} fmi3VariableHandle;

typedef struct {
    const char* modelName;
    const char* modelIdentifier;
    const char* guid;
    const char* description;
    const char* author;
    const char* version;
    const char* generationTool;
    const char* generationDateAndTime;
    const char* variableNamingConvention;
    int numberOfContinuousStates;
    int numberOfEventIndicators;

    bool canHandleVariableCommunicationStepSize;
    bool canHandleEvents;
    bool canRejectSteps;
    bool canInterpolateInputs;
    int maxOutputDerivativeOrder;
    bool canRunAsynchronuously;
    bool canSignalEvents;
    bool canBeInstantiatedOnlyOncePerProcess;
    bool canNotUseMemoryManagementFunctions;
    bool hasRealVariables;
    bool hasIntegerVariables;
    bool hasStringVariables;
    bool hasBooleanVariables;

    bool defaultStartTimeDefined;
    bool defaultStopTimeDefined;
    bool defaultToleranceDefined;

    double defaultStartTime;
    double defaultStopTime;
    double defaultTolerance;

    int numberOfVariables;
    fmi1VariableHandle *variables;
    int variablesSize;

    fmi1Type type;

    fmi1Component_t component;

    fmi1CallbackFunctionsCoSimulation callbacksCoSimulation;
    fmi1CallbackFunctionsModelExchange callbacksModelExchange;

    fmiGetVersion_t getVersion;
    fmiGetTypesPlatform_t getTypesPlatform;
    fmiSetDebugLogging_t setDebugLogging;
    fmiGetReal_t getReal;
    fmiGetInteger_t getInteger;
    fmiGetBoolean_t getBoolean;
    fmiGetString_t getString;
    fmiSetReal_t setReal;
    fmiSetInteger_t setInteger;
    fmiSetBoolean_t setBoolean;
    fmiSetString_t setString;
    fmiInstantiateSlave_t instantiateSlave;
    fmiInitializeSlave_t initializeSlave;
    fmiTerminateSlave_t terminateSlave;
    fmiResetSlave_t resetSlave;
    fmiFreeSlaveInstance_t freeSlaveInstance;
    fmiSetRealInputDerivatives_t setRealInputDerivatives;
    fmiGetRealOutputDerivatives_t getRealOutputDerivatives;
    fmiCancelStep_t cancelStep;
    fmiDoStep_t doStep;
    fmiGetStatus_t getStatus;
    fmiGetRealStatus_t getRealStatus;
    fmiGetIntegerStatus_t getIntegerStatus;
    fmiGetBooleanStatus_t getBooleanStatus;
    fmiGetStringStatus_t getStringStatus;
    fmiGetModelTypesPlatform_t getModelTypesPlatform;
    fmiInstantiateModel_t instantiateModel;
    fmiFreeModelInstance_t freeModelInstance;
    fmiSetTime_t setTime;
    fmiSetContinuousStates_t setContinuousStates;
    fmiCompletedIntegratorStep_t completedIntegratorStep;
    fmiInitialize_t initialize;
    fmiGetDerivatives_t getDerivatives;
    fmiGetEventIndicators_t getEventIndicators;
    fmiEventUpdate_t eventUpdate;
    fmiGetContinuousStates_t getContinuousStates;
    fmiGetNominalContinuousStates_t getNominalContinuousStates;
    fmiGetStateValueReferences_t getStateValueReferences;
    fmiTerminate_t terminate;
} fmi1Data_t;

typedef struct {
    const char* modelName;
    const char* guid;
    const char* description;
    const char* author;
    const char* version;
    const char* copyright;
    const char* license;
    const char* generationTool;
    const char* generationDateAndTime;
    const char* variableNamingConvention;
    int numberOfEventIndicators;

    bool supportsCoSimulation;
    bool supportsModelExchange;

    const char* modelIdentifier;
    bool needsExecutionTool;
    bool canHandleVariableCommunicationStepSize;
    bool canInterpolateInputs;
    int maxOutputDerivativeOrder;
    bool canRunAsynchronuously;
    bool canBeInstantiatedOnlyOncePerProcess;
    bool canNotUseMemoryManagementFunctions;
    bool canGetAndSetFMUState;
    bool canSerializeFMUState;
    bool providesDirectionalDerivative;

    bool hasRealVariables;
    bool hasIntegerVariables;
    bool hasBooleanVariables;
    bool hasStringVariables;

    bool defaultStartTimeDefined;
    bool defaultStopTimeDefined;
    bool defaultToleranceDefined;
    bool defaultStepSizeDefined;

    double defaultStartTime;
    double defaultStopTime;
    double defaultTolerance;
    double defaultStepSize;

    int numberOfContinuousStates;

    int numberOfVariables;
    fmi2VariableHandle *variables;
    int variablesSize;

    fmi2Component component;

    fmi2GetTypesPlatform_t getTypesPlatform;
    fmi2GetVersion_t getVersion;
    fmi2SetDebugLogging_t setDebugLogging;
    fmi2Instantiate_t instantiate;
    fmi2FreeInstance_t freeInstance;
    fmi2SetupExperiment_t setupExperiment;
    fmi2EnterInitializationMode_t enterInitializationMode;
    fmi2ExitInitializationMode_t exitInitializationMode;
    fmi2Terminate_t terminate;
    fmi2Reset_t reset;
    fmi2GetReal_t getReal;
    fmi2GetInteger_t getInteger;
    fmi2GetBoolean_t getBoolean;
    fmi2GetString_t getString;
    fmi2SetReal_t setReal;
    fmi2SetInteger_t setInteger;
    fmi2SetBoolean_t setBoolean;
    fmi2SetString_t setString;
    fmi2GetFMUstate_t getFMUstate;
    fmi2SetFMUstate_t setFMUstate;
    fmi2FreeFMUstate_t freeFMUstate;
    fmi2SerializedFMUstateSize_t serializedFMUstateSize;
    fmi2SerializeFMUstate_t serializeFMUstate;
    fmi2DeSerializeFMUstate_t deSerializeFMUstate;
    fmi2GetDirectionalDerivative_t getDirectionalDerivative;
    fmi2EnterEventMode_t enterEventMode;
    fmi2NewDiscreteStates_t newDiscreteStates;
    fmi2EnterContinuousTimeMode_t enterContinuousTimeMode;
    fmi2CompletedIntegratorStep_t completedIntegratorStep;
    fmi2SetTime_t setTime;
    fmi2SetContinuousStates_t setContinuousStates;
    fmi2GetDerivatives_t getDerivatives;
    fmi2GetEventIndicators_t getEventIndicators;
    fmi2GetContinuousStates_t getContinuousStates;
    fmi2GetNominalsOfContinuousStates_t getNominalsOfContinuousStates;
    fmi2SetRealInputDerivatives_t setRealInputDerivatives;
    fmi2GetRealOutputDerivatives_t getRealOutputDerivatives;
    fmi2DoStep_t doStep;
    fmi2CancelStep_t cancelStep;
    fmi2GetStatus_t getStatus;
    fmi2GetRealStatus_t getRealStatus;
    fmi2GetIntegerStatus_t getIntegerStatus;
    fmi2GetBooleanStatus_t getBooleanStatus;
    fmi2GetStringStatus_t getStringStatus;

    fmi2CallbackFunctions callbacks;
} fmi2Data_t;

typedef struct {
    bool supportsModelExchange;
    bool supportsCoSimulation;
    bool supportsScheduledExecution;

    const char* modelName;
    const char* instantiationToken;
    const char* description;
    const char* author;
    const char* version;
    const char* copyright;
    const char* license;
    const char* generationTool;
    const char* generationDateAndTime;
    const char* variableNamingConvention;

    const char* modelIdentifier;

    //Common capabilities
    bool needsExecutionTool;
    bool canBeInstantiatedOnlyOncePerProcess;
    bool canGetAndSetFMUState;
    bool canSerializeFMUState;
    bool providesDirectionalDerivative;
    bool providesAdjointDerivatives;
    bool providesPerElementDependencies;
    int maxOutputDerivativeOrder;
    bool providesIntermediateUpdate;
    bool mightReturnEarlyFromDoStep;
    bool providesEvaluateDiscreteStates;
    int recommendedIntermediateInputSmoothness;

    //Co-simulation only capabilities
    bool canHandleVariableCommunicationStepSize;
    bool canReturnEarlyAfterIntermediateUpdate;
    double fixedInternalStepSize;
    bool hasEventMode;

    //Model exchange only capabilities
    bool needsCompletedIntegratorStep;

    bool hasFloat64Variables;
    bool hasFloat32Variables;
    bool hasInt64Variables;
    bool hasInt32Variables;
    bool hasInt16Variables;
    bool hasInt8Variables;
    bool hasUInt64Variables;
    bool hasUInt32Variables;
    bool hasUInt16Variables;
    bool hasUInt8Variables;
    bool hasBooleanVariables;
    bool hasStringVariables;
    bool hasBinaryVariables;
    bool hasEnumerationVariables;
    bool hasClockVariables;
    bool hasStructuralParameters;

    bool defaultStartTimeDefined;
    bool defaultStopTimeDefined;
    bool defaultToleranceDefined;
    bool defaultStepSizeDefined;

    double defaultStartTime;
    double defaultStopTime;
    double defaultTolerance;
    double defaultStepSize;

    int numberOfVariables;
    fmi3VariableHandle *variables;
    int variablesSize;

    fmi3Instance fmi3Instance;
    fmi3GetVersion_t getVersion;
    fmi3SetDebugLogging_t setDebugLogging;
    fmi3InstantiateModelExchange_t instantiateModelExchange;
    fmi3InstantiateCoSimulation_t instantiateCoSimulation;
    fmi3InstantiateScheduledExecution_t instantiateScheduledExecution;
    fmi3FreeInstance_t freeInstance;
    fmi3EnterInitializationMode_t enterInitializationMode;
    fmi3ExitInitializationMode_t exitInitializationMode;
    fmi3Terminate_t terminate;
    fmi3SetFloat64_t setFloat64;
    fmi3GetFloat64_t getFloat64;
    fmi3DoStep_t doStep;
    fmi3EnterEventMode_t enterEventMode;
    fmi3Reset_t reset;
    fmi3GetFloat32_t getFloat32;
    fmi3GetInt8_t getInt8;
    fmi3GetUInt8_t getUInt8;
    fmi3GetInt16_t getInt16;
    fmi3GetUInt16_t getUInt16;
    fmi3GetInt32_t getInt32;
    fmi3GetUInt32_t getUInt32;
    fmi3GetInt64_t getInt64;
    fmi3GetUInt64_t getUInt64;
    fmi3GetBoolean_t getBoolean;
    fmi3GetString_t getString;
    fmi3GetBinary_t getBinary;
    fmi3GetClock_t getClock;
    fmi3SetFloat32_t setFloat32;
    fmi3SetInt8_t setInt8;
    fmi3SetUInt8_t setUInt8;
    fmi3SetInt16_t setInt16;
    fmi3SetUInt16_t setUInt16;
    fmi3SetInt32_t setInt32;
    fmi3SetUInt32_t setUInt32;
    fmi3SetInt64_t setInt64;
    fmi3SetUInt64_t setUInt64;
    fmi3SetBoolean_t setBoolean;
    fmi3SetString_t setString;
    fmi3SetBinary_t setBinary;
    fmi3SetClock_t setClock;
    fmi3GetNumberOfVariableDependencies_t getNumberOfVariableDependencies;
    fmi3GetVariableDependencies_t getVariableDependencies;
    fmi3GetFMUState_t getFMUState;
    fmi3SetFMUState_t setFMUState;
    fmi3FreeFMUState_t freeFMUState;
    fmi3SerializedFMUStateSize_t serializedFMUStateSize;
    fmi3SerializeFMUState_t serializeFMUState;
    fmi3DeSerializeFMUState_t deSerializeFMUState;
    fmi3GetDirectionalDerivative_t getDirectionalDerivative;
    fmi3GetAdjointDerivative_t getAdjointDerivative;
    fmi3EnterConfigurationMode_t enterConfigurationMode;
    fmi3ExitConfigurationMode_t exitConfigurationMode;
    fmi3GetIntervalDecimal_t getIntervalDecimal;
    fmi3GetIntervalFraction_t getIntervalFraction;
    fmi3GetShiftDecimal_t getShiftDecimal;
    fmi3GetShiftFraction_t getShiftFraction;
    fmi3SetIntervalDecimal_t setIntervalDecimal;
    fmi3SetIntervalFraction_t setIntervalFraction;
    fmi3EvaluateDiscreteStates_t evaluateDiscreteStates;
    fmi3UpdateDiscreteStates_t updateDiscreteStates;
    fmi3EnterContinuousTimeMode_t enterContinuousTimeMode;
    fmi3CompletedIntegratorStep_t completedIntegratorStep;
    fmi3SetTime_t setTime;
    fmi3SetContinuousStates_t setContinuousStates;
    fmi3GetContinuousStateDerivatives_t getContinuousStateDerivatives;
    fmi3GetEventIndicators_t getEventIndicators;
    fmi3GetContinuousStates_t getContinuousStates;
    fmi3GetNominalsOfContinuousStates_t getNominalsOfContinuousStates;
    fmi3GetNumberOfEventIndicators_t getNumberOfEventIndicators;
    fmi3GetNumberOfContinuousStates_t getNumberOfContinuousStates;
    fmi3EnterStepMode_t enterStepMode;
    fmi3GetOutputDerivatives_t getOutputDerivatives;
    fmi3ActivateModelPartition_t activateModelPartition;
} fmi3Data_t;


typedef struct {
    fmiVersion_t version;
    const char* unzippedLocation;
    const char* resourcesLocation;
    const char* instanceName;
#ifdef _WIN32
    HINSTANCE dll;
#else
    void* dll;
#endif

    fmi1Data_t fmi1;
    fmi2Data_t fmi2;
    fmi3Data_t fmi3;
} fmiHandle;

bool parseModelDescriptionFmi1(fmiHandle *fmuFile);
bool parseModelDescriptionFmi2(fmiHandle *fmuFile);
bool parseModelDescriptionFmi3(fmiHandle *fmuFile);

bool loadFunctionsFmi1(fmiHandle *contents);
bool loadFunctionsFmi2(fmiHandle *contents);
bool loadFunctionsFmi3(fmiHandle *contents);

#endif // FMIC_PRIVATE_H
