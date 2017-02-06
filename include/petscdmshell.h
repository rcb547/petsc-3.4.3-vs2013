#if !defined(__PETSCDMSHELL_H)
#define __PETSCDMSHELL_H

#include <petscdm.h>

PETSC_EXTERN PetscErrorCode DMShellCreate(MPI_Comm,DM*);
PETSC_EXTERN PetscErrorCode DMShellSetMatrix(DM,Mat);
PETSC_EXTERN PetscErrorCode DMShellSetGlobalVector(DM,Vec);
PETSC_EXTERN PetscErrorCode DMShellSetLocalVector(DM,Vec);
PETSC_EXTERN PetscErrorCode DMShellSetCreateGlobalVector(DM,PetscErrorCode (*)(DM,Vec*));
PETSC_EXTERN PetscErrorCode DMShellSetCreateLocalVector(DM,PetscErrorCode (*)(DM,Vec*));
PETSC_EXTERN PetscErrorCode DMShellSetGlobalToLocal(DM,PetscErrorCode (*)(DM,Vec,InsertMode,Vec),PetscErrorCode (*)(DM,Vec,InsertMode,Vec));
PETSC_EXTERN PetscErrorCode DMShellSetGlobalToLocalVecScatter(DM,VecScatter);
PETSC_EXTERN PetscErrorCode DMShellSetLocalToGlobal(DM,PetscErrorCode (*)(DM,Vec,InsertMode,Vec),PetscErrorCode (*)(DM,Vec,InsertMode,Vec));
PETSC_EXTERN PetscErrorCode DMShellSetLocalToGlobalVecScatter(DM,VecScatter);
PETSC_EXTERN PetscErrorCode DMShellSetCreateMatrix(DM,PetscErrorCode (*)(DM,MatType,Mat*));
PETSC_EXTERN PetscErrorCode DMGlobalToLocalBeginDefaultShell(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMGlobalToLocalEndDefaultShell(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMLocalToGlobalBeginDefaultShell(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMLocalToGlobalEndDefaultShell(DM,Vec,InsertMode,Vec);

#endif
