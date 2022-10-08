#define MODEL_IDENTIFIER fmi1me

#include "fmiModelFunctions.h"
#include "fmi4c_common.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define VR_DX 1
#define VR_X 2

typedef struct {
    fmiString instanceName;
    fmiString guid;
    fmiCallbackFunctions callbacks;
    bool loggingOn;
    fmiReal dx;  //Input
    fmiReal dxold;  //Delayed variable(internal)
    fmiReal x;   //Output(integrated value)
    fmiReal xold;   //Delayed variable(internal)
} fmuContext;


DllExport const char* fmiGetModelTypesPlatform() {
    return fmiModelTypesPlatform;
}

DllExport const char* fmiGetVersion() {
    return fmiVersion;
}

DllExport fmiStatus fmiSetDebugLogging(fmiComponent c, fmiBoolean loggingOn) {
    fmuContext *fmu =(fmuContext*)c;
    fmu->loggingOn = loggingOn;
    return fmiOK;
}

/* Data Exchange Functions*/
DllExport fmiStatus fmiGetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal    value[]) {
    fmuContext *fmu =(fmuContext*)c;
    fmiStatus status = fmiOK;
    for(size_t i=0; i<nvr; ++i) {
        switch(vr[i]) {
        case VR_X:
            value[i] = fmu->x;
            break;
        case VR_DX:
            value[i] = fmu->dx;
            break;
        default:
            status = fmiWarning;  // Non-existing value reference;
        }
    }

    return status;
}

DllExport fmiStatus fmiGetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}
DllExport fmiStatus fmiGetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiSetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal    value[]) {
    fmuContext *fmu =(fmuContext*)c;
    fmiStatus status = fmiOK;
    for(size_t i=0; i<nvr; ++i) {
        switch(vr[i]) {
        case VR_X:
            fmu->x = value[i];
            break;
        case VR_DX:
            fmu->dx = value[i];
            break;
        default:
            status = fmiWarning;  // Non-existing value reference;
        }
    }

    return status;
}

DllExport fmiStatus fmiSetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiSetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiSetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiComponent fmiInstantiateModel(fmiString  instanceName,
                                           fmiString  fmuGUID,
                                           fmiCallbackFunctions functions,
                                           fmiBoolean loggingOn)
{
    fmuContext *fmu = calloc(1, sizeof(fmuContext));
    fmu->instanceName = _strdup(instanceName);
    fmu->guid = _strdup(fmuGUID);
    fmu->callbacks = functions;
    fmu->loggingOn = loggingOn;

    if(fmu->loggingOn) {
        fmu->callbacks.logger(fmu, fmu->instanceName, fmiOK, "info", "Successfully instantiated FMU");
    }

    return fmu;
}

DllExport void fmiFreeModelInstance(fmiComponent c)
{
    fmuContext *fmu = (fmuContext*)c;
    free((char*)fmu->instanceName);
    free((char*)fmu->guid);
    free(fmu);
}

DllExport fmiStatus fmiInitialize(fmiComponent c,
                                  fmiBoolean toleranceControlled,
                                  fmiReal relativeTolerance,
                                  fmiEventInfo *eventInfo) {
    UNUSED(toleranceControlled);
    UNUSED(relativeTolerance);
    UNUSED(eventInfo);
    fmuContext *fmu =(fmuContext*)c;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmiOK;
}

DllExport fmiStatus fmiTerminate(fmiComponent c) {
    UNUSED(c);
    return fmiOK;
}

DllExport fmiStatus fmiGetDerivatives(fmiComponent c, fmiReal derivatives[], size_t nx)
{
    fmuContext *fmu = (fmuContext *)c;
    if(nx > 0) {
        derivatives[0] = fmu->dx;
        return fmiOK;
    }
    return fmiWarning; //Asked for derivatives with too short array
}

DllExport fmiStatus fmiSetTime(fmiComponent c, fmiReal time)
{
    UNUSED(c);
    UNUSED(time);
    return fmiOK;  //Nothing to do
}

DllExport fmiStatus fmiSetContinuousStates(fmiComponent c, const fmiReal x[], size_t nx)
{
    fmuContext *fmu = (fmuContext *)c;
    if(nx > 0) {
        fmu->x = x[0];
        return fmiOK;
    }
    return fmiWarning; //Too few states were given
}

DllExport fmiStatus fmiCompletedIntegratorStep(fmiComponent c, fmiBoolean *callEventUpdate)
{
    UNUSED(c);
    UNUSED(callEventUpdate);
    return fmiOK;  //Nothing to do
}

DllExport fmiStatus fmiGetEventIndicators(fmiComponent c, fmiReal eventIndicators[], size_t ni)
{
    UNUSED(c);
    UNUSED(eventIndicators);
    UNUSED(ni);
    return fmiOK;   //Nothing to do
}

DllExport fmiStatus fmiEventUpdate(fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo *eventInfo)
{
    UNUSED(c);
    UNUSED(intermediateResults);
    UNUSED(eventInfo);
    return fmiOK;   //Nothing to do
}

DllExport fmiStatus fmiGetContinuousStates(fmiComponent c, fmiReal states[], size_t nx)
{
    fmuContext *fmu = (fmuContext *)c;
    if(nx > 0) {
        states[0] = fmu->x;
        return fmiOK;
    }
    return fmiWarning; //Asked for states with too short array
}

DllExport fmiStatus fmiGetNominalContinuousStates(fmiComponent c, fmiReal x_nominal[], size_t nx)
{
    UNUSED(c);
    UNUSED(x_nominal);
    UNUSED(nx);
    return fmiOK;   //Nothing to do
}

DllExport fmiStatus fmiGetStateValueReferences(fmiComponent c, fmiValueReference vrx[], size_t nx)
{
    if(nx > 0) {
        vrx[0] = VR_X;
        return fmiOK;
    }
    return fmiWarning;  //Asked for value references with too short array
}
