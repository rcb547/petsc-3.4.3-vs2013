!
!
!  Include file for Fortran use of the IS (index set) package in PETSc
!
#if !defined (__PETSCISDEF_H)
#define __PETSCISDEF_H

#include "finclude/petscsysdef.h"

#if !defined(PETSC_USE_FORTRAN_DATATYPES)
#define IS PetscFortranAddr
#define ISColoring PetscFortranAddr
#define PetscSection PetscFortranAddr
#endif

#define PetscSF PetscFortranAddr
#define PetscLayout PetscFortranAddr

#define ISType PetscEnum
#define ISLocalToGlobalMapping PetscFortranAddr
#define ISGlobalToLocalMappingType PetscEnum
#define ISColoringType PetscEnum

#define ISGENERAL 'general'
#define ISSTRIDE 'stride'
#define ISBLOCK 'block'
#endif
