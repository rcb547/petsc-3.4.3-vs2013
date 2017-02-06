
#ifndef _PCIMPL_H
#define _PCIMPL_H

#include <petscksp.h>
#include <petscpc.h>
#include <petsc-private/petscimpl.h>

typedef struct _PCOps *PCOps;
struct _PCOps {
  PetscErrorCode (*setup)(PC);
  PetscErrorCode (*apply)(PC,Vec,Vec);
  PetscErrorCode (*applyrichardson)(PC,Vec,Vec,Vec,PetscReal,PetscReal,PetscReal,PetscInt,PetscBool ,PetscInt*,PCRichardsonConvergedReason*);
  PetscErrorCode (*applyBA)(PC,PCSide,Vec,Vec,Vec);
  PetscErrorCode (*applytranspose)(PC,Vec,Vec);
  PetscErrorCode (*applyBAtranspose)(PC,PetscInt,Vec,Vec,Vec);
  PetscErrorCode (*setfromoptions)(PC);
  PetscErrorCode (*presolve)(PC,KSP,Vec,Vec);
  PetscErrorCode (*postsolve)(PC,KSP,Vec,Vec);
  PetscErrorCode (*getfactoredmatrix)(PC,Mat*);
  PetscErrorCode (*applysymmetricleft)(PC,Vec,Vec);
  PetscErrorCode (*applysymmetricright)(PC,Vec,Vec);
  PetscErrorCode (*setuponblocks)(PC);
  PetscErrorCode (*destroy)(PC);
  PetscErrorCode (*view)(PC,PetscViewer);
  PetscErrorCode (*reset)(PC);
  PetscErrorCode (*load)(PC,PetscViewer);
};

/*
   Preconditioner context
*/
struct _p_PC {
  PETSCHEADER(struct _PCOps);
  DM             dm;
  PetscInt       setupcalled;
  PetscInt       setfromoptionscalled;
  MatStructure   flag;
  Mat            mat,pmat;
  Vec            diagonalscaleright,diagonalscaleleft; /* used for time integration scaling */
  PetscBool      diagonalscale;
  PetscBool      nonzero_guess; /* used by PCKSP, PCREDUNDANT and PCHMPI */
  PetscBool      useAmat; /* used by several PC that including applying the operator inside the preconditioner */
  PetscErrorCode (*modifysubmatrices)(PC,PetscInt,const IS[],const IS[],Mat[],void*); /* user provided routine */
  void           *modifysubmatricesP; /* context for user routine */
  void           *data;
  PetscInt       presolvedone;  /* has PCPreSolve() already been run */
  void           *user;             /* optional user-defined context */
};

PETSC_EXTERN PetscLogEvent PC_SetUp, PC_SetUpOnBlocks, PC_Apply, PC_ApplyCoarse, PC_ApplyMultiple, PC_ApplySymmetricLeft;
PETSC_EXTERN PetscLogEvent PC_ApplySymmetricRight, PC_ModifySubMatrices, PC_ApplyOnBlocks, PC_ApplyTransposeOnBlocks, PC_ApplyOnMproc;

#endif
