#define MODEL_IDENTIFIER fmi3tlm

#include "fmi3Functions.h"
#include "fmi4c_common.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "cvode/cvode.h"             /* prototypes for CVODE fcts., consts. */
#include "cvode/cvode_serialization.h"
#include "cvode/cvode_dense.h"
#include "nvector/nvector_serial.h"  /* serial FMIC_N_Vector types, fcts., macros */
#include "sundials/sundials_dense.h" /* definitions DlsMat DENSE_ELEM */
#include "sundials/sundials_math.h"
#include "sundials/sundials_types.h" /* definition of type realtype */

#define VR_V 0
#define VR_F 1
#define VR_FD 2

#define Ith(v,i)    NV_Ith_S(v,i-1)       /* Ith numbers components 1..NEQ */
#define IJth(A,i,j) DENSE_ELEM(A,i-1,j-1) /* IJth numbers rows,cols 1..NEQ */

#define LOG_SIZE 1000

//#define TLMVARIABLE

typedef struct {
    fmi3String instanceName;
    fmi3String instantiationToken;
    fmi3InstanceEnvironment instanceEnvironment;
    fmi3LogMessageCallback logger;
    fmi3IntermediateUpdateCallback intermediateUpdate;
    bool loggingOn;
    fmi3Float64 v; //Speed (output)
    fmi3Float64 f; //TLM force (input)
    fmi3Float64 fd; //Disturbance force (tunable parameter)
    fmi3Float64 hfactor;    //Step-size factor
    FMIC_N_Vector abstol;
    int flag;
    size_t n_states;
    double tstart;
    double tcur;
    double reltol;
    double m;
    double c;
    double k;
    FMIC_N_Vector y;
    FMIC_N_Vector y_backup;
    double tsolver;
    double tsolver_backup;
    void *mem;
    void *mem_backup;
    double solverStep;
    double minStepSize;
    bool steadyState;                   //! @todo Initialize (false)
    double lastFlow;
    double tlog[LOG_SIZE];
    double wlog[LOG_SIZE];
    size_t logstart;                    //! @todo Initialize (0)
    size_t logend;                      //! @todo Initialize (0)
    double steadyStateWindowLength;     //! @todo Initialize (0.1)
    double steadyStateAbsolutLimit;     //! @todo Initialize (0.00001)
    double lastNonSteadyStateTime;      //! @todo Initialize (0)
    double steadyStateRatioLimit;       //! @todo Initialize (0.01)
    double wdevsq;                      //! @todo Initialize (0)
    double meanFlow;                    //! @todo Initialize (0)
    double var2;                        //! @todo Initialize (0)
    double wdevsq_backup;
    double meanFlow_backup;
    double var2_backup;
} fmuContext;


int rhs(realtype t, FMIC_N_Vector yy, FMIC_N_Vector yp, void *user_data)
{
    fmuContext* fmu = (fmuContext*)user_data;

    fmu->intermediateUpdate(fmu->instanceEnvironment, t, fmi3True, fmi3True, fmi3False, fmi3False, NULL, NULL);

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
                                         fmi3LogMessageCallback logMessage,
                                         fmi3IntermediateUpdateCallback intermediateUpdate)
{
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(eventModeUsed);
    UNUSED(earlyReturnAllowed);
    UNUSED(requiredIntermediateVariables);  //!< @todo Use this argument
    UNUSED(nRequiredIntermediateVariables); //!< @todo Use this argument

    fmuContext *fmu = calloc(1, sizeof(fmuContext));
    fmu->instanceName = instanceName;
    fmu->instantiationToken = instantiationToken;
    fmu->instanceEnvironment = instanceEnvironment;
    fmu->logger = logMessage;
    fmu->intermediateUpdate = intermediateUpdate;
    fmu->loggingOn = loggingOn;

    fmu->n_states = 2;  //Number of states
    fmu->tstart = 0;    //Start time
    fmu->reltol = 1e-4; //Relative tolerance
    fmu->f = 0;         //Input force
    fmu->v = 0;         //Output speed
    fmu->m = 5;         //Linear inertia
    fmu->c = 10;        //Viscous friction
    fmu->k = 100;       //Spring constant
    fmu->fd = 0;        //Disturbance force
    fmu->minStepSize = 1e-4;
    fmu->solverStep = 1e-4;
    fmu->hfactor = 1;

    if(fmu->loggingOn) {
        fmu->logger(fmu->instanceEnvironment, fmi3OK, "info", "Successfully instantiated FMU");
    }

    fmu->mem = NULL;
    fmu->y = fmu->abstol = NULL;
    fmu->y = FMIC_N_VNew_Serial(fmu->n_states);
    fmu->abstol = FMIC_N_VNew_Serial(fmu->n_states);
    for(size_t i=0; i<fmu->n_states; ++i) {
        Ith(fmu->y,i+1) = 0;     //Start at zero position and velocity
    }
    for(size_t i=0; i<fmu->n_states; ++i) {
        Ith(fmu->abstol,i+1) = 0.01*fmu->reltol;  //Same absolute tolerance for all states for now
    }
    fmu->y_backup = FMIC_N_VClone(fmu->y);

    fmu->mem = CVodeCreate(CV_BDF, CV_NEWTON);
    if(fmu->mem == NULL) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to allocate memory for CVODE solver");
        free(fmu);
        return NULL;
    }

    int flag;
    flag = CVodeInit(fmu->mem, rhs, fmu->tstart, fmu->y);
    if(flag != CV_SUCCESS) {
        char errstr[1024];
        sprintf(errstr, "Failed to initialize CVODE solver, error code: %i", flag);
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, errstr);
        free(fmu);
        return NULL;
    }

    flag = CVodeSVtolerances(fmu->mem, fmu->reltol, fmu->abstol);
    if(flag != CV_SUCCESS) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to set CVODE tolerances");
        free(fmu);
        return NULL;
    }
    flag = CVDense(fmu->mem, fmu->n_states);
    if(flag != CV_SUCCESS) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to specify linear (dense) solver for CVODE");
        free(fmu);
        return NULL;
    }
    flag = CVodeSetMinStep(fmu->mem, 0);
    if(flag != CV_SUCCESS) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to set minimum step for CVODE solver");
        free(fmu);
        return NULL;
    }
    CVodeSetMaxStep(fmu->mem, fmu->solverStep); //!< @todo Initialize!
    if(flag != CV_SUCCESS) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to set maximum step for CVODE solver");
        free(fmu);
        return NULL;
    }
    CVodeSetUserData(fmu->mem, fmu);
    if(flag != CV_SUCCESS) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to set user data pointer for CVODE solver");
        free(fmu);
        return NULL;
    }
    CVodeSetMaxNumSteps(fmu->mem, 1000000);
    if(flag != CV_SUCCESS) {
        fmu->logger(fmu->instanceEnvironment, fmi3Error, 0, "Failed to set maximum number of steps for CVODE solver");
        free(fmu);
        return NULL;
    }

    fmu->tsolver = fmu->tstart;
    fmu->tcur = fmu->tstart;

    fmu->logger(fmu->instanceEnvironment, fmi3OK, 0, "Successfully instantiated FMU");

    return fmu;
}

fmi3Instance fmi3InstantiateModelExchange(fmi3String instanceName,
                                          fmi3String instantiationToken,
                                          fmi3String resourcePath,
                                          fmi3Boolean visible,
                                          fmi3Boolean loggingOn,
                                          fmi3InstanceEnvironment instanceEnvironment,
                                          fmi3LogMessageCallback logMessage)
{
    UNUSED(instanceName);
    UNUSED(instantiationToken);
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(loggingOn);
    UNUSED(instanceEnvironment);
    UNUSED(logMessage);
    return NULL;
}

fmi3Instance fmi3InstantiateScheduledExecution(fmi3String instanceName,
                                               fmi3String instantiationToken,
                                               fmi3String resourcePath,
                                               fmi3Boolean visible,
                                               fmi3Boolean loggingOn,
                                               fmi3InstanceEnvironment instanceEnvironment,
                                               fmi3LogMessageCallback logMessage,
                                               fmi3ClockUpdateCallback intermediateUpdate,
                                               fmi3LockPreemptionCallback lockPreemption,
                                               fmi3UnlockPreemptionCallback unlockPreemption) {
    UNUSED(instanceName);
    UNUSED(instantiationToken);
    UNUSED(resourcePath);
    UNUSED(visible);
    UNUSED(loggingOn);
    UNUSED(instanceEnvironment);
    UNUSED(logMessage);
    UNUSED(intermediateUpdate);
    UNUSED(lockPreemption);
    UNUSED(unlockPreemption);
    return NULL;
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

    fmuContext *fmu = (fmuContext*)instance;

    fmu->steadyState = false;
    fmu->logstart = 0;
    fmu->logend = 0;
    fmu->steadyStateWindowLength = 0.1;
    fmu->steadyStateAbsolutLimit = 0.00001;
    fmu->lastNonSteadyStateTime = 0;
    fmu->steadyStateRatioLimit = 0.01;
    fmu->wdevsq = 0;
    fmu->meanFlow = 0;
    fmu->var2 = 0;

    return fmi3OK;  //Nothing to do
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance)
{
    UNUSED(instance);
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
#ifdef TLMVARIABLE
    else if(nValueReferences == 1 && valueReferences[0] == 3) {
        values[0] = fmu->hfactor;
    }
#endif
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

fmi3Status fmi3GetFloat32(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Float32 values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[],
                       size_t nValueReferences,
                       fmi3Int8 values[],
                       size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3UInt8 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Int16 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt16(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3UInt16 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt32(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Int32 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt32(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3UInt32 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetInt64(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Int64 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetUInt64(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3UInt64 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetBoolean(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Boolean values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetString(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         fmi3String values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetBinary(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         size_t valueSizes[],
                         fmi3Binary values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(valueSizes);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3GetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Clock values[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    return fmi3Warning;
}

fmi3Status fmi3SetFloat32(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Float32 values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[],
                       size_t nValueReferences,
                       const fmi3Int8 values[],
                       size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3UInt8 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Int16 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt16(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3UInt16 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt32(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Int32 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt32(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3UInt32 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetInt64(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Int64 values[],
                        size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetUInt64(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3UInt64 values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetBoolean(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Boolean values[],
                          size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetString(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const fmi3String values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetBinary(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[],
                         size_t nValueReferences,
                         const size_t valueSizes[],
                         const fmi3Binary values[],
                         size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(valueSizes);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}

fmi3Status fmi3SetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Clock values[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(values);
    return fmi3Warning;
}

fmi3Status fmi3EnterStepMode(fmi3Instance instance)
{
    UNUSED(instance);
    return fmi3OK;
}


bool steadyState(fmuContext *fmu)
{
    // tlog and wlog are circular buffers with maximum length LOG_SIZE

    fmu->tlog[fmu->logstart] = fmu->tsolver;
    fmu->wlog[fmu->logstart] = fmu->lastFlow;
    fmu->logend++;
    if(fmu->logend >= LOG_SIZE) {
        fmu->logend = 0;
    }
    if(fmu->logend == fmu->logstart) {
        fmu->logstart = fmu->logend+1;
        if(fmu->logstart > LOG_SIZE) {
            fmu->logstart = 0;
        }
    }

    while(fmu->tlog[fmu->logstart] < fmu->tsolver-fmu->steadyStateWindowLength && fmu->logstart != fmu->logend) {
        fmu->logstart++;
        if(fmu->logstart > LOG_SIZE) {
            fmu->logstart = 0;
        }
    }
    assert(fmu->logstart != fmu->logend);

    //Number of log samples stored
    size_t nlog = (fmu->logend - fmu->logstart);

    int method = 3;
    if(method == 1) {
        if(fmu->tcur > 0.10136*100 && fmu->tcur < 0.11) {
            fmu->steadyState = false;
        }
        if(true) {

            double wmax = -1e100;
            for(size_t i = fmu->logstart; i <= fmu->logend; ++i) {
                if(fabs(fmu->wlog[i] -fmu->lastFlow ) > wmax) {
                    wmax = fabs(fmu->wlog[i] - fmu->lastFlow);
                }
            }

            fmu->steadyState = (wmax < fmu->steadyStateAbsolutLimit && fmu->tsolver > fmu->steadyStateWindowLength + fmu->lastNonSteadyStateTime);
            if(!fmu->steadyState && fmu->tsolver > 2*fmu->steadyStateWindowLength+fmu->lastNonSteadyStateTime) {
                fmu->lastNonSteadyStateTime = fmu->tsolver;
            }
        }
    }
    else if(method == 2) {
        if(fmu->tcur > 10*0.1 && fmu->tcur < 0.11) {
            fmu->steadyState = false;
        }
        else {

            //Sum of values
            double sum_vec = 0;
            for(size_t i=fmu->logstart; i<fmu->logend; ++i) {
                sum_vec+=fmu->wlog[i];
            }

            //Mean of values
            double wmean = sum_vec/nlog;

            //Variance method 1
            double sum1 = 0;
            for(size_t i=fmu->logstart; i<fmu->logend; i++)
            {
                sum1 += (fmu->wlog[i]-wmean)*(fmu->wlog[i]-wmean);
            }
            double var1 = sum1/(nlog-1.0);

            //Variance method 2
            double sum2 = 0;
            for(size_t i=fmu->logstart; i<fmu->logend; i++)
            {
                sum2 += (fmu->wlog[i+1]-fmu->wlog[i])*(fmu->wlog[i+1]-fmu->wlog[i]);
            }
            double var2 = sum2/(nlog-1.0);

            double ratio = var1/(var2+DBL_MIN);

            fmu->steadyState = (ratio<fmu->steadyStateRatioLimit) || fabs(var1-var2) < fmu->steadyStateAbsolutLimit;
        }
    }
    else if(method == 3) {
            double lambda1=0.3;
            double lambda2=0.2;
            double lambda3=0.001;

            //Previous flow
            double prevFlow = fmu->lastFlow;
            if(nlog > 2) {
                prevFlow = fmu->wlog[fmu->logend-2];
            }

            //Mean square deviation (filtered)
            fmu->wdevsq = lambda2*(fmu->lastFlow-fmu->meanFlow)*(fmu->lastFlow - fmu->meanFlow) + (1-lambda2*lambda2)*fmu->wdevsq;

            //Exponentially weighted moving average
            fmu->meanFlow = lambda1*fmu->lastFlow + (1-lambda1)*fmu->meanFlow;

            //Variance (method 1)
            double var1 = (2-lambda1)*fmu->wdevsq;

            //Variance (method 2)
            fmu->var2 = lambda3*(fmu->lastFlow-prevFlow)*(fmu->lastFlow-prevFlow) + (1-lambda3)*fmu->var2;

            //Ratio
            double ratio = var1/(fmu->var2+DBL_MIN);
            fmu->steadyState = (ratio < fmu->steadyStateRatioLimit);
    }

    if(fmu->steadyState) {
        fmu->solverStep = fmin(fmu->solverStep*1.1,fmu->minStepSize*fmu->hfactor);
    }
    else {
        fmu->solverStep = fmu->minStepSize;
    }

    return fmu->steadyState;
}

void saveState(fmuContext *fmu)
{
    FMIC_N_VScale(1.0, fmu->y, fmu->y_backup);

    fmu->tsolver_backup = fmu->tsolver;

    size_t memsize = CVodeSerializationSize(fmu->mem);
    fmu->mem_backup = malloc(memsize+16);
    CVodeSerialize(fmu->mem, &fmu->mem_backup);

    fmu->wdevsq_backup = fmu->wdevsq;
    fmu->meanFlow_backup = fmu->meanFlow;
    fmu->var2_backup = fmu->var2;
}

void restoreState(fmuContext *fmu)
{
    double cvstep;
    CVodeGetCurrentStep(fmu->mem, &cvstep);
    fmu->tsolver = fmu->tsolver_backup;
    FMIC_N_VScale(1.0, fmu->y_backup,  fmu->y);
    CVodeReInit(fmu->mem, fmu->tsolver, fmu->y);
    CVodeDeserialize(fmu->mem, &fmu->mem_backup);
    CVodeSetInitStep(fmu->mem, fmin(fmu->solverStep,cvstep*0.5));

    fmu->wdevsq = fmu->wdevsq_backup;
    fmu->meanFlow = fmu->meanFlow_backup;
    fmu->var2 = fmu->var2_backup;
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

#ifdef TLMVARIABLE
    while(fmu->tsolver < currentCommunicationPoint+communicationStepSize) {

        saveState(fmu);

        double nextTime = currentCommunicationPoint+fmu->solverStep;

        CVodeSetMaxStep(fmu->mem, fmu->solverStep);
        CVode(fmu->mem, nextTime, fmu->y, &fmu->tsolver, CV_ONE_STEP);

        double cvstep;
        CVodeGetCurrentStep(fmu->mem, &cvstep);
        if(!steadyState(fmu) && cvstep >= fmu->minStepSize+1e-10) {
            restoreState(fmu);
            continue;
        }

        fmu->intermediateUpdate(fmu->instanceEnvironment, fmu->tsolver, fmi3True, fmi3True, fmi3True, fmi3False, NULL, NULL);
    }
#else
    while(fmu->tsolver < currentCommunicationPoint+communicationStepSize) {
        CVode(fmu->mem, currentCommunicationPoint+communicationStepSize, fmu->y, &fmu->tsolver, CV_ONE_STEP);
        fmu->intermediateUpdate(fmu->instanceEnvironment, fmu->tsolver, fmi3True, fmi3True, fmi3True, fmi3False, NULL, NULL);
    }
#endif
    return fmi3OK;
}

fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Instance instance,
                                               fmi3ValueReference valueReference,
                                               size_t* nDependencies) {
    UNUSED(instance);
    UNUSED(valueReference);
    UNUSED(nDependencies);
    return fmi3Warning;
}

fmi3Status fmi3GetVariableDependencies(fmi3Instance instance,
                                       fmi3ValueReference dependent,
                                       size_t elementIndicesOfDependent[],
                                       fmi3ValueReference independents[],
                                       size_t elementIndicesOfIndependents[],
                                       fmi3DependencyKind dependencyKinds[],
                                       size_t nDependencies) {
    UNUSED(instance);
    UNUSED(dependent);
    UNUSED(elementIndicesOfDependent);
    UNUSED(independents);
    UNUSED(elementIndicesOfIndependents);
    UNUSED(dependencyKinds);
    UNUSED(nDependencies);
    return fmi3Warning;
}

fmi3Status fmi3GetFMUState(fmi3Instance instance,
                           fmi3FMUState* FMUState) {
    UNUSED(instance);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3SetFMUState(fmi3Instance instance,
                           fmi3FMUState FMUState) {
    UNUSED(instance);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3FreeFMUState(fmi3Instance instance,
                            fmi3FMUState* FMUState) {
    UNUSED(instance);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3SerializedFMUStateSize(fmi3Instance instance,
                                      fmi3FMUState FMUState,
                                      size_t* size) {
    UNUSED(instance);
    UNUSED(FMUState);
    UNUSED(size);
    return fmi3Warning;
}

fmi3Status fmi3SerializeFMUState(fmi3Instance instance,
                                 fmi3FMUState FMUState,
                                 fmi3Byte serializedState[],
                                 size_t size) {
    UNUSED(instance);
    UNUSED(FMUState);
    UNUSED(serializedState);
    UNUSED(size);
    return fmi3Warning;
}

fmi3Status fmi3DeserializeFMUState(fmi3Instance instance,
                                   const fmi3Byte serializedState[],
                                   size_t size,
                                   fmi3FMUState* FMUState) {
    UNUSED(instance);
    UNUSED(serializedState);
    UNUSED(size);
    UNUSED(FMUState);
    return fmi3Warning;
}

fmi3Status fmi3GetDirectionalDerivative(fmi3Instance instance,
                                        const fmi3ValueReference unknowns[],
                                        size_t nUnknowns,
                                        const fmi3ValueReference knowns[],
                                        size_t nKnowns,
                                        const fmi3Float64 seed[],
                                        size_t nSeed,
                                        fmi3Float64 sensitivity[],
                                        size_t nSensitivity) {
    UNUSED(instance);
    UNUSED(unknowns);
    UNUSED(nUnknowns);
    UNUSED(knowns);
    UNUSED(nKnowns);
    UNUSED(seed);
    UNUSED(nSeed);
    UNUSED(sensitivity);
    UNUSED(nSensitivity);
    return fmi3Warning;
}

fmi3Status fmi3GetAdjointDerivative(fmi3Instance instance,
                                    const fmi3ValueReference unknowns[],
                                    size_t nUnknowns,
                                    const fmi3ValueReference knowns[],
                                    size_t nKnowns,
                                    const fmi3Float64 seed[],
                                    size_t nSeed,
                                    fmi3Float64 sensitivity[],
                                    size_t nSensitivity) {
    UNUSED(instance);
    UNUSED(unknowns);
    UNUSED(nUnknowns);
    UNUSED(knowns);
    UNUSED(nKnowns);
    UNUSED(seed);
    UNUSED(nSeed);
    UNUSED(sensitivity);
    UNUSED(nSensitivity);
    return fmi3Warning;
}

fmi3Status fmi3EnterConfigurationMode(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3Warning;
}

fmi3Status fmi3ExitConfigurationMode(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3Warning;
}

fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  fmi3Float64 intervals[],
                                  fmi3IntervalQualifier qualifiers[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervals);
    UNUSED(qualifiers);
    return fmi3Warning;
}

fmi3Status fmi3GetIntervalFraction(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   fmi3UInt64 intervalCounters[],
                                   fmi3UInt64 resolutions[],
                                   fmi3IntervalQualifier qualifiers[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervalCounters);
    UNUSED(resolutions);
    UNUSED(qualifiers);
    return fmi3Warning;
}

fmi3Status fmi3GetShiftDecimal(fmi3Instance instance,
                               const fmi3ValueReference valueReferences[],
                               size_t nValueReferences,
                               fmi3Float64 shifts[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(shifts);
    return fmi3Warning;
}

fmi3Status fmi3GetShiftFraction(fmi3Instance instance,
                                const fmi3ValueReference valueReferences[],
                                size_t nValueReferences,
                                fmi3UInt64 shiftCounters[],
                                fmi3UInt64 resolutions[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(shiftCounters);
    UNUSED(resolutions);
    return fmi3Warning;
}

fmi3Status fmi3SetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  const fmi3Float64 intervals[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervals);
    return fmi3Warning;
}

fmi3Status fmi3SetIntervalFraction(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   const fmi3UInt64 intervalCounters[],
                                   const fmi3UInt64 resolutions[]){
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(intervalCounters);
    UNUSED(resolutions);
    return fmi3Warning;
}

fmi3Status fmi3EvaluateDiscreteStates(fmi3Instance instance) {
    UNUSED(instance);
    return fmi3Warning;
}


fmi3Status fmi3SetShiftDecimal(fmi3Instance instance,
                               const fmi3ValueReference valueReferences[],
                               size_t nValueReferences,
                               const fmi3Float64 shifts[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(shifts);
    return fmi3Warning;
}

fmi3Status fmi3SetShiftFraction(fmi3Instance instance,
                                const fmi3ValueReference valueReferences[],
                                size_t nValueReferences,
                                const fmi3UInt64 counters[],
                                const fmi3UInt64 resolutions[]) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(counters);
    UNUSED(resolutions);
    return fmi3Warning;
}

fmi3Status fmi3GetOutputDerivatives(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Int32 orders[],
                                    fmi3Float64 values[],
                                    size_t nValues) {
    UNUSED(instance);
    UNUSED(valueReferences);
    UNUSED(nValueReferences);
    UNUSED(orders);
    UNUSED(values);
    UNUSED(nValues);
    return fmi3Warning;
}
