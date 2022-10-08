#define MODEL_IDENTIFIER fmi1cs

#include "fmiFunctions.h"
#include "fmiPlatformTypes.h"
#include "fmi4c_common.h"
#include <stdbool.h>
#include <stdio.h>

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


DllExport const char* fmiGetTypesPlatform() {
    return fmiPlatform;
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

DllExport fmiStatus fmiGetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiSetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[]) {
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

DllExport fmiStatus fmiSetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiSetString(fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString value[]) {
    UNUSED(c);
    UNUSED(vr);
    UNUSED(nvr);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiComponent fmiInstantiateSlave(fmiString  instanceName,
                                           fmiString  fmuGUID,
                                           fmiString  fmuLocation,
                                           fmiString  mimeType,
                                           fmiReal    timeout,
                                           fmiBoolean visible,
                                           fmiBoolean interactive,
                                           fmiCallbackFunctions functions,
                                           fmiBoolean loggingOn) {
    UNUSED(fmuLocation);
    UNUSED(mimeType);
    UNUSED(timeout);
    UNUSED(visible);
    UNUSED(interactive);

    fmuContext *fmu = calloc(1, sizeof(fmuContext));
    fmu->instanceName = instanceName;
    fmu->guid = fmuGUID;
    fmu->callbacks = functions;
    fmu->loggingOn = loggingOn;

    return fmu;
}

DllExport fmiStatus fmiInitializeSlave(fmiComponent c,
                                       fmiReal      tStart,
                                       fmiBoolean   StopTimeDefined,
                                       fmiReal      tStop) {
    UNUSED(tStart);
    UNUSED(StopTimeDefined);
    UNUSED(tStop);
    fmuContext *fmu =(fmuContext*)c;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmiOK;
}

DllExport fmiStatus fmiTerminateSlave(fmiComponent c) {
    UNUSED(c);
    return fmiOK;
}

DllExport fmiStatus fmiResetSlave(fmiComponent c) {
    fmuContext *fmu =(fmuContext*)c;
    fmu->x = 0;
    fmu->xold = 0;
    fmu->dxold = 0;
    return fmiOK;
}

DllExport void fmiFreeSlaveInstance(fmiComponent c) {
    fmuContext *fmu =(fmuContext*)c;
    free(fmu);
}

DllExport fmiStatus fmiSetRealInputDerivatives(fmiComponent c,
                                               const  fmiValueReference vr[],
                                               size_t nvr,
                                               const  fmiInteger order[],
                                               const  fmiReal value[]) {
    return fmiError;
}

DllExport fmiStatus fmiGetRealOutputDerivatives(fmiComponent c,
                                                const   fmiValueReference vr[],
                                                size_t  nvr,
                                                const   fmiInteger order[],
                                                fmiReal value[]) {
    return fmiError;
}

DllExport fmiStatus fmiDoStep(fmiComponent c,
                                  fmiReal      currentCommunicationPoint,
                                  fmiReal      communicationStepSize,
                                  fmiBoolean   newStep) {
    UNUSED(currentCommunicationPoint);
    UNUSED(newStep);

    fmuContext *fmu =(fmuContext *)c;

    //Integrate(trapezoid rule)
    fmu->x = fmu->xold + communicationStepSize/2.0*(fmu->dx + fmu->dxold);
    fmu->xold = fmu->x;
    fmu->dxold = fmu->dx;

    return fmiOK;
}

DllExport fmiStatus fmiCancelStep(fmiComponent c) {
    UNUSED(c);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetStatus(fmiComponent c, const fmiStatusKind s, fmiStatus* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetRealStatus(fmiComponent c, const fmiStatusKind s, fmiReal* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetIntegerStatus(fmiComponent c, const fmiStatusKind s, fmiInteger* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetBooleanStatus(fmiComponent c, const fmiStatusKind s, fmiBoolean* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}

DllExport fmiStatus fmiGetStringStatus(fmiComponent c, const fmiStatusKind s, fmiString* value) {
    UNUSED(c);
    UNUSED(s);
    UNUSED(value);
    return fmiWarning;  //This function should not be called
}
