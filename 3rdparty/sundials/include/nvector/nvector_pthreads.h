/*
 * -----------------------------------------------------------------
 * $Revision: 4869 $
 * $Date: 2016-08-19 10:34:20 -0700 (Fri, 19 Aug 2016) $
 * ----------------------------------------------------------------- 
 * Programmer(s): David J. Gardner @ LLNL
 * -----------------------------------------------------------------
 * Acknowledgements: This NVECTOR module is based on the NVECTOR 
 *                   Serial module by Scott D. Cohen, Alan C. 
 *                   Hindmarsh, Radu Serban, and Aaron Collier 
 *                   @ LLNL
 * -----------------------------------------------------------------
 * LLNS Copyright Start
 * Copyright (c) 2014, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department 
 * of Energy by Lawrence Livermore National Laboratory in part under 
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 * -----------------------------------------------------------------
 * This is the header file for the POSIX Threads (Pthreads) 
 * implementation of the NVECTOR module using LOCAL data structs
 * to share data between threads.
 *
 * Part I contains declarations specific to the pthreads
 * implementation of the supplied NVECTOR module.
 *
 * Part II defines accessor macros that allow the user to
 * efficiently use the type FMIC_N_Vector without making explicit
 * references to the underlying data structure.
 *
 * Part III contains the prototype for the constructor FMIC_N_VNew_Pthreads
 * as well as implementation-specific prototypes for various useful
 * vector operations.
 *
 * Notes:
 *
 *   - The definition of the generic FMIC_N_Vector structure can be found
 *     in the header file sundials_nvector.h.
 *
 *   - The definition of the type 'realtype' can be found in the
 *     header file sundials_types.h, and it may be changed (at the 
 *     configuration stage) according to the user's needs. 
 *     The sundials_types.h file also contains the definition
 *     for the type 'booleantype'.
 *
 *   - FMIC_N_Vector arguments to arithmetic vector operations need not
 *     be distinct. For example, the following call:
 *
 *       FMIC_N_VLinearSum_Pthreads(a,x,b,y,y);
 *
 *     (which stores the result of the operation a*x+b*y in y)
 *     is legal.
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_PTHREADS_H
#define _NVECTOR_PTHREADS_H

#include <pthread.h>
#include <sundials/sundials_nvector.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/*
 * -----------------------------------------------------------------
 * PART I: PTHREADS implementation of FMIC_N_Vector
 * -----------------------------------------------------------------
 */

/* pthreads implementation of the FMIC_N_Vector 'content' structure
   contains the length of the vector, number of threads, a pointer 
   to an array of 'realtype' components, and a flag indicating 
   ownership of the data */

struct _FMIC_N_VectorContent_Pthreads {
  long int length;
  booleantype own_data;
  realtype *data;
  int num_threads;
};

typedef struct _FMIC_N_VectorContent_Pthreads *FMIC_N_VectorContent_Pthreads;


/* structure to allow threads to share data */
struct _Pthreads_Data{
  long int start;                /* starting index for loop  */ 
  long int end;                  /* ending index for loop    */
  realtype c1, c2;               /* scaler values            */
  realtype *v1, *v2, *v3;        /* vector data              */
  realtype *global_val;          /* shared global variable   */
  pthread_mutex_t *global_mutex; /* lock for shared variable */ 
};

typedef struct _Pthreads_Data Pthreads_Data;


/*
 * -----------------------------------------------------------------
 * PART II: macros NV_CONTENT_PT, NV_DATA_PT, NV_OWN_DATA_PT,
 *          NV_LENGTH_PT, and NV_Ith_PT
 * -----------------------------------------------------------------
 * In the descriptions below, the following user declarations
 * are assumed:
 *
 * FMIC_N_Vector v;
 * long int i;
 *
 * (1) NV_CONTENT_PT
 *
 *     This routines gives access to the contents of the pthreads
 *     vector FMIC_N_Vector.
 *
 *     The assignment v_cont = NV_CONTENT_PT(v) sets v_cont to be
 *     a pointer to the pthreads FMIC_N_Vector content structure.
 *
 * (2) NV_DATA_PT NV_OWN_DATA_PT NV_LENGTH_PT and NV_NUM_THREADS_PT
 *
 *     These routines give access to the individual parts of
 *     the content structure of a pthreads FMIC_N_Vector.
 *
 *     The assignment v_data = NV_DATA_PT(v) sets v_data to be
 *     a pointer to the first component of v. The assignment
 *     NV_DATA_PT(v) = data_V sets the component array of v to
 *     be data_v by storing the pointer data_v.
 *
 *     The assignment v_len = NV_LENGTH_PT(v) sets v_len to be
 *     the length of v. The call NV_LENGTH_PT(v) = leFMIC_N_V sets
 *     the length of v to be leFMIC_N_V.
 *
 *     The assignment v_nthreads = NV_NUM_THREADS(v) sets v_nthreads
 *     to be the number of threads that operate on v. The call 
 *     NV_NUM_THREADS(v) = nthreads_v sets the number of threads that 
 *     operate on v to be nthreads_v.
 *
 * (3) NV_Ith_PT
 *
 *     In the following description, the components of an
 *     FMIC_N_Vector are numbered 0..n-1, where n is the length of v.
 *
 *     The assignment r = NV_Ith_PT(v,i) sets r to be the value of
 *     the ith component of v. The assignment NV_Ith_PT(v,i) = r
 *     sets the value of the ith component of v to be r.
 *
 * Note: When looping over the components of an FMIC_N_Vector v, it is
 * more efficient to first obtain the component array via
 * v_data = NV_DATA_PT(v) and then access v_data[i] within the
 * loop than it is to use NV_Ith_PT(v,i) within the loop.
 * -----------------------------------------------------------------
 */

#define NV_CONTENT_PT(v)       ( (FMIC_N_VectorContent_Pthreads)(v->content) )

#define NV_LENGTH_PT(v)        ( NV_CONTENT_PT(v)->length )

#define NV_NUM_THREADS_PT(v)   ( NV_CONTENT_PT(v)->num_threads )

#define NV_OWN_DATA_PT(v)      ( NV_CONTENT_PT(v)->own_data )

#define NV_DATA_PT(v)          ( NV_CONTENT_PT(v)->data )

#define NV_Ith_PT(v,i)         ( NV_DATA_PT(v)[i] )

/*
 * -----------------------------------------------------------------
 * PART III: functions exported by nvector_Pthreads
 * 
 * CONSTRUCTORS:
 *    FMIC_N_VNew_Pthreads
 *    FMIC_N_VNewEmpty_Pthreads
 *    FMIC_N_VMake_Pthreads
 *    FMIC_N_VCloneVectorArray_Pthreads
 *    FMIC_N_VCloneVectorArrayEmpty_Pthreads
 * DESTRUCTORS:
 *    FMIC_N_VDestroy_Pthreads
 *    FMIC_N_VDestroyVectorArray_Pthreads
 * OTHER:
 *    FMIC_N_VGetLength_Pthreads
 *    FMIC_N_VPrint_Pthreads
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNew_Pthreads
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a pthreads vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNew_Pthreads(long int vec_length, int n_threads);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNewEmpty_Pthreads
 * -----------------------------------------------------------------
 * This function creates a new pthreads FMIC_N_Vector with an empty (NULL)
 * data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNewEmpty_Pthreads(long int vec_length, int n_threads);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VMake_Pthreads
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a pthreads vector
 * with a user-supplied data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VMake_Pthreads(long int vec_length, int n_threads, realtype *v_data);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArray_Pthreads
 * -----------------------------------------------------------------
 * This function creates an array of 'count' PTHREADS vectors by
 * cloning a given vector w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray_Pthreads(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArrayEmpty_Pthreads
 * -----------------------------------------------------------------
 * This function creates an array of 'count' PTHREADS vectors each
 * with an empty (NULL) data array by cloning w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArrayEmpty_Pthreads(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VDestroyVectorArray_Pthreads
 * -----------------------------------------------------------------
 * This function frees an array of PTHREADS vectors created with 
 * FMIC_N_VCloneVectorArray_Pthreads or FMIC_N_VCloneVectorArrayEmpty_Pthreads.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray_Pthreads(FMIC_N_Vector *vs, int count);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetLength_Pthreads
 * -----------------------------------------------------------------
 * This function returns number of vector elements.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT long int FMIC_N_VGetLength_Pthreads(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VPrint_Pthreads
 * -----------------------------------------------------------------
 * This function prints the content of a pthreads vector to stdout.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VPrint_Pthreads(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * pthreads implementations of various useful vector operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID_Pthreads(FMIC_N_Vector v);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty_Pthreads(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone_Pthreads(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy_Pthreads(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace_Pthreads(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer_Pthreads(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer_Pthreads(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum_Pthreads(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst_Pthreads(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd_Pthreads(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv_Pthreads(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale_Pthreads(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs_Pthreads(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv_Pthreads(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst_Pthreads(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd_Pthreads(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm_Pthreads(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm_Pthreads(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask_Pthreads(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin_Pthreads(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm_Pthreads(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm_Pthreads(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare_Pthreads(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest_Pthreads(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask_Pthreads(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient_Pthreads(FMIC_N_Vector num, FMIC_N_Vector denom);

#ifdef __cplusplus
}
#endif

#endif
