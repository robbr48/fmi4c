#define MODEL_IDENTIFIER fmi1cs

#include "fmiFunctions.h"
#include "fmiPlatformTypes.h"
#include <stdbool.h>
#include <stdio.h>

#define VR_DX 1
#define VR_X 2
#define UNUSED(x)(void)(x)
//

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

    fmuContext *fmu = malloc(sizeof(fmuContext));
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
                                               const  fmiReal value[]);

DllExport fmiStatus fmiGetRealOutputDerivatives(fmiComponent c,
                                                const   fmiValueReference vr[],
                                                size_t  nvr,
                                                const   fmiInteger order[],
                                                fmiReal value[]);

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

