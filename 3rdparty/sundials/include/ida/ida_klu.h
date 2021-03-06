/*
 * -----------------------------------------------------------------
 * $Revision: 4558 $
 * $Date: 2015-10-05 09:04:16 -0700 (Mon, 05 Oct 2015) $
 * ----------------------------------------------------------------- 
 * Programmer(s): Carol S. Woodward @ LLNL
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
 * This is the header file for the IDAKLU linear solver module.
 * -----------------------------------------------------------------
 */

#ifndef _IDAKLU_H
#define _IDAKLU_H

#include "ida/ida_sparse.h"
#include "sundials/sundials_sparse.h"

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/*
 * -----------------------------------------------------------------
 * Function : IDAKLU
 * -----------------------------------------------------------------
 * A call to the IDAKLU function links the main integrator      
 * with the IDAKLU linear solver module.                        
 *                                                                
 * ida_mem is the pointer to integrator memory returned by        
 *     IDACreate.             
 *
 *                                                                
 * IDAKLU returns:                                              
 *     IDASLU_SUCCESS   = 0  if successful                              
 *     IDASLU_LMEM_FAIL = -1 if there was a memory allocation failure   
 *     IDASLU_ILL_INPUT = -2 if NVECTOR found incompatible           
 *                                                                
 * NOTE: The KLU linear solver assumes a serial implementation  
 *       of the NVECTOR package. Therefore, IDAKLU will first
 *       test for a compatible FMIC_N_Vector internal representation
 *       by checking that the functions FMIC_N_VGetArrayPointer and
 *       FMIC_N_VSetArrayPointer exist.
 * -----------------------------------------------------------------
 */

  SUNDIALS_EXPORT int IDAKLU(void *ida_mem, int n, int nnz, int sparesetype); 

/*
 * -----------------------------------------------------------------
 * IDAKLUReInit
 * -----------------------------------------------------------------
 * This routine reinitializes memory and flags for a new factorization 
 * (symbolic and numeric) to be conducted at the next solver setup
 * call.  This routine is useful in the cases where the number of nonzeroes 
 * has changed or if the structure of the linear system has changed
 * which would require a new symbolic (and numeric factorization).
 *
 * The reinit_type argumenmt governs the level of reinitialization:
 *
 * reinit_type = 1: The Jacobian matrix will be destroyed and 
 *                  a new one will be allocated based on the nnz
 *                  value passed to this call. New symbolic and
 *                  numeric factorizations will be completed at the next
 *                  solver setup.
 *
 * reinit_type = 2: Only symbolic and numeric factorizations will be 
 *                  completed.  It is assumed that the Jacobian size
 *                  has not exceeded the size of nnz given in the prior
 *                  call to IDAKLU.
 *
 * This routine assumes no other changes to solver use are necessary.
 *
 * The return value is IDASLS_SUCCESS = 0, IDASLS_MEM_NULL = -1, 
 * IDASLS_LMEM_NULL = -2, IDASLS_ILL_INPUT = -3, or IDASLS_MEM_FAIL = -4.
 *
 * -----------------------------------------------------------------
 */

SUNDIALS_EXPORT int IDAKLUReInit(void *ida_mem_v, int n, int nnz, 
				 int reinit_type);

/*
 * -----------------------------------------------------------------
 * Function: IDAKLUB
 * -----------------------------------------------------------------
 * IDAKLUB links the main IDAS integrator with the IDAKLU
 * linear solver for the backward integration.
 * -----------------------------------------------------------------
 */

  SUNDIALS_EXPORT int IDAKLUB(void *ida_mem, int which, int nB, int nnzB);

/* 
 * -----------------------------------------------------------------
 * Optional Input Specification Functions
 * -----------------------------------------------------------------
 *
 * IDAKLUSetOrdering sets the ordering used by KLU for reducing fill.
 * Options are: 0 for AMD, 1 for COLAMD, and 2 for the natural ordering.
 * The default used in IDA is 1 for COLAMD.
 * -----------------------------------------------------------------
 */

  SUNDIALS_EXPORT int IDAKLUSetOrdering(void *ida_mem, int ordering_choice); 


  
#ifdef __cplusplus
}
#endif

#endif
