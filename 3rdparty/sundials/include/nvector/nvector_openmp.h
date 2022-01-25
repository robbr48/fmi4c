/*
 * -----------------------------------------------------------------
 * $Revision: 4869 $
 * $Date: 2016-08-19 10:34:20 -0700 (Fri, 19 Aug 2016) $
 * ----------------------------------------------------------------- 
 * Programmer(s): David J. Gardner and Carol S. Woodward @ LLNL
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
 * This is the header file for the OpenMP implementation of the
 * NVECTOR module.
 *
 * Part I contains declarations specific to the OpenMP
 * implementation of the supplied NVECTOR module.
 *
 * Part II defines accessor macros that allow the user to
 * efficiently use the type FMIC_N_Vector without making explicit
 * references to the underlying data structure.
 *
 * Part III contains the prototype for the constructor FMIC_N_VNew_OpenMP
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
 *       FMIC_N_VLinearSum_OpenMP(a,x,b,y,y);
 *
 *     (which stores the result of the operation a*x+b*y in y)
 *     is legal.
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_OPENMP_H
#define _NVECTOR_OPENMP_H

#include <sundials/sundials_nvector.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/*
 * -----------------------------------------------------------------
 * PART I: OPENMP implementation of FMIC_N_Vector
 * -----------------------------------------------------------------
 */

/* OpenMP implementation of the FMIC_N_Vector 'content' structure
   contains the length of the vector, a pointer to an array
   of 'realtype' components, and a flag indicating ownership of
   the data */

struct _FMIC_N_VectorContent_OpenMP {
  long int length;
  booleantype own_data;
  realtype *data;
  int num_threads;
};

typedef struct _FMIC_N_VectorContent_OpenMP *FMIC_N_VectorContent_OpenMP;

/*
 * -----------------------------------------------------------------
 * PART II: macros NV_CONTENT_OMP, NV_DATA_OMP, NV_OWN_DATA_OMP,
 *          NV_LENGTH_OMP, and NV_Ith_OMP
 * -----------------------------------------------------------------
 * In the descriptions below, the following user declarations
 * are assumed:
 *
 * FMIC_N_Vector v;
 * long int i;
 *
 * (1) NV_CONTENT_OMP
 *
 *     This routines gives access to the contents of the OpenMP
 *     vector FMIC_N_Vector.
 *
 *     The assignment v_cont = NV_CONTENT_OMP(v) sets v_cont to be
 *     a pointer to the OpenMP FMIC_N_Vector content structure.
 *
 * (2) NV_DATA_OMP NV_OWN_DATA_OMP and NV_LENGTH_OMP
 *
 *     These routines give access to the individual parts of
 *     the content structure of a OpenMP FMIC_N_Vector.
 *
 *     The assignment v_data = NV_DATA_OMP(v) sets v_data to be
 *     a pointer to the first component of v. The assignment
 *     NV_DATA_OMP(v) = data_V sets the component array of v to
 *     be data_v by storing the pointer data_v.
 *
 *     The assignment v_len = NV_LENGTH_OMP(v) sets v_len to be
 *     the length of v. The call NV_LENGTH_OMP(v) = leFMIC_N_V sets
 *     the length of v to be leFMIC_N_V.
 *
 * (3) NV_Ith_OMP
 *
 *     In the following description, the components of an
 *     FMIC_N_Vector are numbered 0..n-1, where n is the length of v.
 *
 *     The assignment r = NV_Ith_OMP(v,i) sets r to be the value of
 *     the ith component of v. The assignment NV_Ith_OMP(v,i) = r
 *     sets the value of the ith component of v to be r.
 *
 * Note: When looping over the components of an FMIC_N_Vector v, it is
 * more efficient to first obtain the component array via
 * v_data = NV_DATA_OMP(v) and then access v_data[i] within the
 * loop than it is to use NV_Ith_OMP(v,i) within the loop.
 * -----------------------------------------------------------------
 */

#define NV_CONTENT_OMP(v)  ( (FMIC_N_VectorContent_OpenMP)(v->content) )

#define NV_LENGTH_OMP(v)   ( NV_CONTENT_OMP(v)->length )

#define NV_NUM_THREADS_OMP(v)   ( NV_CONTENT_OMP(v)->num_threads )

#define NV_OWN_DATA_OMP(v) ( NV_CONTENT_OMP(v)->own_data )

#define NV_DATA_OMP(v)     ( NV_CONTENT_OMP(v)->data )

#define NV_Ith_OMP(v,i)    ( NV_DATA_OMP(v)[i] )

/*
 * -----------------------------------------------------------------
 * PART III: functions exported by nvector_OpenMP
 * 
 * CONSTRUCTORS:
 *    FMIC_N_VNew_OpenMP
 *    FMIC_N_VNewEmpty_OpenMP
 *    FMIC_N_VMake_OpenMP
 *    FMIC_N_VCloneVectorArray_OpenMP
 *    FMIC_N_VCloneVectorArrayEmpty_OpenMP
 * DESTRUCTORS:
 *    FMIC_N_VDestroy_OpenMP
 *    FMIC_N_VDestroyVectorArray_OpenMP
 * OTHER:
 *    FMIC_N_VGetLength_OpenMP
 *    FMIC_N_VPrint_OpenMP
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNew_OpenMP
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a OpenMP vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNew_OpenMP(long int vec_length, int num_threads);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNewEmpty_OpenMP
 * -----------------------------------------------------------------
 * This function creates a new OpenMP FMIC_N_Vector with an empty (NULL)
 * data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNewEmpty_OpenMP(long int vec_length, int num_threads);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VMake_OpenMP
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a OpenMP vector
 * with a user-supplied data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VMake_OpenMP(long int vec_length, realtype *v_data, int num_threads);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArray_OpenMP
 * -----------------------------------------------------------------
 * This function creates an array of 'count' OPENMP vectors by
 * cloning a given vector w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray_OpenMP(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArrayEmpty_OpenMP
 * -----------------------------------------------------------------
 * This function creates an array of 'count' OPENMP vectors each
 * with an empty (NULL) data array by cloning w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArrayEmpty_OpenMP(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VDestroyVectorArray_OpenMP
 * -----------------------------------------------------------------
 * This function frees an array of OPENMP vectors created with 
 * FMIC_N_VCloneVectorArray_OpenMP or FMIC_N_VCloneVectorArrayEmpty_OpenMP.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray_OpenMP(FMIC_N_Vector *vs, int count);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetLength_OpenMP
 * -----------------------------------------------------------------
 * This function returns number of vector elements.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT long int FMIC_N_VGetLength_OpenMP(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VPrint_OpenMP
 * -----------------------------------------------------------------
 * This function prints the content of a OpenMP vector to stdout.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VPrint_OpenMP(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * OpenMP implementations of various useful vector operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID_OpenMP(FMIC_N_Vector v);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty_OpenMP(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone_OpenMP(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy_OpenMP(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace_OpenMP(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer_OpenMP(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer_OpenMP(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum_OpenMP(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst_OpenMP(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd_OpenMP(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv_OpenMP(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale_OpenMP(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs_OpenMP(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv_OpenMP(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst_OpenMP(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd_OpenMP(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm_OpenMP(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm_OpenMP(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask_OpenMP(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin_OpenMP(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm_OpenMP(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm_OpenMP(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare_OpenMP(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest_OpenMP(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask_OpenMP(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient_OpenMP(FMIC_N_Vector num, FMIC_N_Vector denom);

#ifdef __cplusplus
}
#endif

#endif
