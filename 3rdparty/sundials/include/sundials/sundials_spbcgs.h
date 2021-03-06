/*
 * -----------------------------------------------------------------
 * $Revision: 4378 $
 * $Date: 2015-02-19 10:55:14 -0800 (Thu, 19 Feb 2015) $
 * -----------------------------------------------------------------
 * Programmer(s): Peter Brown and Aaron Collier @ LLNL
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
 * This is the header file for the implementation of the scaled,
 * preconditioned Bi-CGSTAB (SPBCG) iterative linear solver.
 * -----------------------------------------------------------------
 */

#ifndef _SPBCG_H
#define _SPBCG_H

#include <sundials/sundials_iterative.h>

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/*
 * -----------------------------------------------------------------
 * Types: struct SpbcgMemRec and struct *SpbcgMem
 * -----------------------------------------------------------------
 * A variable declaration of type struct *SpbcgMem denotes a pointer
 * to a data structure of type struct SpbcgMemRec. The SpbcgMemRec
 * structure contains numerous fields that must be accessed by the
 * SPBCG linear solver module.
 *
 *  l_max  maximum Krylov subspace dimension that SpbcgSolve will
 *         be permitted to use
 *
 *  r  vector (type FMIC_N_Vector) which holds the scaled, preconditioned
 *     linear system residual
 *
 *  r_star  vector (type FMIC_N_Vector) which holds the initial scaled,
 *          preconditioned linear system residual
 *
 *  p, q, u and Ap  vectors (type FMIC_N_Vector) used for workspace by
 *                  the SPBCG algorithm
 *
 *  vtemp  scratch vector (type FMIC_N_Vector) used as temporary vector
 *         storage
 * -----------------------------------------------------------------
 */

typedef struct {

  int l_max;

  FMIC_N_Vector r_star;
  FMIC_N_Vector r;
  FMIC_N_Vector p;
  FMIC_N_Vector q;
  FMIC_N_Vector u;
  FMIC_N_Vector Ap;
  FMIC_N_Vector vtemp;

} SpbcgMemRec, *SpbcgMem;

/*
 * -----------------------------------------------------------------
 * Function : SpbcgMalloc
 * -----------------------------------------------------------------
 * SpbcgMalloc allocates additional memory needed by the SPBCG
 * linear solver module.
 *
 *  l_max  maximum Krylov subspace dimension that SpbcgSolve will
 *         be permitted to use
 *
 *  vec_tmpl  implementation-specific template vector (type FMIC_N_Vector)
 *            (created using either FMIC_N_VNew_Serial or FMIC_N_VNew_Parallel)
 *
 * If successful, SpbcgMalloc returns a non-NULL memory pointer. If
 * an error occurs, then a NULL pointer is returned.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT SpbcgMem SpbcgMalloc(int l_max, FMIC_N_Vector vec_tmpl);

/*
 * -----------------------------------------------------------------
 * Function : SpbcgSolve
 * -----------------------------------------------------------------
 * SpbcgSolve solves the linear system Ax = b by means of a scaled
 * preconditioned Bi-CGSTAB (SPBCG) iterative method.
 *
 *  mem  pointer to an internal memory block allocated during a
 *       prior call to SpbcgMalloc
 *
 *  A_data  pointer to a data structure containing information
 *          about the coefficient matrix A (passed to user-supplied
 *          function referenced by atimes (function pointer))
 *
 *  x  vector (type FMIC_N_Vector) containing initial guess x_0 upon
 *     entry, but which upon return contains an approximate solution
 *     of the linear system Ax = b (solution only valid if return
 *     value is either SPBCG_SUCCESS or SPBCG_RES_REDUCED)
 *
 *  b  vector (type FMIC_N_Vector) set to the right-hand side vector b
 *     of the linear system (undisturbed by function)
 *
 *  pretype  variable (type int) indicating the type of
 *           preconditioning to be used (see sundials_iterative.h)
 *
 *  delta  tolerance on the L2 norm of the scaled, preconditioned
 *         residual (if return value == SPBCG_SUCCESS, then
 *         ||sb*P1_inv*(b-Ax)||_L2 <= delta)
 *
 *  P_data  pointer to a data structure containing preconditioner
 *          information (passed to user-supplied function referenced
 *          by psolve (function pointer))
 *
 *  sx  vector (type FMIC_N_Vector) containing positive scaling factors
 *      for x (pass sx == NULL if scaling NOT required)
 *
 *  sb  vector (type FMIC_N_Vector) containing positive scaling factors
 *      for b (pass sb == NULL if scaling NOT required)
 *
 *  atimes  user-supplied routine responsible for computing the
 *          matrix-vector product Ax (see sundials_iterative.h)
 *
 *  psolve  user-supplied routine responsible for solving the
 *          preconditioned linear system Pz = r (ignored if
 *          pretype == PREC_NONE) (see sundials_iterative.h)
 *
 *  res_norm  pointer (type realtype*) to the L2 norm of the
 *            scaled, preconditioned residual (if return value
 *            is either SPBCG_SUCCESS or SPBCG_RES_REDUCED, then
 *            *res_norm = ||sb*P1_inv*(b-Ax)||_L2, where x is
 *            the computed approximate solution, sb is the diagonal
 *            scaling matrix for the right-hand side b, and P1_inv
 *            is the inverse of the left-preconditioner matrix)
 *
 *  nli  pointer (type int*) to the total number of linear
 *       iterations performed
 *
 *  nps  pointer (type int*) to the total number of calls made
 *       to the psolve routine
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT int SpbcgSolve(SpbcgMem mem, void *A_data, FMIC_N_Vector x, FMIC_N_Vector b,
			       int pretype, realtype delta, void *P_data, FMIC_N_Vector sx,
			       FMIC_N_Vector sb, ATimesFn atimes, PSolveFn psolve,
			       realtype *res_norm, int *nli, int *nps);

/* Return values for SpbcgSolve */

#define SPBCG_SUCCESS            0  /* SPBCG algorithm converged          */
#define SPBCG_RES_REDUCED        1  /* SPBCG did NOT converge, but the
				       residual was reduced               */
#define SPBCG_CONV_FAIL          2  /* SPBCG algorithm failed to converge */
#define SPBCG_PSOLVE_FAIL_REC    3  /* psolve failed recoverably          */
#define SPBCG_ATIMES_FAIL_REC    4  /* atimes failed recoverably          */
#define SPBCG_PSET_FAIL_REC      5  /* pset faild recoverably             */

#define SPBCG_MEM_NULL          -1  /* mem argument is NULL               */
#define SPBCG_ATIMES_FAIL_UNREC -2  /* atimes returned failure flag       */
#define SPBCG_PSOLVE_FAIL_UNREC -3  /* psolve failed unrecoverably        */
#define SPBCG_PSET_FAIL_UNREC   -4  /* pset failed unrecoverably          */

/*
 * -----------------------------------------------------------------
 * Function : SpbcgFree
 * -----------------------------------------------------------------
 * SpbcgFree frees the memory allocated by a call to SpbcgMalloc.
 * It is illegal to use the pointer mem after a call to SpbcgFree.
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT void SpbcgFree(SpbcgMem mem);

/*
 * -----------------------------------------------------------------
 * Macro : SPBCG_VTEMP
 * -----------------------------------------------------------------
 * This macro provides access to the vector r in the
 * memory block of the SPBCG module. The argument mem is the
 * memory pointer returned by SpbcgMalloc, of type SpbcgMem,
 * and the macro value is of type FMIC_N_Vector.
 *
 * Note: Only used by IDA (r contains P_inverse F if nli_inc == 0).
 * -----------------------------------------------------------------
 */

#define SPBCG_VTEMP(mem) (mem->r)

#ifdef __cplusplus
}
#endif

#endif
