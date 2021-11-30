#ifndef FMIC_FUNCTIONS_FMI2_H
#define FMIC_FUNCTIONS_FMI2_H

// Callback functions
typedef void (*fmi2CallbackLogger) (fmi2ComponentEnvironment, fmi2String, fmi2Status, fmi2String, fmi2String, ...);
typedef void* (*fmi2CallbackAllocateMemory)(size_t, size_t);
typedef void (*fmi2CallbackFreeMemory) (void*);
typedef void (*fmi2StepFinished) (fmi2ComponentEnvironment, fmi2Status);

// Structs
typedef struct {
    fmi2CallbackLogger logger;
    fmi2CallbackAllocateMemory allocateMemory;
    fmi2CallbackFreeMemory freeMemory;
    fmi2StepFinished stepFinished;
    fmi2ComponentEnvironment componentEnvironment;
} fmi2CallbackFunctions;

typedef struct {
    fmi2Boolean newDiscreteStatesNeeded;
    fmi2Boolean terminateSimulation;
    fmi2Boolean nominalsOfContinuousStatesChanged;
    fmi2Boolean valuesOfContinuousStatesChanged;
    fmi2Boolean nextEventTimeDefined;
    fmi2Real nextEventTime;
} fmi2EventInfo;

// API functions
typedef const char* (__stdcall *fmi2GetTypesPlatform_t)(void);

typedef const char* (__stdcall *fmi2GetVersion_t)(void);

typedef fmi2Status (__stdcall *fmi2SetDebugLogging_t)(fmi2Component,
                                                      fmi2Boolean,
                                                      size_t,
                                                      const fmi2String[]);

typedef fmi2Component (__stdcall *fmi2Instantiate_t)(fmi2String,
                                                     fmi2Type,
                                                     fmi2String,
                                                     fmi2String,
                                                     const fmi2CallbackFunctions*,
                                                     fmi2Boolean,
                                                     fmi2Boolean);

typedef void (__stdcall *fmi2FreeInstance_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2SetupExperiment_t)(fmi2Component, fmi2Boolean, fmi2Real, fmi2Real, fmi2Boolean, fmi2Real);
typedef fmi2Status (__stdcall *fmi2EnterInitializationMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2ExitInitializationMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2Terminate_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2Reset_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2GetReal_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2Real []);
typedef fmi2Status (__stdcall *fmi2GetInteger_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2Integer[]);
typedef fmi2Status (__stdcall *fmi2GetBoolean_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2Boolean[]);
typedef fmi2Status (__stdcall *fmi2GetString_t)(fmi2Component, const fmi2ValueReference[], size_t, fmi2String []);
typedef fmi2Status (__stdcall *fmi2SetReal_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Real []);
typedef fmi2Status (__stdcall *fmi2SetInteger_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Integer[]);
typedef fmi2Status (__stdcall *fmi2SetBoolean_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2Boolean[]);
typedef fmi2Status (__stdcall *fmi2SetString_t)(fmi2Component, const fmi2ValueReference[], size_t, const fmi2String []);
typedef fmi2Status (__stdcall *fmi2GetFMUstate_t)(fmi2Component, fmi2FMUstate*);
typedef fmi2Status (__stdcall *fmi2SetFMUstate_t)(fmi2Component, fmi2FMUstate);
typedef fmi2Status (__stdcall *fmi2FreeFMUstate_t)(fmi2Component, fmi2FMUstate*);
typedef fmi2Status (__stdcall *fmi2SerializedFMUstateSize_t)(fmi2Component, fmi2FMUstate, size_t*);
typedef fmi2Status (__stdcall *fmi2SerializeFMUstate_t)(fmi2Component, fmi2FMUstate, fmi2Byte[], size_t);
typedef fmi2Status (__stdcall *fmi2DeSerializeFMUstate_t)(fmi2Component, const fmi2Byte[], size_t, fmi2FMUstate*);
typedef fmi2Status (__stdcall *fmi2GetDirectionalDerivative_t)(fmi2Component, const fmi2ValueReference[], size_t,
                                                               const fmi2ValueReference[], size_t,
                                                               const fmi2Real[], fmi2Real[]);
typedef fmi2Status (__stdcall *fmi2EnterEventMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2NewDiscreteStates_t)(fmi2Component, fmi2EventInfo*);
typedef fmi2Status (__stdcall *fmi2EnterContinuousTimeMode_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2CompletedIntegratorStep_t)(fmi2Component, fmi2Boolean, fmi2Boolean*, fmi2Boolean*);
typedef fmi2Status (__stdcall *fmi2SetTime_t)(fmi2Component, fmi2Real);
typedef fmi2Status (__stdcall *fmi2SetContinuousStates_t)(fmi2Component, const fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetDerivatives_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetEventIndicators_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetContinuousStates_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2GetNominalsOfContinuousStates_t)(fmi2Component, fmi2Real[], size_t);
typedef fmi2Status (__stdcall *fmi2SetRealInputDerivatives_t)(fmi2Component, const fmi2ValueReference [],
                                                              size_t, const fmi2Integer [], const fmi2Real []);
typedef fmi2Status (__stdcall *fmi2GetRealOutputDerivatives_t)(fmi2Component, const fmi2ValueReference [],
                                                               size_t, const fmi2Integer [], fmi2Real []);
typedef fmi2Status (__stdcall *fmi2DoStep_t)(fmi2Component, fmi2Real, fmi2Real, fmi2Boolean);
typedef fmi2Status (__stdcall *fmi2CancelStep_t)(fmi2Component);
typedef fmi2Status (__stdcall *fmi2GetStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Status* );
typedef fmi2Status (__stdcall *fmi2GetRealStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Real* );
typedef fmi2Status (__stdcall *fmi2GetIntegerStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Integer*);
typedef fmi2Status (__stdcall *fmi2GetBooleanStatus_t)(fmi2Component, const fmi2StatusKind, fmi2Boolean*);
typedef fmi2Status (__stdcall *fmi2GetStringStatus_t)(fmi2Component, const fmi2StatusKind, fmi2String* );

#endif // FMIC_FUNCTIONS_FMI2_H
