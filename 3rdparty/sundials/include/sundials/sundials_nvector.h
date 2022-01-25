/*
 * -----------------------------------------------------------------
 * $Revision: 4803 $
 * $Date: 2016-07-08 14:01:04 -0700 (Fri, 08 Jul 2016) $
 * ----------------------------------------------------------------- 
 * Programmer(s): Radu Serban and Aaron Collier @ LLNL
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
 * This is the header file for a generic NVECTOR package.
 * It defines the FMIC_N_Vector structure (_generic_FMIC_N_Vector) which
 * contains the following fields:
 *   - an implementation-dependent 'content' field which contains
 *     the description and actual data of the vector
 *   - an 'ops' filed which contains a structure listing operations
 *     acting on such vectors
 *
 * Part I of this file contains type declarations for the
 * _generic_FMIC_N_Vector and _generic_FMIC_N_Vector_Ops structures, as well
 * as references to pointers to such structures (FMIC_N_Vector).
 *
 * Part II of this file contains the prototypes for the vector
 * functions which operate on FMIC_N_Vector.
 *
 * At a minimum, a particular implementation of an NVECTOR must
 * do the following:
 *  - specify the 'content' field of FMIC_N_Vector,
 *  - implement the operations on those FMIC_N_Vectors,
 *  - provide a constructor routine for new vectors
 *
 * Additionally, an NVECTOR implementation may provide the following:
 *  - macros to access the underlying FMIC_N_Vector data
 *  - a constructor for an array of FMIC_N_Vectors
 *  - a constructor for an empty FMIC_N_Vector (i.e., a new FMIC_N_Vector with
 *    a NULL data pointer).
 *  - a routine to print the content of an FMIC_N_Vector
 * -----------------------------------------------------------------
 */

#ifndef _NVECTOR_H
#define _NVECTOR_H

#include <sundials/sundials_types.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

  
/*
 * -----------------------------------------------------------------
 * Implemented FMIC_N_Vector types
 * -----------------------------------------------------------------
 */
  
typedef enum {
  SUNDIALS_NVEC_SERIAL, 
  SUNDIALS_NVEC_PARALLEL, 
  SUNDIALS_NVEC_OPENMP, 
  SUNDIALS_NVEC_PTHREADS, 
  SUNDIALS_NVEC_PARHYP, 
  SUNDIALS_NVEC_PETSC,
  SUNDIALS_NVEC_CUSTOM
} FMIC_N_Vector_ID;
  
/*
 * -----------------------------------------------------------------
 * Generic definition of FMIC_N_Vector
 * -----------------------------------------------------------------
 */

/* Forward reference for pointer to FMIC_N_Vector_Ops object */
typedef struct _generic_FMIC_N_Vector_Ops *FMIC_N_Vector_Ops;

/* Forward reference for pointer to FMIC_N_Vector object */
typedef struct _generic_FMIC_N_Vector *FMIC_N_Vector;

/* Define array of FMIC_N_Vectors */
typedef FMIC_N_Vector *FMIC_N_Vector_S;

/* Structure containing function pointers to vector operations  */  
struct _generic_FMIC_N_Vector_Ops {
  FMIC_N_Vector_ID (*nvgetvectorid)(FMIC_N_Vector);
  FMIC_N_Vector    (*nvclone)(FMIC_N_Vector);
  FMIC_N_Vector    (*nvcloneempty)(FMIC_N_Vector);
  void        (*nvdestroy)(FMIC_N_Vector);
  void        (*nvspace)(FMIC_N_Vector, long int *, long int *);
  realtype*   (*nvgetarraypointer)(FMIC_N_Vector);
  void        (*nvsetarraypointer)(realtype *, FMIC_N_Vector);
  void        (*nvlinearsum)(realtype, FMIC_N_Vector, realtype, FMIC_N_Vector, FMIC_N_Vector);
  void        (*nvconst)(realtype, FMIC_N_Vector);
  void        (*nvprod)(FMIC_N_Vector, FMIC_N_Vector, FMIC_N_Vector);
  void        (*nvdiv)(FMIC_N_Vector, FMIC_N_Vector, FMIC_N_Vector);
  void        (*nvscale)(realtype, FMIC_N_Vector, FMIC_N_Vector);
  void        (*nvabs)(FMIC_N_Vector, FMIC_N_Vector);
  void        (*nvinv)(FMIC_N_Vector, FMIC_N_Vector);
  void        (*nvaddconst)(FMIC_N_Vector, realtype, FMIC_N_Vector);
  realtype    (*nvdotprod)(FMIC_N_Vector, FMIC_N_Vector);
  realtype    (*nvmaxnorm)(FMIC_N_Vector);
  realtype    (*nvwrmsnorm)(FMIC_N_Vector, FMIC_N_Vector);
  realtype    (*nvwrmsnormmask)(FMIC_N_Vector, FMIC_N_Vector, FMIC_N_Vector);
  realtype    (*nvmin)(FMIC_N_Vector);
  realtype    (*nvwl2norm)(FMIC_N_Vector, FMIC_N_Vector);
  realtype    (*nvl1norm)(FMIC_N_Vector);
  void        (*nvcompare)(realtype, FMIC_N_Vector, FMIC_N_Vector);
  booleantype (*nvinvtest)(FMIC_N_Vector, FMIC_N_Vector);
  booleantype (*nvconstrmask)(FMIC_N_Vector, FMIC_N_Vector, FMIC_N_Vector);
  realtype    (*nvminquotient)(FMIC_N_Vector, FMIC_N_Vector);
};

/*
 * -----------------------------------------------------------------
 * A vector is a structure with an implementation-dependent
 * 'content' field, and a pointer to a structure of vector
 * operations corresponding to that implementation.
 * -----------------------------------------------------------------
 */

struct _generic_FMIC_N_Vector {
  void *content;
  struct _generic_FMIC_N_Vector_Ops *ops;
};
  
/*
 * -----------------------------------------------------------------
 * Functions exported by NVECTOR module
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * FMIC_N_VGetVectorID
 *   Returns an identifier for the vector type from enumeration 
 *   FMIC_N_Vector_ID.
 *
 * FMIC_N_VClone
 *   Creates a new vector of the same type as an existing vector.
 *   It does not copy the vector, but rather allocates storage for
 *   the new vector.
 *
 * FMIC_N_VCloneEmpty
 *   Creates a new vector of the same type as an existing vector,
 *   but does not allocate storage.
 *
 * FMIC_N_VDestroy
 *   Destroys a vector created with FMIC_N_VClone.
 *
 * FMIC_N_VSpace
 *   Returns space requirements for one FMIC_N_Vector (type 'realtype' in
 *   lrw and type 'long int' in liw).
 *
 * FMIC_N_VGetArrayPointer
 *   Returns a pointer to the data component of the given FMIC_N_Vector.
 *   NOTE: This function assumes that the internal data is stored
 *   as a contiguous 'realtype' array. This routine is only used in
 *   the solver-specific interfaces to the dense and banded linear
 *   solvers, as well as the interfaces to  the banded preconditioners
 *   distributed with SUNDIALS.
 *   
 * FMIC_N_VSetArrayPointer
 *   Overwrites the data field in the given FMIC_N_Vector with a user-supplied
 *   array of type 'realtype'.
 *   NOTE: This function assumes that the internal data is stored
 *   as a contiguous 'realtype' array. This routine is only used in
 *   the interfaces to the dense linear solver.
 *
 * FMIC_N_VLinearSum
 *   Performs the operation z = a*x + b*y
 *
 * FMIC_N_VConst
 *   Performs the operation z[i] = c for i = 0, 1, ..., N-1
 *
 * FMIC_N_VProd
 *   Performs the operation z[i] = x[i]*y[i] for i = 0, 1, ..., N-1
 *
 * FMIC_N_VDiv
 *   Performs the operation z[i] = x[i]/y[i] for i = 0, 1, ..., N-1
 *
 * FMIC_N_VScale
 *   Performs the operation z = c*x
 *
 * FMIC_N_VAbs
 *   Performs the operation z[i] = |x[i]| for i = 0, 1, ..., N-1
 *
 * FMIC_N_VInv
 *   Performs the operation z[i] = 1/x[i] for i = 0, 1, ..., N-1
 *   This routine does not check for division by 0. It should be
 *   called only with an FMIC_N_Vector x which is guaranteed to have
 *   all non-zero components.
 *
 * FMIC_N_VAddConst
 *   Performs the operation z[i] = x[i] + b   for i = 0, 1, ..., N-1
 *
 * FMIC_N_VDotProd
 *   Returns the dot product of two vectors:
 *         sum (i = 0 to N-1) {x[i]*y[i]}
 *
 * FMIC_N_VMaxNorm
 *   Returns the maximum norm of x:
 *         max (i = 0 to N-1) ABS(x[i])
 *
 * FMIC_N_VWrmsNorm
 *   Returns the weighted root mean square norm of x with weight 
 *   vector w:
 *         sqrt [(sum (i = 0 to N-1) {(x[i]*w[i])^2})/N]
 *
 * FMIC_N_VWrmsNormMask
 *   Returns the weighted root mean square norm of x with weight
 *   vector w, masked by the elements of id:
 *         sqrt [(sum (i = 0 to N-1) {(x[i]*w[i]*msk[i])^2})/N]
 *   where msk[i] = 1.0 if id[i] > 0 and
 *         msk[i] = 0.0 if id[i] < 0
 *
 * FMIC_N_VMin
 *   Returns the smallest element of x:
 *         min (i = 0 to N-1) x[i]
 *
 * FMIC_N_VWL2Norm
 *   Returns the weighted Euclidean L2 norm of x with weight 
 *   vector w:
 *         sqrt [(sum (i = 0 to N-1) {(x[i]*w[i])^2})]
 *
 * FMIC_N_VL1Norm
 *   Returns the L1 norm of x:
 *         sum (i = 0 to N-1) {ABS(x[i])}
 *
 * FMIC_N_VCompare
 *   Performs the operation
 *          z[i] = 1.0 if ABS(x[i]) >= c   i = 0, 1, ..., N-1
 *                 0.0 otherwise
 *
 * FMIC_N_VInvTest
 *   Performs the operation z[i] = 1/x[i] with a test for 
 *   x[i] == 0.0 before inverting x[i].
 *   This routine returns TRUE if all components of x are non-zero 
 *   (successful inversion) and returns FALSE otherwise.
 *
 * FMIC_N_VConstrMask
 *   Performs the operation : 
 *       m[i] = 1.0 if constraint test fails for x[i]
 *       m[i] = 0.0 if constraint test passes for x[i]
 *   where the constraint tests are as follows:
 *      If c[i] = +2.0, then x[i] must be >  0.0.
 *      If c[i] = +1.0, then x[i] must be >= 0.0.
 *      If c[i] = -1.0, then x[i] must be <= 0.0.
 *      If c[i] = -2.0, then x[i] must be <  0.0.
 *   This routine returns a boolean FALSE if any element failed
 *   the constraint test, TRUE if all passed. It also sets a
 *   mask vector m, with elements equal to 1.0 where the
 *   corresponding constraint test failed, and equal to 0.0
 *   where the constraint test passed.
 *   This routine is specialized in that it is used only for
 *   constraint checking.
 *
 * FMIC_N_VMinQuotient
 *   Performs the operation : 
 *       minq  = min ( num[i]/denom[i]) over all i such that   
 *       denom[i] != 0.
 *   This routine returns the minimum of the quotients obtained
 *   by term-wise dividing num[i] by denom[i]. A zero element
 *   in denom will be skipped. If no such quotients are found,
 *   then the large value BIG_REAL is returned.
 *
 * -----------------------------------------------------------------
 *
 * The following table lists the vector functions used by
 * different modules in SUNDIALS. The symbols in the table
 * have the following meaning:
 * S    -  called by the solver;
 * D    -  called by the dense linear solver module
 * B    -  called by the band linear solver module
 * Di   -  called by the diagonal linear solver module
 * I    -  called by the iterative linear solver module
 * BP   -  called by the band preconditioner module
 * BBDP -  called by the band-block diagonal preconditioner module
 * F    -  called by the Fortran-to-C interface
 *
 *                  ------------------------------------------------
 *                                         MODULES                  
 * NVECTOR          ------------------------------------------------
 * FUNCTIONS          CVODE/CVODES          IDA             KINSOL    
 * -----------------------------------------------------------------
 * FMIC_N_VGetVectorID
 * -----------------------------------------------------------------
 * FMIC_N_VClone           S Di I                S I BBDP        S I BBDP
 * -----------------------------------------------------------------
 * FMIC_N_VCloneEmpty      F                     F               F
 * -----------------------------------------------------------------
 * FMIC_N_VDestroy         S Di I                S I BBDP        S I BBDP
 * -----------------------------------------------------------------
 * FMIC_N_VSpace           S                     S               S
 * -----------------------------------------------------------------
 * FMIC_N_VGetArrayPointer D B BP BBDP F         D B BBDP        BBDP F
 * -----------------------------------------------------------------
 * FMIC_N_VSetArrayPointer D F                   D               F
 * -----------------------------------------------------------------
 * FMIC_N_VLinearSum       S D Di I              S D I           S I
 * -----------------------------------------------------------------
 * FMIC_N_VConst           S I                   S I             I
 * -----------------------------------------------------------------
 * FMIC_N_VProd            S Di I                S I             S I
 * -----------------------------------------------------------------
 * FMIC_N_VDiv             S Di I                S I             S I
 * -----------------------------------------------------------------
 * FMIC_N_VScale           S D B Di I BP BBDP    S D B I BBDP    S I BBDP
 * -----------------------------------------------------------------
 * FMIC_N_VAbs             S                     S               S
 * -----------------------------------------------------------------
 * FMIC_N_VInv             S Di                  S               S
 * -----------------------------------------------------------------
 * FMIC_N_VAddConst        S Di                  S
 * -----------------------------------------------------------------
 * FMIC_N_VDotProd         I                     I               I
 * -----------------------------------------------------------------
 * FMIC_N_VMaxNorm         S                     S               S
 * -----------------------------------------------------------------
 * FMIC_N_VWrmsNorm        S D B I BP BBDP       S
 * -----------------------------------------------------------------
 * FMIC_N_VWrmsNormMask                          S
 * -----------------------------------------------------------------
 * FMIC_N_VMin             S                     S               S
 * -----------------------------------------------------------------
 * FMIC_N_VWL2Norm                                               S I
 * -----------------------------------------------------------------
 * FMIC_N_VL1Norm                                                I
 * -----------------------------------------------------------------
 * FMIC_N_VCompare         Di                    S
 * -----------------------------------------------------------------
 * FMIC_N_VInvTest         Di
 * -----------------------------------------------------------------
 * FMIC_N_VConstrMask                            S               S
 * -----------------------------------------------------------------
 * FMIC_N_VMinQuotient                           S               S
 * -----------------------------------------------------------------
 */
  
SUNDIALS_EXPORT FMIC_N_Vector_ID FMIC_N_VGetVectorID(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VClone(FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector FMIC_N_VCloneEmpty(FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroy(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSpace(FMIC_N_Vector v, long int *lrw, long int *liw);
SUNDIALS_EXPORT realtype *FMIC_N_VGetArrayPointer(FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VSetArrayPointer(realtype *v_data, FMIC_N_Vector v);
SUNDIALS_EXPORT void FMIC_N_VLinearSum(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VConst(realtype c, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VProd(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VDiv(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VScale(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAbs(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VInv(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT void FMIC_N_VAddConst(FMIC_N_Vector x, realtype b, FMIC_N_Vector z);
SUNDIALS_EXPORT realtype FMIC_N_VDotProd(FMIC_N_Vector x, FMIC_N_Vector y);
SUNDIALS_EXPORT realtype FMIC_N_VMaxNorm(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNorm(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VWrmsNormMask(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id);
SUNDIALS_EXPORT realtype FMIC_N_VMin(FMIC_N_Vector x);
SUNDIALS_EXPORT realtype FMIC_N_VWL2Norm(FMIC_N_Vector x, FMIC_N_Vector w);
SUNDIALS_EXPORT realtype FMIC_N_VL1Norm(FMIC_N_Vector x);
SUNDIALS_EXPORT void FMIC_N_VCompare(realtype c, FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VInvTest(FMIC_N_Vector x, FMIC_N_Vector z);
SUNDIALS_EXPORT booleantype FMIC_N_VConstrMask(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m);
SUNDIALS_EXPORT realtype FMIC_N_VMinQuotient(FMIC_N_Vector num, FMIC_N_Vector denom);

/*
 * -----------------------------------------------------------------
 * Additional functions exported by NVECTOR module
 * -----------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------
 * FMIC_N_VCloneEmptyVectorArray
 *   Creates (by cloning 'w') an array of 'count' empty FMIC_N_Vectors
 *
 * FMIC_N_VCloneVectorArray
 *   Creates (by cloning 'w') an array of 'count' FMIC_N_Vectors
 *
 * FMIC_N_VDestroyVectorArray
 *   Frees memory for an array of 'count' FMIC_N_Vectors that was
 *   created by a call to FMIC_N_VCloneVectorArray
 *
 * These functions are used by the SPGMR iterative linear solver 
 * module and by the CVODES and IDAS solvers.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneEmptyVectorArray(int count, FMIC_N_Vector w);
SUNDIALS_EXPORT FMIC_N_Vector *FMIC_N_VCloneVectorArray(int count, FMIC_N_Vector w);
SUNDIALS_EXPORT void FMIC_N_VDestroyVectorArray(FMIC_N_Vector *vs, int count);

#ifdef __cplusplus
}
#endif

#endif
