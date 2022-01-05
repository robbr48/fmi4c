#define MODEL_IDENTIFIER fmi3tlm

#include "fmi3Functions.h"
#include <stdbool.h>
#include <stdio.h>

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "nvector/nvector_serial.h"  /* serial N_Vector types, fcts., macros */
#include "sundials/sundials_dense.h" /* definitions DlsMat DENSE_ELEM */
#include "sundials/sundials_types.h" /* definition of type realtype */
#include "sunnonlinsol/sunnonlinsol_newton.h"
#include "sunmatrix/sunmatrix_dense.h"
#include "sunlinsol/sunlinsol_dense.h"

#define VR_V 0
#define VR_F 1
#define VR_FD 2
#define UNUSED(x)(void)(x)

#define Ith(v,i)    NV_Ith_S(v,i-1)       /* Ith numbers components 1..NEQ */
#define IJth(A,i,j) DENSE_ELEM(A,i-1,j-1) /* IJth numbers rows,cols 1..NEQ */


typedef struct {
    fmi3String instanceName;
    fmi3String instantiationToken;
    fmi3InstanceEnvironment instanceEnvironment;
    fmi3CallbackLogMessage logger;
    fmi3CallbackIntermediateUpdate intermediateUpdate;
    bool loggingOn;
    fmi3Float64 v; //Speed (output)
    fmi3Float64 f; //TLM force (input)
    fmi3Float64 fd; //Disturbance force (tunable parameter)
    SUNMatrix A;
    SUNLinearSolver LS;
    SUNNonlinearSolver NLS;
    void *mem;
    N_Vector y, abstol;
    int flag;
    size_t n_states;
    double tstart;
    double tsolver;
    double tcur;
    double reltol;
    double m;
    double c;
    double k;
} fmuContext;


int rhs(realtype t, N_Vector yy, N_Vector yp, void *user_data)
{
    fmuContext* fmu = (fmuContext*)user_data;

    fmu->intermediateUpdate(fmu->instanceEnvironment, t, fmi3False, fmi3True, fmi3True, fmi3False, fmi3False, NULL, NULL);

    Ith(yp,1) = -fmu->c/fmu->m*Ith(yy,1) - fmu->k/fmu->m*Ith(yy,2) - fmu->f/fmu->m - fmu->fd/fmu->m;
    Ith(yp,2) = Ith(yy,1);

    return(0);
}


const char* fmi3GetVersion()
{
    return fmi3Version;
}

fmi3Status fmi3SetDebugLogging(fmi3Instance instance,
                               fmi3Boolean loggingOn,
                               size_t nCategories,
                               const fmi3String categories[])
{
    UNUSED(nCategories);
    UNUSED(categories);
    fmuContext *fmu =(fmuContext*)instance;
    fmu->loggingOn = loggingOn;
    return fmi3OK;
}



fmi3Instance fmi3InstantiateCoSimulation(fmi3String instanceName,
                                         fmi3String instantiationToken,
                                         fmi3String resourcePath,
                                         fmi3Boolean visible,
                                         fmi3Boolean loggingOn,
                                         fmi3Boolean eventModeUsed,
                                         fmi3Boolean earlyReturnAllowed,
                                         const fmi3ValueReference requiredIntermediateVariables[],
                                         size_t nRequiredIntermediateVariables,
                                         fmi3InstanceEnvironment instanceEnvironment,
                                         fmi3CallbackLogMessage logMessage,
                                         fmi3CallbackIntermediateUpdate intermediateUpdate)
{
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(eventModeUsed);
    UNUSED(earlyReturnAllowed);
    UNUSED(requiredIntermediateVariables);  //!< @todo Use this argument
    UNUSED(nRequiredIntermediateVariables); //!< @todo Use this argument

    fmuContext *fmu = malloc(sizeof(fmuContext));
    fmu->instanceName = instanceName;
    fmu->instantiationToken = instantiationToken;
    fmu->instanceEnvironment = instanceEnvironment;
    fmu->logger = logMessage;
    fmu->intermediateUpdate = intermediateUpdate;
    fmu->loggingOn = loggingOn;

    fmu->n_states = 2;  //Number of states
    fmu->tstart = 0;    //Start time
    fmu->tsolver;       //Solver time
    fmu->tcur;          //Current time
    fmu->reltol = 1e-4; //Relative tolerance
    fmu->f = 0;         //Input force
    fmu->v = 0;         //Output speed
    fmu->m = 5;         //Linear inertia
    fmu->c = 10;        //Viscous friction
    fmu->k = 100;       //Spring constant
    fmu->fd = 0;        //Disturbance force

    //CVODE variables
    fmu->A = 0;         //Linear solver matrix
    fmu->LS = 0;        //Linear solver
    fmu->NLS = 0;       //Nonlinear solver

    if(fmu->loggingOn) {
        fmu->logger(fmu->instanceEnvironment, fmu->instanceName, fmi3OK, "info", "Successfully instantiated FMU");
    }

    fmu->mem = NULL;
    fmu->y = fmu->abstol = NULL;
    fmu->y = N_VNew_Serial(fmu->n_states);
    fmu->abstol = N_VNew_Serial(fmu->n_states);
    for(size_t i=0; i<fmu->n_states; ++i) {
        Ith(fmu->y,i+1) = 0;     //Start at zero position and velocity
    }
    for(size_t i=0; i<fmu->n_states; ++i) {
        Ith(fmu->abstol,i+1) = 0.01*fmu->reltol;  //Same absolute tolerance for all states for now
    }
    fmu->mem = CVodeCreate(CV_BDF);
    CVodeInit(fmu->mem, rhs, fmu->tstart, fmu->y);
    CVodeSVtolerances(fmu->mem, fmu->reltol, fmu->abstol);
    if(fmu->NLS) {
        SUNNonlinSolFree(fmu->NLS);
    }
    if(fmu->LS) {
        SUNLinSolFree(fmu->LS);
    }
    if (fmu->A) {
        SUNMatDestroy(fmu->A);
    }
    fmu->NLS = SUNNonlinSol_Newton(fmu->y);
    CVodeSetNonlinearSolver(fmu->mem, fmu->NLS);
    fmu->A = SUNDenseMatrix(fmu->n_states, fmu->n_states);
    fmu->LS = SUNLinSol_Dense(fmu->y, fmu->A);
    CVodeSetLinearSolver(fmu->mem, fmu->LS, fmu->A);
    CVodeSetMaxStep(fmu->mem, 5e-4); //!< @warning Maximum step size is hard-coded
    CVodeSetMaxNumSteps(fmu->mem, 1000000);
    CVodeSetUserData(fmu->mem, fmu);

    fmu->tsolver = fmu->tstart;
    fmu->tcur = fmu->tstart;

    fmu->logger(fmu->instanceEnvironment, fmu->instanceName, fmi3OK, 0, "Successfully instantiated FMU");

    return fmu;
}

void fmi3FreeInstance(fmi3Instance instance)
{
    fmuContext *fmu = (fmuContext*)instance;
    free(fmu);
}

fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
                                       fmi3Boolean toleranceDefined,
                                       fmi3Float64 tolerance,
                                       fmi3Float64 startTime,
                                       fmi3Boolean stopTimeDefined,
                                       fmi3Float64 stopTime)
{
    UNUSED(toleranceDefined);
    UNUSED(tolerance);
    UNUSED(startTime);
    UNUSED(stopTimeDefined);
    UNUSED(stopTime);
    UNUSED(instance);
    return fmi3OK;
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance)
{
    UNUSED(instance);
    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance,
                              fmi3Boolean stepEvent,
                              fmi3Boolean stateEvent,
                              const fmi3Int32 rootsFound[],
                              size_t nEventIndicators,
                              fmi3Boolean timeEvent)
{
    UNUSED(instance);
    UNUSED(stepEvent);
    UNUSED(stateEvent);
    UNUSED(rootsFound);
    UNUSED(nEventIndicators);
    UNUSED(timeEvent);
    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3Terminate(fmi3Instance instance)
{
    UNUSED(instance);
    //! @todo Something needed here?
    return fmi3OK;
}

fmi3Status fmi3Reset(fmi3Instance instance)
{
    fmuContext *fmu = (fmuContext*)instance;
    fmu->v = 0;
    fmu->f = 0;
    fmu->fd = 0;

    //! @todo Reset CVODE somehow?
    return fmi3OK;
}


fmi3Status fmi3GetFloat64(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Float64 values[],
                          size_t nValues)
{

    UNUSED(nValues);
    fmuContext *fmu = (fmuContext *)instance;

    if(nValueReferences == 1 && valueReferences[0] == 0) {
        values[0] = Ith(fmu->y,1);
    }
    else if(nValueReferences == 1 && valueReferences[0] == 1) {
        values[0] = fmu->f;
    }
    else if(nValueReferences == 1 && valueReferences[0] == 2) {
        values[0] = fmu->fd;
    }
    return fmi3OK;
}

fmi3Status fmi3SetFloat64(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Float64 values[],
                          size_t nValues)
{
    UNUSED(nValues);

    fmuContext *fmu = (fmuContext *)instance;

    if(nValueReferences == 1 && valueReferences[0] == 1) {
        fmu->f = values[0];
    }
    else if(nValueReferences == 1 && valueReferences[0] == 2) {
        fmu->fd = values[0];
    }
    return fmi3OK;
}


fmi3Status fmi3EnterStepMode(fmi3Instance instance)
{
    UNUSED(instance);
    return fmi3OK;
}


fmi3Status fmi3DoStep(fmi3Instance instance,
                      fmi3Float64 currentCommunicationPoint,
                      fmi3Float64 communicationStepSize,
                      fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                      fmi3Boolean* eventEncountered,
                      fmi3Boolean* terminateSimulation,
                      fmi3Boolean* earlyReturn,
                      fmi3Float64* lastSuccessfulTime)
{
    UNUSED(noSetFMUStatePriorToCurrentPoint);
    UNUSED(eventEncountered);
    UNUSED(terminateSimulation);
    UNUSED(earlyReturn);
    UNUSED(lastSuccessfulTime);

    fmuContext *fmu = (fmuContext *)instance;

    while(fmu->tsolver < currentCommunicationPoint+communicationStepSize) {
        CVode(fmu->mem, currentCommunicationPoint+communicationStepSize, fmu->y, &fmu->tsolver, CV_NORMAL);
        fmu->intermediateUpdate(fmu->instanceEnvironment, fmu->tsolver, fmi3False, fmi3True, fmi3True, fmi3True, fmi3False, NULL, NULL);
    }

    return fmi3OK;
}
