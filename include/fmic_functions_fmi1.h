#ifndef FMIC_FUNCTIONS_FMI1_H
#define FMIC_FUNCTIONS_FMI1_H

// Callback functions
typedef void (*fmi1CallbackLogger_t)(fmi1Component,
                                     fmi1String instanceName,
                                     fmi1Status status,
                                     fmi1String category,
                                     fmi1String message,
                                     ...);
typedef void* (*fmi1CallbackAllocateMemory_t)(size_t nobj, size_t size);
typedef void (*fmi1CallbackFreeMemory_t)(void* obj);
typedef void  (*fmi1StepFinished_t)(fmi1Component, fmi1Status status);


// Structs
typedef struct {
    fmi1CallbackLogger_t logger;
    fmi1CallbackAllocateMemory_t allocateMemory;
    fmi1CallbackFreeMemory_t freeMemory;
} fmi1CallbackFunctionsModelExchange;


typedef struct {
    fmi1CallbackLogger_t logger;
    fmi1CallbackAllocateMemory_t allocateMemory;
    fmi1CallbackFreeMemory_t freeMemory;
    fmi1StepFinished_t stepFinished;
} fmi1CallbackFunctionsCoSimulation;

typedef struct {
    fmi1Boolean iterationConverged;
    fmi1Boolean stateValueReferencesChanged;
    fmi1Boolean stateValuesChanged;
    fmi1Boolean terminateSimulation;
    fmi1Boolean upcomingTimeEvent;
    fmi1Real nextEventTime;
} fmi1EventInfo;

// API functions
typedef const char* (__stdcall *fmi1GetTypesPlatform_t)();
typedef const char* (__stdcall *fmi1GetVersion_t)();
typedef fmi1Status (__stdcall *fmi1SetDebugLogging_t)(fmi1Component, fmi1Boolean);
typedef fmi1Status (__stdcall *fmi1GetReal_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1Real[]);
typedef fmi1Status (__stdcall *fmi1GetInteger_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1Integer[]);
typedef fmi1Status (__stdcall *fmi1GetBoolean_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1Boolean[]);
typedef fmi1Status (__stdcall *fmi1GetString_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1String []);
typedef fmi1Status (__stdcall *fmi1SetReal_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1Real[]);
typedef fmi1Status (__stdcall *fmi1SetInteger_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1Integer[]);
typedef fmi1Status (__stdcall *fmi1SetBoolean_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1Boolean[]);
typedef fmi1Status (__stdcall *fmi1SetString_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1String[]);
typedef fmi1Component (__stdcall *fmi1InstantiateSlave_t)(fmi1String, fmi1String, fmi1String, fmi1String, fmi1Real, fmi1Boolean,
                                                          fmi1Boolean, fmi1CallbackFunctionsCoSimulation, fmi1Boolean);
typedef fmi1Status (__stdcall *fmi1InitializeSlave_t)(fmi1Component,  fmi1Real, fmi1Boolean, fmi1Real);
typedef fmi1Status (__stdcall *fmi1TerminateSlave_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmi1ResetSlave_t)(fmi1Component);
typedef void (__stdcall *fmi1FreeSlaveInstance_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmi1SetRealInputDerivatives_t)(fmi1Component, const fmi1ValueReference[], size_t,
                                                              const fmi1Integer[], const fmi1Real[]);
typedef fmi1Status (__stdcall *fmi1GetRealOutputDerivatives_t)(fmi1Component, const fmi1ValueReference[], size_t,
                                                               const fmi1Integer[], fmi1Real[]);
typedef fmi1Status (__stdcall *fmi1CancelStep_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmi1DoStep_t)(fmi1Component, fmi1Real, fmi1Real, fmi1Boolean);
typedef fmi1Status (__stdcall *fmi1GetStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Status*);
typedef fmi1Status (__stdcall *fmi1GetRealStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Real*);
typedef fmi1Status (__stdcall *fmi1GetIntegerStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Integer*);
typedef fmi1Status (__stdcall *fmi1GetBooleanStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Boolean*);
typedef fmi1Status (__stdcall *fmi1GetStringStatus_t)(fmi1Component, const fmi1StatusKind, fmi1String*);
typedef const char* (__stdcall *fmi1GetModelTypesPlatform_t)();
typedef fmi1Component (__stdcall *fmi1InstantiateModel_t)(fmi1String, fmi1String,
                                                          fmi1CallbackFunctionsModelExchange, fmi1Boolean);
typedef void (__stdcall *fmi1FreeModelInstance_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmi1SetTime_t)(fmi1Component, fmi1Real);
typedef fmi1Status (__stdcall *fmi1SetContinuousStates_t)(fmi1Component, const fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmi1CompletedIntegratorStep_t)(fmi1Component, fmi1Boolean*);
typedef fmi1Status (__stdcall *fmi1Initialize_t)(fmi1Component, fmi1Boolean, fmi1Real, fmi1EventInfo*);
typedef fmi1Status (__stdcall *fmi1GetDerivatives_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmi1GetEventIndicators_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmi1EventUpdate_t)(fmi1Component, fmi1Boolean, fmi1EventInfo*);
typedef fmi1Status (__stdcall *fmi1GetContinuousStates_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmi1GetNominalContinuousStates_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmi1GetStateValueReferences_t)(fmi1Component, fmi1ValueReference[], size_t);
typedef fmi1Status (__stdcall *fmi1Terminate_t)(fmi1Component);

#endif // FMIC_FUNCTIONS_FMI1_H
