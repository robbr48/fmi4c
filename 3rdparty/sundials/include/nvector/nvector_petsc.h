/*
 * -----------------------------------------------------------------
 * $Revision:$
 * $Date:$
 * ----------------------------------------------------------------- 
 * Programmer(s): Slaven Peles @ LLNL
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
 * This is the main header file for the PETSc vector wrapper 
 * for NVECTOR module.
 *
 * Part I contains declarations specific to the PETSc vector wrapper
 * implementation.
 *
 * Part II contains the prototype for the constructor
 * FMIC_N_VMake_Petsc as well as PETSc-specific prototypes
 * for various useful vector operations.
 *
 * Notes:
 *
 *   - The definition of the generic FMIC_N_Vector structure can be
 *     found in the header file sundials_nvector.h.
 *
 *   - The definition of the type realtype can be found in the
 *     header file sundials_types.h, and it may be changed (at the 
 *     build configuration stage) according to the user's needs. 
 *     The sundials_types.h file also contains the definition
 *     for the type booleantype.
 *
 *   - FMIC_N_Vector arguments to arithmetic vector operations need not
 *     be distinct. For example, the following call:
 *
 *        FMIC_N_VLinearSum_Petsc(a,x,b,y,y);
 *
 *     (which stores the result of the operation a*x+b*y in y)
 *     is legal.
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_PETSC_H
#define _NVECTOR_PETSC_H

#include <mpi.h>
#include <petscvec.h>
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
   to the underlying PETSc vector, the MPI communicator,
   and a flag indicating ownership of the PETSc vector */

struct _FMIC_N_VectorContent_Petsc {
  long int local_length;   /* copy of local vector length  */
  long int global_length;  /* copy of global vector length */
  booleantype own_data;    /* ownership of data            */
  Vec *pvec;               /* pointer to PETSc vector      */
  MPI_Comm comm;           /* copy of MPI communicator     */
};

typedef struct _FMIC_N_VectorContent_Petsc *FMIC_N_VectorContent_Petsc;

/*
 * -----------------------------------------------------------------
 * PART II: functions exported by nvector_Petsc
 * 
 * CONSTRUCTORS:
 *    FMIC_N_VNewEmpty_Petsc
 *    FMIC_N_VMake_Petsc
 *    FMIC_N_VCloneVectorArray_Petsc
 *    FMIC_N_VCloneVectorArrayEmpty_Petsc
 * DESTRUCTORS:
 *    FMIC_N_VDestroyVectorArray_Petsc
 * OTHER:
 *    FMIC_N_VGetVector_Petsc
 *    FMIC_N_VPrint_Petsc
 * -----------------------------------------------------------------
 */


/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNewEmpty_Petsc
 * -----------------------------------------------------------------
 * This function creates a new FMIC_N_Vector wrapper around an empty
 * (NULL) PETSc vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNewEmpty_Petsc(MPI_Comm comm, 
                                           long int local_length,
                                           long int global_length);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VMake_Petsc
 * -----------------------------------------------------------------
 * This function is not supported for PETSc vector wrapper.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VMake_Petsc(Vec *v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetArrayPointer_Petsc
 * -----------------------------------------------------------------
 * This function is not supported for PETSc vector wrapper.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer_Petsc(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArray_Petsc
 * -----------------------------------------------------------------
 * This function creates an array of 'count' PARALLEL vectors by
 * cloning a given vector w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray_Petsc(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArrayEmpty_Petsc
 * -----------------------------------------------------------------
 * This function creates an array of 'count' PARALLEL vectors each 
 * with an empty (NULL) data array by cloning w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArrayEmpty_Petsc(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VDestroyVectorArray_Petsc
 * -----------------------------------------------------------------
 * This function frees an array of FMIC_N_Vector created with 
 * FMIC_N_VCloneVectorArray_Petsc or FMIC_N_VCloneVectorArrayEmpty_Petsc.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray_Petsc(FMIC_N_Vector *vs, int count);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetVector_Petsc
 * -----------------------------------------------------------------
 * Extracts PETSc vector from FMIC_N_Vector wrapper.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT Vec *FMIC_N_VGetVector_Petsc(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VPrint_Petsc
 * -----------------------------------------------------------------
 * This function prints the content of a parallel vector to stdout.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VPrint_Petsc(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * PETSc implementations of the vector operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID_Petsc(FMIC_N_Vector v);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty_Petsc(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone_Petsc(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy_Petsc(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace_Petsc(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer_Petsc(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum_Petsc(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst_Petsc(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd_Petsc(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv_Petsc(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale_Petsc(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs_Petsc(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv_Petsc(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst_Petsc(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd_Petsc(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm_Petsc(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm_Petsc(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask_Petsc(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin_Petsc(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm_Petsc(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm_Petsc(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare_Petsc(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest_Petsc(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask_Petsc(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient_Petsc(FMIC_N_Vector num, FMIC_N_Vector denom);

#ifdef __cplusplus
}
#endif

#endif /* _NVECTOR_PETSC_H */
