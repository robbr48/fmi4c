/*
 * -----------------------------------------------------------------
 * $Revision: 4790 $
 * $Date: 2016-06-29 14:47:05 -0700 (Wed, 29 Jun 2016) $
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
 * This is the implementation file for a generic NVECTOR package.
 * It contains the implementation of the FMIC_N_Vector operations listed
 * in nvector.h.
 * -----------------------------------------------------------------
 */

#include <stdlib.h>

#include <sundials/sundials_nvector.h>

/*
 * -----------------------------------------------------------------
 * Functions in the 'ops' structure
 * -----------------------------------------------------------------
 */

FMIC_N_Vector_ID FMIC_N_VGetVectorID(FMIC_N_Vector w)
{
  FMIC_N_Vector_ID id;
  id = w->ops->nvgetvectorid(w);
  return(id);
}

FMIC_N_Vector FMIC_N_VClone(FMIC_N_Vector w)
{
  FMIC_N_Vector v = NULL;
  v = w->ops->nvclone(w);
  return(v);
}

FMIC_N_Vector FMIC_N_VCloneEmpty(FMIC_N_Vector w)
{
  FMIC_N_Vector v = NULL;
  v = w->ops->nvcloneempty(w);
  return(v);
}

void FMIC_N_VDestroy(FMIC_N_Vector v)
{
  if (v==NULL) return;
  v->ops->nvdestroy(v);
  return;
}

void FMIC_N_VSpace(FMIC_N_Vector v, long int *lrw, long int *liw)
{
  v->ops->nvspace(v, lrw, liw);
  return;
}

realtype *FMIC_N_VGetArrayPointer(FMIC_N_Vector v)
{
  return((realtype *) v->ops->nvgetarraypointer(v));
}

void FMIC_N_VSetArrayPointer(realtype *v_data, FMIC_N_Vector v)
{
  v->ops->nvsetarraypointer(v_data, v);
  return;
}

void FMIC_N_VLinearSum(realtype a, FMIC_N_Vector x, realtype b, FMIC_N_Vector y, FMIC_N_Vector z)
{
  z->ops->nvlinearsum(a, x, b, y, z);
  return;
}

void FMIC_N_VConst(realtype c, FMIC_N_Vector z)
{
  z->ops->nvconst(c, z);
  return;
}

void FMIC_N_VProd(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z)
{
  z->ops->nvprod(x, y, z);
  return;
}

void FMIC_N_VDiv(FMIC_N_Vector x, FMIC_N_Vector y, FMIC_N_Vector z)
{
  z->ops->nvdiv(x, y, z);
  return;
}

void FMIC_N_VScale(realtype c, FMIC_N_Vector x, FMIC_N_Vector z) 
{
  z->ops->nvscale(c, x, z);
  return;
}

void FMIC_N_VAbs(FMIC_N_Vector x, FMIC_N_Vector z)
{
  z->ops->nvabs(x, z);
  return;
}

void FMIC_N_VInv(FMIC_N_Vector x, FMIC_N_Vector z)
{
  z->ops->nvinv(x, z);
  return;
}

void FMIC_N_VAddConst(FMIC_N_Vector x, realtype b, FMIC_N_Vector z)
{
  z->ops->nvaddconst(x, b, z);
  return;
}

realtype FMIC_N_VDotProd(FMIC_N_Vector x, FMIC_N_Vector y)
{
  return((realtype) y->ops->nvdotprod(x, y));
}

realtype FMIC_N_VMaxNorm(FMIC_N_Vector x)
{
  return((realtype) x->ops->nvmaxnorm(x));
}

realtype FMIC_N_VWrmsNorm(FMIC_N_Vector x, FMIC_N_Vector w)
{
  return((realtype) x->ops->nvwrmsnorm(x, w));
}

realtype FMIC_N_VWrmsNormMask(FMIC_N_Vector x, FMIC_N_Vector w, FMIC_N_Vector id)
{
  return((realtype) x->ops->nvwrmsnormmask(x, w, id));
}

realtype FMIC_N_VMin(FMIC_N_Vector x)
{
  return((realtype) x->ops->nvmin(x));
}

realtype FMIC_N_VWL2Norm(FMIC_N_Vector x, FMIC_N_Vector w)
{
  return((realtype) x->ops->nvwl2norm(x, w));
}

realtype FMIC_N_VL1Norm(FMIC_N_Vector x)
{
  return((realtype) x->ops->nvl1norm(x));
}

void FMIC_N_VCompare(realtype c, FMIC_N_Vector x, FMIC_N_Vector z)
{
  z->ops->nvcompare(c, x, z);
  return;
}

booleantype FMIC_N_VInvTest(FMIC_N_Vector x, FMIC_N_Vector z)
{
  return((booleantype) z->ops->nvinvtest(x, z));
}

booleantype FMIC_N_VConstrMask(FMIC_N_Vector c, FMIC_N_Vector x, FMIC_N_Vector m)
{
  return((booleantype) x->ops->nvconstrmask(c, x, m));
}

realtype FMIC_N_VMinQuotient(FMIC_N_Vector num, FMIC_N_Vector denom)
{
  return((realtype) num->ops->nvminquotient(num, denom));
}

/*
 * -----------------------------------------------------------------
 * Additional functions exported by the generic NVECTOR:
 *   FMIC_N_VCloneEmptyVectorArray
 *   FMIC_N_VCloneVectorArray
 *   FMIC_N_VDestroyVectorArray
 * -----------------------------------------------------------------
 */

FMIC_N_Vector *FMIC_N_VCloneEmptyVectorArray(int count, FMIC_N_Vector w)
{
  FMIC_N_Vector *vs = NULL;
  int j;

  if (count <= 0) return(NULL);

  vs = (FMIC_N_Vector *) malloc(count * sizeof(FMIC_N_Vector));
  if(vs == NULL) return(NULL);

  for (j = 0; j < count; j++) {
    vs[j] = FMIC_N_VCloneEmpty(w);
    if (vs[j] == NULL) {
      FMIC_N_VDestroyVectorArray(vs, j-1);
      return(NULL);
    }
  }

  return(vs);
}

FMIC_N_Vector *FMIC_N_VCloneVectorArray(int count, FMIC_N_Vector w)
{
  FMIC_N_Vector *vs = NULL;
  int j;

  if (count <= 0) return(NULL);

  vs = (FMIC_N_Vector *) malloc(count * sizeof(FMIC_N_Vector));
  if(vs == NULL) return(NULL);

  for (j = 0; j < count; j++) {
    vs[j] = FMIC_N_VClone(w);
    if (vs[j] == NULL) {
      FMIC_N_VDestroyVectorArray(vs, j-1);
      return(NULL);
    }
  }

  return(vs);
}

void FMIC_N_VDestroyVectorArray(FMIC_N_Vector *vs, int count)
{
  int j;

  if (vs==NULL) return;

  for (j = 0; j < count; j++) FMIC_N_VDestroy(vs[j]);

  free(vs); vs = NULL;

  return;
}
