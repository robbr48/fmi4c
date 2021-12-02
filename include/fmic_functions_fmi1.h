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
typedef const char* (__stdcall *fmiGetTypesPlatform_t)();
typedef const char* (__stdcall *fmiGetVersion_t)();
typedef fmi1Status (__stdcall *fmiSetDebugLogging_t)(fmi1Component, fmi1Boolean);
typedef fmi1Status (__stdcall *fmiGetReal_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1Real[]);
typedef fmi1Status (__stdcall *fmiGetInteger_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1Integer[]);
typedef fmi1Status (__stdcall *fmiGetBoolean_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1Boolean[]);
typedef fmi1Status (__stdcall *fmiGetString_t)(fmi1Component, const fmi1ValueReference[], size_t, fmi1String []);
typedef fmi1Status (__stdcall *fmiSetReal_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1Real[]);
typedef fmi1Status (__stdcall *fmiSetInteger_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1Integer[]);
typedef fmi1Status (__stdcall *fmiSetBoolean_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1Boolean[]);
typedef fmi1Status (__stdcall *fmiSetString_t)(fmi1Component, const fmi1ValueReference[], size_t, const fmi1String[]);
typedef fmi1Component (__stdcall *fmiInstantiateSlave_t)(fmi1String, fmi1String, fmi1String, fmi1String, fmi1Real, fmi1Boolean,
                                                          fmi1Boolean, fmi1CallbackFunctionsCoSimulation, fmi1Boolean);
typedef fmi1Status (__stdcall *fmiInitializeSlave_t)(fmi1Component,  fmi1Real, fmi1Boolean, fmi1Real);
typedef fmi1Status (__stdcall *fmiTerminateSlave_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmiResetSlave_t)(fmi1Component);
typedef void (__stdcall *fmiFreeSlaveInstance_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmiSetRealInputDerivatives_t)(fmi1Component, const fmi1ValueReference[], size_t,
                                                              const fmi1Integer[], const fmi1Real[]);
typedef fmi1Status (__stdcall *fmiGetRealOutputDerivatives_t)(fmi1Component, const fmi1ValueReference[], size_t,
                                                               const fmi1Integer[], fmi1Real[]);
typedef fmi1Status (__stdcall *fmiCancelStep_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmiDoStep_t)(fmi1Component, fmi1Real, fmi1Real, fmi1Boolean);
typedef fmi1Status (__stdcall *fmiGetStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Status*);
typedef fmi1Status (__stdcall *fmiGetRealStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Real*);
typedef fmi1Status (__stdcall *fmiGetIntegerStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Integer*);
typedef fmi1Status (__stdcall *fmiGetBooleanStatus_t)(fmi1Component, const fmi1StatusKind, fmi1Boolean*);
typedef fmi1Status (__stdcall *fmiGetStringStatus_t)(fmi1Component, const fmi1StatusKind, fmi1String*);
typedef const char* (__stdcall *fmiGetModelTypesPlatform_t)();
typedef fmi1Component (__stdcall *fmiInstantiateModel_t)(fmi1String, fmi1String,
                                                          fmi1CallbackFunctionsModelExchange, fmi1Boolean);
typedef void (__stdcall *fmiFreeModelInstance_t)(fmi1Component);
typedef fmi1Status (__stdcall *fmiSetTime_t)(fmi1Component, fmi1Real);
typedef fmi1Status (__stdcall *fmiSetContinuousStates_t)(fmi1Component, const fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmiCompletedIntegratorStep_t)(fmi1Component, fmi1Boolean*);
typedef fmi1Status (__stdcall *fmiInitialize_t)(fmi1Component, fmi1Boolean, fmi1Real, fmi1EventInfo*);
typedef fmi1Status (__stdcall *fmiGetDerivatives_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmiGetEventIndicators_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmiEventUpdate_t)(fmi1Component, fmi1Boolean, fmi1EventInfo*);
typedef fmi1Status (__stdcall *fmiGetContinuousStates_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmiGetNominalContinuousStates_t)(fmi1Component, fmi1Real[], size_t);
typedef fmi1Status (__stdcall *fmiGetStateValueReferences_t)(fmi1Component, fmi1ValueReference[], size_t);
typedef fmi1Status (__stdcall *fmiTerminate_t)(fmi1Component);

#endif // FMIC_FUNCTIONS_FMI1_H
