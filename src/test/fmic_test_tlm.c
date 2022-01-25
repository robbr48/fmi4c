#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "fmic.h"
#include "fmic_test.h"
#include "fmic_test_fmi1.h"
#include "fmic_test_fmi2.h"
#include "fmic_test_fmi3.h"
#include "fmic_test_tlm.h"

#define TABLE_SIZE 1500

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    fmi3Handle *fmu1;
    double table1[2][TABLE_SIZE];
    size_t table1start;
    size_t table1end;

    fmi3Handle *fmu2;
    double table2[2][TABLE_SIZE];
    size_t table2start;
    size_t table2end;

    double Zc;
    double dt;
} tlmContext;

static tlmContext tlm;

double getInterpolatedForce1(double time)
{
    while(tlm.table1[0][tlm.table1end] < time) {
#ifdef _WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
    }

    pthread_mutex_lock(&mutex1);

    int i = tlm.table1end;

    //find first element where time is not too small
    while(tlm.table1[0][i] > time) {
        --i;
        if(i < 0) {
            i = TABLE_SIZE-1;
        }
    }
    int j = i+1;

    double t1 = tlm.table1[0][j];
    double t2 = tlm.table1[0][i];
    double x1 = tlm.table1[1][j];
    double x2 = tlm.table1[1][i];

    double force = x1 + ((x2-x1)/(t2-t1)) * (time - t1);
    pthread_mutex_unlock(&mutex1);
    return force;
}


double getInterpolatedForce2(double time)
{
    while(tlm.table2[0][tlm.table2end] < time) {
#ifdef _WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
    }

    pthread_mutex_lock(&mutex2);

    int i = tlm.table2end;

    //find first element where time is not too small
    while(tlm.table2[0][i] > time) {
        --i;
        if(i < 0) {
            i = TABLE_SIZE-1;
        }
    }
    int j = i+1;

    double t1 = tlm.table2[0][j];
    double t2 = tlm.table2[0][i];
    double x1 = tlm.table2[1][j];
    double x2 = tlm.table2[1][i];

    double force = x1 + ((x2-x1)/(t2-t1)) * (time - t1);
    pthread_mutex_unlock(&mutex2);
    return force;
}

void intermediateUpdateTLM(
        fmi3InstanceEnvironment instanceEnvironment,
        fmi3Float64  intermediateUpdateTime,
        fmi3Boolean  clocksTicked,
        fmi3Boolean  intermediateVariableSetRequested,
        fmi3Boolean  intermediateVariableGetAllowed,
        fmi3Boolean  intermediateStepFinished,
        fmi3Boolean  canReturnEarly,
        fmi3Boolean* earlyReturnRequested,
        fmi3Float64* earlyReturnTime)
{
    UNUSED(clocksTicked);
    UNUSED(canReturnEarly);
    UNUSED(earlyReturnRequested);
    UNUSED(earlyReturnTime);

    fmi3ValueReference vr_v = 0;    //Connection velocity (output)
    fmi3ValueReference vr_f = 1;    //Connection force (input)

    fmi3Handle *fmu = (fmi3Handle*)instanceEnvironment;
    if(intermediateVariableGetAllowed) {
        double v;
        fmi3GetFloat64(fmu, &vr_v, 1, &v, 1);
        if(intermediateStepFinished) {
            if(fmu == tlm.fmu1) {
                ++tlm.table2end;
                while(tlm.table2[0][tlm.table2start] < intermediateUpdateTime - 2*tlm.dt) {
                    ++tlm.table2start;
                }
                if(tlm.table2end > TABLE_SIZE-1) {    //Prevent overflow (circular buffer)
                    tlm.table2end = 0;
                }
                if(tlm.table2start > TABLE_SIZE-1) {
                    tlm.table2start = 0;
                }
                tlm.table2[0][tlm.table2end] = intermediateUpdateTime;
                double c = getInterpolatedForce1(intermediateUpdateTime - tlm.dt);
                tlm.table2[1][tlm.table2end] = 2*tlm.Zc*v + c;
            }
            if(fmu == tlm.fmu2) { //!@todo Costly to compare strings
                ++tlm.table1end;
                while(tlm.table1[0][tlm.table1start] < intermediateUpdateTime - 2*tlm.dt) {
                    ++tlm.table1start;
                }
                if(tlm.table1end > TABLE_SIZE-1) {    //Prevent overflow (circular buffer)
                    tlm.table1end = 0;
                }
                if(tlm.table1start > TABLE_SIZE-1) {
                    tlm.table1start = 0;
                }
                tlm.table1[0][tlm.table1end] = intermediateUpdateTime;
                double c = getInterpolatedForce2(intermediateUpdateTime - tlm.dt);
                tlm.table1[1][tlm.table1end] = 2*tlm.Zc*v + c;
            }
        }
    }
    if(intermediateVariableGetAllowed && intermediateVariableSetRequested) {
        double c;
        if(fmu == tlm.fmu1) {
            c = getInterpolatedForce1(intermediateUpdateTime-tlm.dt);
        }
        else {
            c = getInterpolatedForce2(intermediateUpdateTime-tlm.dt);
        }
        double v;
        fmi3GetFloat64(fmu, &vr_v, 1, &v, 1);

        double F = v*tlm.Zc + c;
        fmi3SetFloat64(fmu, &vr_f, 1, &F, 1);
    }
}

//Argument struct for doStep thread
struct doStepArgs {
    fmi3Handle *fmu;
    fmi3Float64 tcur;
    fmi3Float64 tstep;
};


//doStep thread
void* doStepInThread(void* argsptr)
{
    struct doStepArgs *args = (struct doStepArgs*)argsptr;
    bool eventEncountered, terminateSimulation, earlyReturn;
    double lastT;
    fmi3DoStep(args->fmu, args->tcur, args->tstep, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastT);
    return NULL;
}


int testFMI3TLM(fmiHandle *fmua, fmiHandle *fmub)
{
    fmi3Handle *fmu3a = loadFmu3(fmua);
    if(fmu3a == NULL) {
        printf("Failed to load first FMU file\n");
        exit(1);
    }
    fmi3Handle *fmu3b = loadFmu3(fmub);
    if(fmu3b == NULL) {
        printf("Failed to load second FMU file\n");
        exit(1);
    }

    //TLM coupling parameters
    fmi3Float64 mtlm = 1;       //Distributed inertia [kg]
    fmi3Float64 ktlm = 2e5;     //Distributed stiffness [N/m]

    //Simulation parameters
    fmi3Float64 tcur = 0;       //Current simulation time
    fmi3Float64 tstop = 2;      //Simulation stop time
    fmi3Float64 tstep = 0.001;  //Simulation time step

    //Model parameters
    fmi3Float64 fd = 100;       //Disturbance force

    //Compute TLM parameters
    tlm.Zc = sqrt(mtlm*ktlm);       //Resulting characteristic impedance (global variable)
    tlm.dt = sqrt(mtlm/ktlm);       //Resulting time delay (global variable)
    printf("dt = %f\n",tlm.dt);

    //Value references
    fmi3ValueReference vr_v = 0;    //Connection velocity (output)
    fmi3ValueReference vr_f = 1;    //Connection force (input)
    fmi3ValueReference vr_fd = 5;   //Disturbance force (input to one of the FMUs)

    //Initialize interpolation tables (zero force)
    tlm.fmu1 = fmu3a;
    tlm.table1start = 0;
    tlm.table1end = 1;
    tlm.table1[0][tlm.table1start] = -10000;
    tlm.table1[1][tlm.table1start] = 0;
    tlm.table1[0][tlm.table1end] = 0;
    tlm.table1[1][tlm.table1end] = 0;

    tlm.fmu2 = fmu3b;
    tlm.table2start = 0;
    tlm.table2end = 1;
    tlm.table2[0][tlm.table2start] = -10000;
    tlm.table2[1][tlm.table2start] = 0;
    tlm.table2[0][tlm.table2end] = 0;
    tlm.table2[1][tlm.table2end] = 0;

    //Instantiate
    fmi3ValueReference requiredIntermediateVariables[2] = {0, 1};
    fmi3ValueReference nRequiredIntermediateVaraibles = 2;
    fmi3InstantiateCoSimulation(fmu3a, fmi3False, fmi3True, fmi3False, fmi3False, requiredIntermediateVariables, nRequiredIntermediateVaraibles, fmu3a, loggerFmi3, intermediateUpdateTLM);
    fmi3InstantiateCoSimulation(fmu3b, fmi3False, fmi3True, fmi3False, fmi3False, requiredIntermediateVariables, nRequiredIntermediateVaraibles, fmu3b, loggerFmi3, intermediateUpdateTLM);

    //Enter initialization
    fmi3EnterInitializationMode(fmu3a, fmi3False, 0, tcur, fmi3True, tstop);
    fmi3EnterInitializationMode(fmu3b, fmi3False, 0, tcur, fmi3True, tstop);

    //Exit initialization
    fmi3ExitInitializationMode(fmu3a);
    fmi3ExitInitializationMode(fmu3b);

    //Simlation loop
    double f = 0;
    fmi3SetFloat64(fmu3a, &vr_f, 1, &f, 1);    //Initialize TLM force to zero
    fmi3SetFloat64(fmu3b, &vr_f, 1, &f, 1);

    fmi3Float64 m1 = 5;         //Mass 1
    fmi3Float64 c1 = 50;        //Damping 1
    fmi3Float64 k1 = 1000;      //Spring stiffness 1
    fmi3Float64 m2 = 7;         //Mass 2
    fmi3Float64 c2 = 60;        //Damping 2
    fmi3Float64 k2 = 600;       //Spring stiffness 2

    fmi3ValueReference vr_m = 2;
    fmi3ValueReference vr_c = 3;
    fmi3ValueReference vr_k = 4;

    fmi3SetFloat64(fmu3a, &vr_m, 1, &m1, 1);
    fmi3SetFloat64(fmu3a, &vr_c, 1, &c1, 1);
    fmi3SetFloat64(fmu3a, &vr_k, 1, &k1, 1);
    fmi3SetFloat64(fmu3b, &vr_m, 1, &m2, 1);
    fmi3SetFloat64(fmu3b, &vr_c, 1, &c2, 1);
    fmi3SetFloat64(fmu3b, &vr_k, 1, &k2, 1);

    FILE *pResultFile;
    fclose(fopen(outputCsvPath, "w"));          //Clear old results in file
    pResultFile = fopen(outputCsvPath, "a");    //Open result file for appending
    fprintf(pResultFile, "t,v1,v2,f1,f2\n");          //Print header row

    struct doStepArgs stepArgs1;
    stepArgs1.tstep = tstep;
    stepArgs1.fmu = fmu3a;

    struct doStepArgs stepArgs2;
    stepArgs2.tstep = tstep;
    stepArgs2.fmu = fmu3b;

    pthread_t thread1, thread2;

    printf("Starting simulation...\n");

    while(tcur < tstop) {
        if(tcur > 0.1/* && tcur < 0.1+tstep*/) {
            fmi3SetFloat64(fmu3b, &vr_fd, 1, &fd, 1); //Apply disturbance force
        }

        //Create threads
        stepArgs1.tcur = tcur;
        pthread_create(&thread1, NULL, doStepInThread, &stepArgs1);

        stepArgs2.tcur = tcur;
        pthread_create(&thread2, NULL, doStepInThread, &stepArgs2);

        // wait for them to finish
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

        double v1, v2, f1, f2;
        fmi3GetFloat64(fmu3a, &vr_v, 1, &v1, 1);
        fmi3GetFloat64(fmu3b, &vr_v, 1, &v2, 1);
        fmi3GetFloat64(fmu3a, &vr_f, 1, &f1, 1);
        fmi3GetFloat64(fmu3b, &vr_f, 1, &f2, 1);
        fprintf(pResultFile, "%f,%f,%f,%f,%f\n",tcur,v1,v2,f1,f2);
        tcur += tstep;

        printf("\rSimulating, %.0f%% done...",tcur/tstop*100);
        fflush(stdout);
    }

    fclose(pResultFile);

    fmi3Terminate(fmu3a);
    fmi3Terminate(fmu3b);
    fmi3FreeInstance(fmu3a);
    fmi3FreeInstance(fmu3b);

    printf("TLM test finished!\n");

    return 0;
}
