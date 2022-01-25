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
 * This is the header file for the serial implementation of the
 * NVECTOR module.
 *
 * Part I contains declarations specific to the serial
 * implementation of the supplied NVECTOR module.
 *
 * Part II defines accessor macros that allow the user to
 * efficiently use the type FMIC_N_Vector without making explicit
 * references to the underlying data structure.
 *
 * Part III contains the prototype for the constructor FMIC_N_VNew_Serial
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
 *       FMIC_N_VLinearSum_Serial(a,x,b,y,y);
 *
 *     (which stores the result of the operation a*x+b*y in y)
 *     is legal.
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_SERIAL_H
#define _NVECTOR_SERIAL_H

#include <sundials/sundials_nvector.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/*
 * -----------------------------------------------------------------
 * PART I: SERIAL implementation of FMIC_N_Vector
 * -----------------------------------------------------------------
 */

/* serial implementation of the FMIC_N_Vector 'content' structure
   contains the length of the vector, a pointer to an array
   of 'realtype' components, and a flag indicating ownership of
   the data */

struct _FMIC_N_VectorContent_Serial {
  long int length;
  booleantype own_data;
  realtype *data;
};

typedef struct _FMIC_N_VectorContent_Serial *FMIC_N_VectorContent_Serial;

/*
 * -----------------------------------------------------------------
 * PART II: macros NV_CONTENT_S, NV_DATA_S, NV_OWN_DATA_S,
 *          NV_LENGTH_S, and NV_Ith_S
 * -----------------------------------------------------------------
 * In the descriptions below, the following user declarations
 * are assumed:
 *
 * FMIC_N_Vector v;
 * long int i;
 *
 * (1) NV_CONTENT_S
 *
 *     This routines gives access to the contents of the serial
 *     vector FMIC_N_Vector.
 *
 *     The assignment v_cont = NV_CONTENT_S(v) sets v_cont to be
 *     a pointer to the serial FMIC_N_Vector content structure.
 *
 * (2) NV_DATA_S NV_OWN_DATA_S and NV_LENGTH_S
 *
 *     These routines give access to the individual parts of
 *     the content structure of a serial FMIC_N_Vector.
 *
 *     The assignment v_data = NV_DATA_S(v) sets v_data to be
 *     a pointer to the first component of v. The assignment
 *     NV_DATA_S(v) = data_V sets the component array of v to
 *     be data_v by storing the pointer data_v.
 *
 *     The assignment v_len = NV_LENGTH_S(v) sets v_len to be
 *     the length of v. The call NV_LENGTH_S(v) = leFMIC_N_V sets
 *     the length of v to be leFMIC_N_V.
 *
 * (3) NV_Ith_S
 *
 *     In the following description, the components of an
 *     FMIC_N_Vector are numbered 0..n-1, where n is the length of v.
 *
 *     The assignment r = NV_Ith_S(v,i) sets r to be the value of
 *     the ith component of v. The assignment NV_Ith_S(v,i) = r
 *     sets the value of the ith component of v to be r.
 *
 * Note: When looping over the components of an FMIC_N_Vector v, it is
 * more efficient to first obtain the component array via
 * v_data = NV_DATA_S(v) and then access v_data[i] within the
 * loop than it is to use NV_Ith_S(v,i) within the loop.
 * -----------------------------------------------------------------
 */

#define NV_CONTENT_S(v)  ( (FMIC_N_VectorContent_Serial)(v->content) )

#define NV_LENGTH_S(v)   ( NV_CONTENT_S(v)->length )

#define NV_OWN_DATA_S(v) ( NV_CONTENT_S(v)->own_data )

#define NV_DATA_S(v)     ( NV_CONTENT_S(v)->data )

#define NV_Ith_S(v,i)    ( NV_DATA_S(v)[i] )

/*
 * -----------------------------------------------------------------
 * PART III: functions exported by nvector_serial
 * 
 * CONSTRUCTORS:
 *    FMIC_N_VNew_Serial
 *    FMIC_N_VNewEmpty_Serial
 *    FMIC_N_VMake_Serial
 *    FMIC_N_VCloneVectorArray_Serial
 *    FMIC_N_VCloneVectorArrayEmpty_Serial
 * DESTRUCTORS:
 *    FMIC_N_VDestroy_Serial
 *    FMIC_N_VDestroyVectorArray_Serial
 * OTHER:
 *    FMIC_N_VGetLength_Serial
 *    FMIC_N_VPrint_Serial
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNew_Serial
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a serial vector.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNew_Serial(long int vec_length);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VNewEmpty_Serial
 * -----------------------------------------------------------------
 * This function creates a new serial FMIC_N_Vector with an empty (NULL)
 * data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VNewEmpty_Serial(long int vec_length);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VMake_Serial
 * -----------------------------------------------------------------
 * This function creates and allocates memory for a serial vector
 * with a user-supplied data array.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VMake_Serial(long int vec_length, realtype *v_data);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArray_Serial
 * -----------------------------------------------------------------
 * This function creates an array of 'count' SERIAL vectors by
 * cloning a given vector w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray_Serial(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VCloneVectorArrayEmpty_Serial
 * -----------------------------------------------------------------
 * This function creates an array of 'count' SERIAL vectors each
 * with an empty (NULL) data array by cloning w.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArrayEmpty_Serial(int count, FMIC_N_Vector w);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VDestroyVectorArray_Serial
 * -----------------------------------------------------------------
 * This function frees an array of SERIAL vectors created with 
 * FMIC_N_VCloneVectorArray_Serial or FMIC_N_VCloneVectorArrayEmpty_Serial.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray_Serial(FMIC_N_Vector *vs, int count);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VGetLength_Serial
 * -----------------------------------------------------------------
 * This function returns number of vector elements.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT long int FMIC_N_VGetLength_Serial(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * Function : FMIC_N_VPrint_Serial
 * -----------------------------------------------------------------
 * This function prints the content of a serial vector to stdout.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void FMIC_N_VPrint_Serial(FMIC_N_Vector v);

/*
 * -----------------------------------------------------------------
 * serial implementations of various useful vector operations
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID_Serial(FMIC_N_Vector v);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty_Serial(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone_Serial(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy_Serial(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace_Serial(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer_Serial(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer_Serial(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum_Serial(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst_Serial(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd_Serial(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv_Serial(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale_Serial(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs_Serial(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv_Serial(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst_Serial(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd_Serial(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm_Serial(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm_Serial(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask_Serial(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin_Serial(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm_Serial(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm_Serial(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare_Serial(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest_Serial(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask_Serial(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient_Serial(FMIC_N_Vector num, FMIC_N_Vector denom);

#ifdef __cplusplus
}
#endif

#endif
