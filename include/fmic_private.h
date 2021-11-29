#ifndef FMIC_PRIVATE_H
#define FMIC_PRIVATE_H

#include "fmic_types.h"
#include "3rdparty/fmi/fmi2FunctionTypes.h"
#include "3rdparty/fmi/fmi3FunctionTypes.h"
#include "fmic_functions.h"

#include <windows.h>

#define TRACEFUNC printf("In function: %s\n",__func__);

#define LOADFUNCTION(FUNCNAME) (FUNCNAME ## _t)GetProcAddress(dll, #FUNCNAME);
#define CHECKFUNCTION(FUNCNAME) ({ \
    if(fmu->FUNCNAME == NULL) { \
        printf("Failed to load function \"%s\" from %s\n",#FUNCNAME,fmu->modelIdentifier); \
        return NULL; \
    } \
})

typedef struct {
    fmi2DataType datatype;
    char *instanceName;
    char *description;
    char *quantity;
    char *unit;
    char *displayUnit;
    fmi2Real startReal;
    long valueReference;
    fmi2Causality causality;
    fmi2Variability variability;
    fmi2Initial initial;
    bool canHandleMultipleSetPerTimeInstant;
} fmi2VariableHandle;

typedef struct {
    fmi3DataType datatype;
    char *instanceName;
    char *description;
    char *quantity;
    char *unit;
    char *displayUnit;
    fmi3Float64 startFloat64;
    long valueReference;
    fmi3Causality causality;
    fmi3Variability variability;
    fmi3Initial initial;
    bool intermediateUpdate;
} fmi3VariableHandle;

typedef struct {
    const char* unzippedLocation;
    const char* resourcesLocation;
    const char* instanceName;
} fmiHandle;

typedef struct {
    HINSTANCE dll;

    int hierarchyLength;
    char* hierarchy[5];

    bool supportsModelExchange;
    bool supportsCoSimulation;
    bool supportsScheduledExecution;

    const char* instanceName;
    const char* unzippedLocation;
    const char* resourcesLocation;

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
    bool providesEvaluateDiscreteStates;
    bool recommendedIntermediateInputSmoothness;

    //Co-simulation only capabilities
    bool canHandleVariableCommunicationStepSize;
    bool canReturnEarlyAfterIntermediateUpdate;
    double fixedInternalStepSize;
    bool hasEventMode;

    //Model exchange only capabilities
    bool completedIntegratorStepNotNeeded;


    int numberOfVariables;
    fmi3VariableHandle *variables;
    int variablesSize;

    bool defaultStartTimeDefined;
    bool defaultStopTimeDefined;
    bool defaultToleranceDefined;
    bool defaultStepSizeDefined;

    double defaultStartTime;
    double defaultStopTime;
    double defaultTolerance;
    double defaultStepSize;

    fmi3Instance _fmi3Instance;
    fmi3GetVersion_t fmi3GetVersion;
    fmi3SetDebugLogging_t fmi3SetDebugLogging;
    fmi3InstantiateModelExchange_t fmi3InstantiateModelExchange;
    fmi3InstantiateCoSimulation_t fmi3InstantiateCoSimulation;
    fmi3InstantiateScheduledExecution_t fmi3InstantiateScheduledExecution;
    fmi3FreeInstance_t fmi3FreeInstance;
    fmi3EnterInitializationMode_t fmi3EnterInitializationMode;
    fmi3ExitInitializationMode_t fmi3ExitInitializationMode;
    fmi3Terminate_t fmi3Terminate;
    fmi3SetFloat64_t fmi3SetFloat64;
    fmi3GetFloat64_t fmi3GetFloat64;
    fmi3DoStep_t fmi3DoStep;
    fmi3EnterEventMode_t fmi3EnterEventMode;
    fmi3Reset_t fmi3Reset;
    fmi3GetFloat32_t fmi3GetFloat32;
    fmi3GetInt8_t fmi3GetInt8;
    fmi3GetUInt8_t fmi3GetUInt8;
    fmi3GetInt16_t fmi3GetInt16;
    fmi3GetUInt16_t fmi3GetUInt16;
    fmi3GetInt32_t fmi3GetInt32;
    fmi3GetUInt32_t fmi3GetUInt32;
    fmi3GetInt64_t fmi3GetInt64;
    fmi3GetUInt64_t fmi3GetUInt64;
    fmi3GetBoolean_t fmi3GetBoolean;
    fmi3GetString_t fmi3GetString;
    fmi3GetBinary_t fmi3GetBinary;
    fmi3GetClock_t fmi3GetClock;
    fmi3SetFloat32_t fmi3SetFloat32;
    fmi3SetInt8_t fmi3SetInt8;
    fmi3SetUInt8_t fmi3SetUInt8;
    fmi3SetInt16_t fmi3SetInt16;
    fmi3SetUInt16_t fmi3SetUInt16;
    fmi3SetInt32_t fmi3SetInt32;
    fmi3SetUInt32_t fmi3SetUInt32;
    fmi3SetInt64_t fmi3SetInt64;
    fmi3SetUInt64_t fmi3SetUInt64;
    fmi3SetBoolean_t fmi3SetBoolean;
    fmi3SetString_t fmi3SetString;
    fmi3SetBinary_t fmi3SetBinary;
    fmi3SetClock_t fmi3SetClock;
    fmi3GetNumberOfVariableDependencies_t fmi3GetNumberOfVariableDependencies;
    fmi3GetVariableDependencies_t fmi3GetVariableDependencies;
    fmi3GetFMUState_t fmi3GetFMUState;
    fmi3SetFMUState_t fmi3SetFMUState;
    fmi3FreeFMUState_t fmi3FreeFMUState;
    fmi3SerializedFMUStateSize_t fmi3SerializedFMUStateSize;
    fmi3SerializeFMUState_t fmi3SerializeFMUState;
    fmi3DeSerializeFMUState_t fmi3DeSerializeFMUState;
    fmi3GetDirectionalDerivative_t fmi3GetDirectionalDerivative;
    fmi3GetAdjointDerivative_t fmi3GetAdjointDerivative;
    fmi3EnterConfigurationMode_t fmi3EnterConfigurationMode;
    fmi3ExitConfigurationMode_t fmi3ExitConfigurationMode;
    fmi3GetIntervalDecimal_t fmi3GetIntervalDecimal;
    fmi3GetIntervalFraction_t fmi3GetIntervalFraction;
    fmi3GetShiftDecimal_t fmi3GetShiftDecimal;
    fmi3GetShiftFraction_t fmi3GetShiftFraction;
    fmi3SetIntervalDecimal_t fmi3SetIntervalDecimal;
    fmi3SetIntervalFraction_t fmi3SetIntervalFraction;
    fmi3EvaluateDiscreteStates_t fmi3EvaluateDiscreteStates;
    fmi3UpdateDiscreteStates_t fmi3UpdateDiscreteStates;
    fmi3EnterContinuousTimeMode_t fmi3EnterContinuousTimeMode;
    fmi3CompletedIntegratorStep_t fmi3CompletedIntegratorStep;
    fmi3SetTime_t fmi3SetTime;
    fmi3SetContinuousStates_t fmi3SetContinuousStates;
    fmi3GetContinuousStateDerivatives_t fmi3GetContinuousStateDerivatives;
    fmi3GetEventIndicators_t fmi3GetEventIndicators;
    fmi3GetContinuousStates_t fmi3GetContinuousStates;
    fmi3GetNominalsOfContinuousStates_t fmi3GetNominalsOfContinuousStates;
    fmi3GetNumberOfEventIndicators_t fmi3GetNumberOfEventIndicators;
    fmi3GetNumberOfContinuousStates_t fmi3GetNumberOfContinuousStates;
    fmi3EnterStepMode_t fmi3EnterStepMode;
    fmi3GetOutputDerivatives_t fmi3GetOutputDerivatives;
    fmi3ActivateModelPartition_t fmi3ActivateModelPartition;
} fmi3Handle;

typedef struct {
    HINSTANCE dll;

    const char* instanceName;
    const char* unzippedLocation;
    const char* resourcesLocation;
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
    const char* numberOfEventIndicators;

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

    int numberOfVariables;
    fmi2VariableHandle *variables;
    int variablesSize;

    bool defaultStartTimeDefined;
    bool defaultStopTimeDefined;
    bool defaultToleranceDefined;
    bool defaultStepSizeDefined;

    double defaultStartTime;
    double defaultStopTime;
    double defaultTolerance;
    double defaultStepSize;

    fmi2Component _fmi2Component;
    fmi2GetTypesPlatform_t fmi2GetTypesPlatform;
    fmi2GetVersion_t fmi2GetVersion;
    fmi2SetDebugLogging_t fmi2SetDebugLogging;
    fmi2Instantiate_t fmi2Instantiate;
    fmi2FreeInstance_t fmi2FreeInstance;
    fmi2SetupExperiment_t fmi2SetupExperiment;
    fmi2EnterInitializationMode_t fmi2EnterInitializationMode;
    fmi2ExitInitializationMode_t fmi2ExitInitializationMode;
    fmi2Terminate_t fmi2Terminate;
    fmi2Reset_t fmi2Reset;
    fmi2GetReal_t fmi2GetReal;
    fmi2GetInteger_t fmi2GetInteger;
    fmi2GetBoolean_t fmi2GetBoolean;
    fmi2GetString_t fmi2GetString;
    fmi2SetReal_t fmi2SetReal;
    fmi2SetInteger_t fmi2SetInteger;
    fmi2SetBoolean_t fmi2SetBoolean;
    fmi2SetString_t fmi2SetString;
    fmi2GetFMUstate_t fmi2GetFMUstate;
    fmi2SetFMUstate_t fmi2SetFMUstate;
    fmi2FreeFMUstate_t fmi2FreeFMUstate;
    fmi2SerializedFMUstateSize_t fmi2SerializedFMUstateSize;
    fmi2SerializeFMUstate_t fmi2SerializeFMUstate;
    fmi2DeSerializeFMUstate_t fmi2DeSerializeFMUstate;
    fmi2GetDirectionalDerivative_t fmi2GetDirectionalDerivative;
    fmi2EnterEventMode_t fmi2EnterEventMode;
    fmi2NewDiscreteStates_t fmi2NewDiscreteStates;
    fmi2EnterContinuousTimeMode_t fmi2EnterContinuousTimeMode;
    fmi2CompletedIntegratorStep_t fmi2CompletedIntegratorStep;
    fmi2SetTime_t fmi2SetTime;
    fmi2SetContinuousStates_t fmi2SetContinuousStates;
    fmi2GetDerivatives_t fmi2GetDerivatives;
    fmi2GetEventIndicators_t fmi2GetEventIndicators;
    fmi2GetContinuousStates_t fmi2GetContinuousStates;
    fmi2GetNominalsOfContinuousStates_t fmi2GetNominalsOfContinuousStates;
    fmi2SetRealInputDerivatives_t fmi2SetRealInputDerivatives;
    fmi2GetRealOutputDerivatives_t fmi2GetRealOutputDerivatives;
    fmi2DoStep_t fmi2DoStep;
    fmi2CancelStep_t fmi2CancelStep;
    fmi2GetStatus_t fmi2GetStatus;
    fmi2GetRealStatus_t fmi2GetRealStatus;
    fmi2GetIntegerStatus_t fmi2GetIntegerStatus;
    fmi2GetBooleanStatus_t fmi2GetBooleanStatus;
    fmi2GetStringStatus_t fmi2GetStringStatus;

    fmi2CallbackFunctions callbacks;
} fmi2Handle;

bool parseModelDescriptionFmi2(fmi2Handle *fmuFile);
bool parseModelDescriptionFmi3(fmi3Handle *fmuFile);

bool loadFunctionsFmi2(fmi2Handle *contents);
bool loadFunctionsFmi3(fmi3Handle *contents);

#endif // FMIC_PRIVATE_H
