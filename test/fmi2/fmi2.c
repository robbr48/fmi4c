#define MODEL_IDENTIFIER fmi2

#include "fmi2Functions.h"
#include <stdbool.h>
#include <stdio.h>

#define VR_DX 1
#define VR_X 2
#define UNUSED(x)(void)(x)

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
}

/* Creation and destruction of FMU instances and setting debug status */
fmi2Component fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID, fmi2String fmuResourceLocation, const fmi2CallbackFunctions* functions, fmi2Boolean visible, fmi2Boolean loggingOn)
{
    UNUSED(fmuResourceLocation);
    UNUSED(visible);

    fmuContext *fmu = malloc(sizeof(fmuContext));
    fmu->instanceName = instanceName;
    fmu->guid = fmuGUID;
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
    enterEventMode = fmi2False;
    terminateSimulation = fmi2False;
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
   UNUSED(c);
   UNUSED(eventIndicators);
   UNUSED(ni);
   return fmi2OK;   //Nothing to do
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
