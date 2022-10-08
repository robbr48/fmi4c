#define MODEL_IDENTIFIER fmi2

#include "fmi2Functions.h"
#include "fmi4c_common.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define VR_DX 1
#define VR_X 2

typedef struct {
    fmi2String instanceName;
    fmi2String guid;
    fmi2Type type;
    const fmi2CallbackFunctions *callbacks;
    bool loggingOn;
    fmi2Real dx;  //Input
    fmi2Real dxold;  //Delayed variable(internal)
    fmi2Real x;   //Output(integrated value)
    fmi2Real xold;   //Delayed variable(internal)
} fmuContext;


const char* fmi2GetTypesPlatform(void) {
    return fmi2TypesPlatform;
}

const char* fmi2GetVersion(void) {
    return fmi2Version;
}

fmi2Status  fmi2SetDebugLogging(fmi2Component c, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[]) {
    UNUSED(nCategories);
    UNUSED(categories);
    fmuContext *fmu = (fmuContext*)c;
    fmu->loggingOn = loggingOn;
    return fmi2OK;
}

/* Creation and destruction of FMU instances and setting debug status */
fmi2Component fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID, fmi2String fmuResourceLocation, const fmi2CallbackFunctions* functions, fmi2Boolean visible, fmi2Boolean loggingOn)
{
    UNUSED(fmuResourceLocation);
    UNUSED(visible);

    fmuContext *fmu = calloc(1, sizeof(fmuContext));
    fmu->instanceName = _strdup(instanceName);
    fmu->guid = _strdup(fmuGUID);
    fmu->type = fmuType;
    fmu->callbacks = functions;
    fmu->loggingOn = loggingOn;

    if(fmu->loggingOn) {
        //fmu->callbacks->logger(fmu->callbacks->componentEnvironment, fmu->instanceName, fmi2OK, "info", "Successfully instantiated FMU");
    }

    return fmu;
}

void fmi2FreeInstance(fmi2Component c) {
    fmuContext *fmu = (fmuContext*)c;
    free((char*)fmu->instanceName);
    free((char*)fmu->guid);
    free(fmu);
}

/* Enter and exit initialization mode, terminate and reset */
fmi2Status fmi2SetupExperiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime) {
    UNUSED(toleranceDefined);
    UNUSED(tolerance);
    UNUSED(startTime);
    UNUSED(stopTimeDefined);
    UNUSED(stopTime);
    fmuContext *fmu = (fmuContext*)c;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmi2OK;
}

fmi2Status fmi2EnterInitializationMode(fmi2Component c) {
    UNUSED(c);
    return fmi2OK; //Nothing to do
}

fmi2Status fmi2ExitInitializationMode(fmi2Component c) {
    UNUSED(c);
    return fmi2OK; //Nothing to do
}

fmi2Status fmi2Terminate(fmi2Component c) {
    UNUSED(c);
    return fmi2OK; //Nothing to do
}

fmi2Status fmi2Reset(fmi2Component c) {
    fmuContext *fmu = (fmuContext*)c;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmi2OK;
}

fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]) {
    fmuContext *fmu =(fmuContext*)c;
    fmi2Status status = fmi2OK;
    for(size_t i=0; i<nvr; ++i) {
        switch(vr[i]) {
        case VR_X:
            value[i] = fmu->x;
            break;
        case VR_DX:
            value[i] = fmu->dx;
            break;
        default:
            status = fmi2Warning;  // Non-existing value reference;
        }
    }

    return status;
}

fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]) {
    fmuContext *fmu =(fmuContext*)c;
    fmi2Status status = fmi2OK;
    for(size_t i=0; i<nvr; ++i) {
        switch(vr[i]) {
        case VR_X:
            fmu->x = value[i];
            break;
        case VR_DX:
            fmu->dx = value[i];
            break;
        default:
            status = fmi2Warning;  // Non-existing value reference;
        }
    }

    return status;
}

fmi2Status fmi2GetInteger(fmi2Component c,
                          const fmi2ValueReference vr[],
                          size_t nvr,
                          fmi2Integer value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetBoolean(fmi2Component c,
                          const fmi2ValueReference vr[],
                          size_t nvr,
                          fmi2Boolean value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetString(fmi2Component c,
                         const fmi2ValueReference vr[],
                         size_t nvr,
                         fmi2String value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2SetInteger(fmi2Component c,
                          const fmi2ValueReference vr[],
                          size_t nvr,
                          const fmi2Integer value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2SetBoolean(fmi2Component c,
                          const fmi2ValueReference vr[],
                          size_t nvr,
                          const fmi2Boolean value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2SetString(fmi2Component c,
                         const fmi2ValueReference vr[],
                         size_t nvr,
                         const fmi2String value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmi2Warning;
}

//Model exchange
fmi2Status fmi2EnterEventMode(fmi2Component c)
{
    UNUSED(c);
    return fmi2OK;  //Nothing to do
}

fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo* eventInfo)
{
    UNUSED(c);
    eventInfo->newDiscreteStatesNeeded = fmi2False;
    eventInfo->nextEventTime = -0.0;
    eventInfo->nextEventTime = fmi2False;
    eventInfo->nominalsOfContinuousStatesChanged = fmi2False;
    eventInfo->terminateSimulation = fmi2False;
    eventInfo->valuesOfContinuousStatesChanged = fmi2False;
    return fmi2OK;
}

fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
    UNUSED(c);
    return fmi2OK;  //Nothing to do
}

fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
                                       fmi2Boolean noSetFMUStatePriorToCurrentPoint,
                                       fmi2Boolean* enterEventMode,
                                       fmi2Boolean* terminateSimulation)
{
    UNUSED(c);
    UNUSED(noSetFMUStatePriorToCurrentPoint);
    *enterEventMode = fmi2False;
    *terminateSimulation = fmi2False;
    return fmi2OK;
}

fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time)
{
    UNUSED(c);
    UNUSED(time);
    return fmi2OK;  //Nothing to do
}

fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real states[], size_t nStates)
{
    fmuContext *fmu = (fmuContext *)c;
    if(nStates > 0) {
        fmu->x = states[0];
        return fmi2OK;
    }
    return fmi2Warning; //Too few states were given
}

fmi2Status fmi2GetDerivatives(fmi2Component c, fmi2Real derivatives[], size_t nx)
{
    fmuContext *fmu = (fmuContext *)c;
    if(nx > 0) {
        derivatives[0] = fmu->dx;
        return fmi2OK;
    }
    return fmi2Warning; //Asked for derivatives with too short array
}

fmi2Status fmi2GetEventIndicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni)
{
    fmuContext *fmu = (fmuContext *)c;
    if(ni > 0) {
        // Not sure what to set here, but setting 0 to prevent use of uninitialized memory warning in valgrind
        eventIndicators[0] = 0;
        return fmi2OK;
    }
    return fmi2Warning; //Asked for eventIndicators with too short array
}

fmi2Status fmi2GetContinuousStates(fmi2Component c, fmi2Real states[], size_t nStates)
{
    fmuContext *fmu = (fmuContext *)c;
    if(nStates > 0) {
        states[0] = fmu->x;
        return fmi2OK;
    }
    return fmi2Warning; //Asked for states with too short array
}

fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c, fmi2Real x_nominal[], size_t nx)
{
    UNUSED(c);
    UNUSED(x_nominal);
    UNUSED(nx);
    return fmi2OK;  //Nothing to do
}

fmi2Status fmi2DoStep(fmi2Component c, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint) {
    UNUSED(currentCommunicationPoint);
    UNUSED(noSetFMUStatePriorToCurrentPoint);

    fmuContext *fmu =(fmuContext *)c;

    //Integrate(trapezoid rule)
    fmu->x = fmu->xold + communicationStepSize/2.0*(fmu->dx + fmu->dxold);
    fmu->xold = fmu->x;
    fmu->dxold = fmu->dx;

    return fmi2OK;
}

fmi2Status fmi2GetFMUstate(fmi2Component c,
                           fmi2FMUstate* FMUstate) {
    UNUSED(c);
    UNUSED(FMUstate);
    return fmi2Warning;
}

fmi2Status fmi2SetFMUstate(fmi2Component c,
                           fmi2FMUstate FMUstate) {
    UNUSED(c);
    UNUSED(FMUstate);
    return fmi2Warning;
}

fmi2Status fmi2FreeFMUstate(fmi2Component c,
                            fmi2FMUstate* FMUstate) {
    UNUSED(c);
    UNUSED(FMUstate);
    return fmi2Warning;
}

fmi2Status fmi2SerializedFMUstateSize(fmi2Component c,
                                      fmi2FMUstate FMUstate,
                                      size_t* size) {
    UNUSED(c);
    UNUSED(FMUstate);
    UNUSED(size);
    return fmi2Warning;
}

fmi2Status fmi2SerializeFMUstate(fmi2Component c,
                                 fmi2FMUstate FMUstate,
                                 fmi2Byte serializedState[],
                                 size_t size) {
    UNUSED(c);
    UNUSED(FMUstate);
    UNUSED(serializedState);
    UNUSED(size);
    return fmi2Warning;
}

fmi2Status fmi2DeSerializeFMUstate(fmi2Component c,
                                   const fmi2Byte serializedState[],
                                   size_t size,
                                   fmi2FMUstate* FMUstate) {
    UNUSED(c);
    UNUSED(serializedState);
    UNUSED(size);
    UNUSED(FMUstate);
    return fmi2Warning;
}

fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
                                        const fmi2ValueReference vUnknownRef[],
                                        size_t nUnknown,
                                        const fmi2ValueReference vKnownRef[],
                                        size_t nKnown,
                                        const fmi2Real dvKnown[],
                                        fmi2Real dvUnknown[]) {
    UNUSED(c);
    UNUSED(vUnknownRef);
    UNUSED(nUnknown);
    UNUSED(vKnownRef);
    UNUSED(nKnown);
    UNUSED(dvKnown);
    UNUSED(dvUnknown);
    return fmi2Warning;
}

fmi2Status fmi2SetRealInputDerivatives(fmi2Component c,
                                       const fmi2ValueReference vr[],
                                       size_t nvr,
                                       const fmi2Integer order[],
                                       const fmi2Real value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(order);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c,
                                        const fmi2ValueReference vr[],
                                        size_t nvr,
                                        const fmi2Integer order[],
                                        fmi2Real value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(order);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2CancelStep(fmi2Component c) {
    UNUSED(c);
    return fmi2Warning;
}

fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmi2Warning;
}

fmi2Status fmi2GetStringStatus(fmi2Component c,
                               const fmi2StatusKind s,
                               fmi2String* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmi2Warning;
}
