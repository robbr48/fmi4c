/*
 * -----------------------------------------------------------------
 * $Revision: 4867 $
 * $Date: 2016-08-19 10:05:14 -0700 (Fri, 19 Aug 2016) $
 * ----------------------------------------------------------------- 
 * Programmer(s): Scott D. Cohen, Alan C. Hindmarsh, Radu Serban,
 *                and Aaron Collier @ LLNL
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
 * This is the main header file for the MPI-enabled implementation
 * of the NVECTOR module.
 *
 * Part I contains declarations specific to the parallel
 * implementation of the supplied NVECTOR module.
 *
 * Part II defines accessor macros that allow the user to efficiently
 * use the type FMIC_N_Vector without making explicit references to the
 * underlying data structure.
 *
 * Part III contains the prototype for the constructor
 * FMIC_N_VNew_Parallel as well as implementation-specific prototypes
 * for various useful vector operations.
 *
 * Notes:
 *
 *   - The definition of the generic FMIC_N_Vector structure can be
 *     found in the header file sundials_nvector.h.
 *
 *   - The definition of the type realtype can be found in the
 *     header file sundials_types.h, and it may be changed (at the 
 *     configuration stage) according to the user's needs. 
 *     The sundials_types.h file also contains the definition
 *     for the type booleantype.
 *
 *   - FMIC_N_Vector arguments to arithmetic vector operations need not
 *     be distinct. For example, the following call:
 *
 *        FMIC_N_VLinearSum_Parallel(a,x,b,y,y);
 *
 *     (which stores the result of the operation a*x+b*y in y)
 *     is legal.
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_PARALLEL_H
#define _NVECTOR_PARALLEL_H

#include <mpi.h>
#include <sundials/sundials_nvector.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif


/*
 * -----------------------------------------------------------------
 * PART I: PARALLEL implementation of FMIC_N_Vector               
 * -----------------------------------------------------------------
 */

/* define MPI data types */

#if defined(SUNDIALS_SINGLE_PRECISION)

#define PVEC_REAL_MPI_TYPE MPI_FLOAT

#elif defined(SUNDIALS_DOUBLE_PRECISION)

#define PVEC_REAL_MPI_TYPE MPI_DOUBLE

#elif defined(SUNDIALS_EXTENDED_PRECISION)

#define PVEC_REAL_MPI_TYPE MPI_LONG_DOUBLE

#endif

#define PVEC_INTEGER_MPI_TYPE MPI_LONG

/* parallel implementation of the FMIC_N_Vector 'content' structure
   contains the global and local lengths of the vector, a pointer
   to an array of 'realtype components', the MPI communicator,
   and a flag indicating ownership of the data */

struct _FMIC_N_VectorContent_Parallel {
  long int local_length;   /* local vector length         */
  long int global_length;  /* global vector length        */
  booleantype own_data;    /* ownership of data           */
  realtype *data;          /* local data array            */
  MPI_Comm comm;           /* pointer to MPI communicator */
};

typedef struct _FMIC_N_VectorContent_Parallel *FMIC_N_VectorContent_Parallel;

/*
 * -----------------------------------------------------------------
 * PART II: macros NV_CONTENT_P, NV_DATA_P, NV_OWN_DATA_P,
 *          NV_LOCLENGTH_P, NV_GLOBLENGTH_P,NV_COMM_P, and NV_Ith_P
 * -----------------------------------------------------------------
 * In the descriptions below, the following user declarations
 * are assumed:
 *
 * FMIC_N_Vector v;
 * long int v_len, s_len, i;
 *
 * (1) NV_CONTENT_P
 *
 *     This routines gives access to the contents of the parallel
 *     vector FMIC_N_Vector.
 *
 *     The assignment v_cont = NV_CONTENT_P(v) sets v_cont to be
 *     a pointer to the parallel FMIC_N_Vector content structure.
 *
 * (2) NV_DATA_P, NV_OWN_DATA_P, NV_LOCLENGTH_P, NV_GLOBLENGTH_P,
 *     and NV_COMM_P
 *
 *     These routines give access to the individual parts of
 *     the content structure of a parallel FMIC_N_Vector.
 *
 *     The assignment v_data = NV_DATA_P(v) sets v_data to be
 *     a pointer to the first component of the local data for
 *     the vector v. The assignment NV_DATA_P(v) = data_v sets
 *     the component array of v to be data_V by storing the
 *     pointer data_v.
 *
 *     The assignment v_llen = NV_LOCLENGTH_P(v) sets v_llen to
 *     be the length of the local part of the vector v. The call
 *     NV_LOCLENGTH_P(v) = lleFMIC_N_V sets the local length
 *     of v to be lleFMIC_N_V.
 *
 *     The assignment v_glen = NV_GLOBLENGTH_P(v) sets v_glen to
 *     be the global length of the vector v. The call
 *     NV_GLOBLENGTH_P(v) = gleFMIC_N_V sets the global length of v to
 *     be gleFMIC_N_V.
 *
 *     The assignment v_comm = NV_COMM_P(v) sets v_comm to be the
 *     MPI communicator of the vector v. The assignment
 *     NV_COMM_C(v) = comm_v sets the MPI communicator of v to be
 *     comm_v.
 *
 * (3) NV_Ith_P
 *
 *     In the following description, the components of the
 *     local part of an FMIC_N_Vector are numbered 0..n-1, where n
 *     is the local length of (the local part of) v.
 *
 *     The assignment r = NV_Ith_P(v,i) sets r to be the value
 *     of the ith component of the local part of the vector v.
 *     The assignment NV_Ith_P(v,i) = r sets the value of the
 *     ith local component of v to be r.
 *
 * Note: When looping over the components of an FMIC_N_Vector v, it is
 * more efficient to first obtain the component array via
 * v_data = NV_DATA_P(v) and then access v_data[i] within the
 * loop than it is to use NV_Ith_P(v,i) within the loop.
 * -----------------------------------------------------------------
 */

#define NV_CONTENT_P(v)    ( (FMIC_N_VectorContent_Parallel)(v->content) )

#define NV_LOCLENGTH_P(v)  ( NV_CONTENT_P(v)->local_length )

#define NV_GLOBLENGTH_P(v) ( NV_CONTENT_P(v)->global_length )

#define NV_OWN_DATA_P(v)   ( NV_CONTENT_P(v)->own_data )

#define NV_DATA_P(v)       ( NV_CONTENT_P(v)->data )

#define NV_COMM_P(v)       ( NV_CONTENT_P(v)->comm )

#define NV_Ith_P(v,i)      ( NV_DATA_P(v)[i] )

/*
 * -----------------------------------------------------------------
 * PART III: functions exported by nvector_parallel
 * 
 * CONSTRUCTORS:
 *    FMIC_N_VNew_Parallel
 *    FMIC_N_VNewEmpty_Parallel
 *    FMIC_N_VMake_Parallel
 *    FMIC_N_VCloneVectorArray_Parallel
 *    FMIC_N_VCloneVectorArrayEmpty_Parallel
 * DESTRUCTORS:
 *    FMIC_N_VDestroy_Parallel
 *    FMIC_N_VDestroyVectorArray_Parallel
 * OTHER:
 *    FMIC_N_VGetLength_Parallel
 *    FMIC_N_VGetLocalLength_Parallel
 *    FMIC_N_VPrint_Parallel
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNew_Parallel
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a parallel vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNew_Parallel(MPI_Comm comm, 
                                         long int local_length,
                                         long int global_length);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNewEmpty_Parallel
 * -----------------------------------------------------------------
 * This function creates a new parallel FMIC_N_Vector with an empty
 * (NULL) data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNewEmpty_Parallel(MPI_Comm comm, 
                                              long int local_length,
                                              long int global_length);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VMake_Parallel
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a parallel vector
 * with a user-supplied data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VMake_Parallel(MPI_Comm comm, 
                                          long int local_length,
                                          long int global_length,
                                          realtype *v_data);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArray_Parallel
 * -----------------------------------------------------------------
 * This function creates an array of 'count' PARALLEL vectors by
 * cloning a given vector w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray_Parallel(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArrayEmpty_Parallel
 * -----------------------------------------------------------------
 * This function creates an array of 'count' PARALLEL vectors each 
 * with an empty (NULL) data array by cloning w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArrayEmpty_Parallel(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VDestroyVectorArray_Parallel
 * -----------------------------------------------------------------
 * This function frees an array of FMIC_N_Vector created with 
 * FMIC_N_VCloneVectorArray_Parallel or FMIC_N_VCloneVectorArrayEmpty_Parallel.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray_Parallel(FMIC_N_Vector *vs, int count);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetLength_Parallel
 * -----------------------------------------------------------------
 * This function returns number of vector elements (global vector 
 * length). It returns locally stored integer, and is therefore 
 * a local call.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT long int FMIC_N_VGetLength_Parallel(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetLocalLength_Parallel
 * -----------------------------------------------------------------
 * This function returns local vector length.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT long int FMIC_N_VGetLocalLength_Parallel(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VPrint_Parallel
 * -----------------------------------------------------------------
 * This function prints the content of a parallel vector to stdout.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VPrint_Parallel(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * parallel implementations of the vector operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID_Parallel(FMIC_N_Vector v);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty_Parallel(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone_Parallel(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy_Parallel(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace_Parallel(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer_Parallel(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer_Parallel(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum_Parallel(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst_Parallel(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd_Parallel(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv_Parallel(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale_Parallel(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs_Parallel(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv_Parallel(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst_Parallel(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd_Parallel(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm_Parallel(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm_Parallel(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask_Parallel(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin_Parallel(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm_Parallel(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm_Parallel(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare_Parallel(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest_Parallel(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask_Parallel(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient_Parallel(FMIC_N_Vector num, FMIC_N_Vector denom);

#ifdef __cplusplus
}
#endif

#endif
