/*
 * -----------------------------------------------------------------
 * $Revision: 4378 $
 * $Date: 2015-02-19 10:55:14 -0800 (Thu, 19 Feb 2015) $
 * -----------------------------------------------------------------
 * Programmer(s): Jean M. Sexton @ SMU
 *                Slaven Peles @ LLNL
 * ----------------------------------------------------------------- 
 * Based on work by: Scott D. Cohen, Alan C. Hindmarsh, Radu Serban,
 *                   and Aaron Collier @ LLNL
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
 * Part II contains the prototype for the constructor
 * FMIC_N_VNew_ParHyp as well as implementation-specific prototypes
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
 *        FMIC_N_VLinearSum_ParHyp(a,x,b,y,y);
 *
 *     (which stores the result of the operation a*x+b*y in y)
 *     is legal.
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_PARHYP_H
#define _NVECTOR_PARHYP_H

#include <mpi.h>
#include <sundials/sundials_nvector.h>

/* hypre header files */
#include <_hypre_parcsr_mv.h>

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

/* 
 * Parallel implementation of the FMIC_N_Vector 'content' structure
 * contains the global and local lengths of the vector, a pointer
 * to an array of 'realtype components', the MPI communicator,
 * and a flag indicating ownership of the data. 
 */
struct _FMIC_N_VectorContent_ParHyp {
  long int local_length;      /* local vector length         */
  long int global_length;     /* global vector length        */
  booleantype own_parvector;  /* ownership of HYPRE vector   */
  MPI_Comm comm;              /* pointer to MPI communicator */

  hypre_ParVector *x;   /* The actual hypre_ParVector object */
};

typedef struct _FMIC_N_VectorContent_ParHyp *FMIC_N_VectorContent_ParHyp;


/*
 * -----------------------------------------------------------------
 * PART II: functions exported by nvector_ParHyp
 * 
 * CONSTRUCTORS:
 *    FMIC_N_VNewEmpty_ParHyp
 *    FMIC_N_VMake_ParHyp
 *    FMIC_N_VCloneVectorArray_ParHyp
 *    FMIC_N_VCloneVectorArrayEmpty_ParHyp
 * DESTRUCTORS:
 *    FMIC_N_VDestroy_ParHyp
 *    FMIC_N_VDestroyVectorArray_ParHyp
 * ACCESSOR FUNCTIONS:
 *    FMIC_N_VGetVector_ParHyp
 * OTHER:
 *    FMIC_N_VPrint_ParHyp
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNewEmpty_ParHyp
 * -----------------------------------------------------------------
 * This function creates a new hypre vector wrapper without the 
 * hypre vector itself.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNewEmpty_ParHyp(MPI_Comm comm, 
                                            long int local_length,
                                            long int global_length);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VMake_ParHyp
 * -----------------------------------------------------------------
 * This function creates a hypre vector wrapper around user-supplied 
 * hypre vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VMake_ParHyp(hypre_ParVector *x);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArray_ParHyp
 * -----------------------------------------------------------------
 * This function creates an array of 'count' FMIC_N_Vectors by cloning a 
 * given vector w. Both, the wrapper and the underlying hypre vector
 * are cloned.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray_ParHyp(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArrayEmpty_ParHyp
 * -----------------------------------------------------------------
 * This function creates an array of 'count' empty hypre vector
 * wrappers by cloning w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArrayEmpty_ParHyp(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VDestroyVectorArray_ParHyp
 * -----------------------------------------------------------------
 * This function frees an array of FMIC_N_Vector created with 
 * FMIC_N_VCloneVectorArray_ParHyp or FMIC_N_VCloneVectorArrayEmpty_ParHyp.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray_ParHyp(FMIC_N_Vector *vs, int count);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetVector_ParHyp
 * -----------------------------------------------------------------
 * Extracts underlying HYPRE vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT hypre_ParVector *FMIC_N_VGetVector_ParHyp(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VPrint_ParHyp
 * -----------------------------------------------------------------
 * This function prints the content of a parallel vector to stdout.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VPrint_ParHyp(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * parallel implementations of the vector operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID_ParHyp(FMIC_N_Vector v);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty_ParHyp(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone_ParHyp(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy_ParHyp(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace_ParHyp(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer_ParHyp(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer_ParHyp(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum_ParHyp(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst_ParHyp(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd_ParHyp(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv_ParHyp(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale_ParHyp(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs_ParHyp(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv_ParHyp(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst_ParHyp(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd_ParHyp(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm_ParHyp(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm_ParHyp(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask_ParHyp(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin_ParHyp(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm_ParHyp(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm_ParHyp(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare_ParHyp(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest_ParHyp(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask_ParHyp(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient_ParHyp(FMIC_N_Vector num, FMIC_N_Vector denom);

#ifdef __cplusplus
}
#endif

#endif
