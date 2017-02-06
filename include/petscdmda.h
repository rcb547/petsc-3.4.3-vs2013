#if !defined(__PETSCDMDA_H)
#define __PETSCDMDA_H

#include <petscdm.h>
#include <petscdmdatypes.h>
#include <petscpf.h>
#include <petscao.h>

/*MC
     DMDA_STENCIL_STAR - "Star"-type stencil. In logical grid coordinates, only (i,j,k), (i+s,j,k), (i,j+s,k),
                       (i,j,k+s) are in the stencil  NOT, for example, (i+s,j+s,k)

     Level: beginner

.seealso: DMDA_STENCIL_BOX, DMDAStencilType, DMDASetStencilType()
M*/

/*MC
     DMDA_STENCIL_BOX - "Box"-type stencil. In logical grid coordinates, any of (i,j,k), (i+s,j+r,k+t) may
                      be in the stencil.

     Level: beginner

.seealso: DMDA_STENCIL_STAR, DMDAStencilType, DMDASetStencilType()
M*/

PETSC_EXTERN const char *const DMDABoundaryTypes[];

PETSC_EXTERN PetscErrorCode DMDASetInterpolationType(DM,DMDAInterpolationType);
PETSC_EXTERN PetscErrorCode DMDAGetInterpolationType(DM,DMDAInterpolationType*);

PETSC_EXTERN PetscErrorCode DMDASetElementType(DM,DMDAElementType);
PETSC_EXTERN PetscErrorCode DMDAGetElementType(DM,DMDAElementType*);
PETSC_EXTERN PetscErrorCode DMDAGetElements(DM,PetscInt *,PetscInt *,const PetscInt*[]);
PETSC_EXTERN PetscErrorCode DMDARestoreElements(DM,PetscInt *,PetscInt *,const PetscInt*[]);

typedef enum { DMDA_X,DMDA_Y,DMDA_Z } DMDADirection;

#define MATSEQUSFFT        "sequsfft"

PETSC_EXTERN PetscErrorCode DMDACreate(MPI_Comm,DM*);
PETSC_EXTERN PetscErrorCode DMDASetDim(DM,PetscInt);
PETSC_EXTERN PetscErrorCode DMDASetSizes(DM,PetscInt,PetscInt,PetscInt);
PETSC_EXTERN PetscErrorCode DMDACreate1d(MPI_Comm,DMDABoundaryType,PetscInt,PetscInt,PetscInt,const PetscInt[],DM *);
PETSC_EXTERN PetscErrorCode DMDACreate2d(MPI_Comm,DMDABoundaryType,DMDABoundaryType,DMDAStencilType,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,const PetscInt[],const PetscInt[],DM*);
PETSC_EXTERN PetscErrorCode DMDACreate3d(MPI_Comm,DMDABoundaryType,DMDABoundaryType,DMDABoundaryType,DMDAStencilType,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,const PetscInt[],const PetscInt[],const PetscInt[],DM*);

PETSC_EXTERN PetscErrorCode DMDAGlobalToNaturalBegin(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMDAGlobalToNaturalEnd(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMDANaturalToGlobalBegin(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMDANaturalToGlobalEnd(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMDALocalToLocalBegin(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMDALocalToLocalEnd(DM,Vec,InsertMode,Vec);
PETSC_EXTERN PetscErrorCode DMDACreateNaturalVector(DM,Vec *);

PETSC_EXTERN PetscErrorCode DMDAGetCorners(DM,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*);
PETSC_EXTERN PetscErrorCode DMDAGetGhostCorners(DM,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*);
PETSC_EXTERN PetscErrorCode DMDAGetInfo(DM,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,DMDABoundaryType*,DMDABoundaryType*,DMDABoundaryType*,DMDAStencilType*);
PETSC_EXTERN PetscErrorCode DMDAGetProcessorSubset(DM,DMDADirection,PetscInt,MPI_Comm*);
PETSC_EXTERN PetscErrorCode DMDAGetProcessorSubsets(DM,DMDADirection,MPI_Comm*);
PETSC_EXTERN PetscErrorCode DMDAGetRay(DM,DMDADirection,PetscInt,Vec*,VecScatter*);

PETSC_EXTERN PetscErrorCode DMDAGlobalToNaturalAllCreate(DM,VecScatter*);
PETSC_EXTERN PetscErrorCode DMDANaturalAllToGlobalCreate(DM,VecScatter*);

PETSC_EXTERN PetscErrorCode DMDAGetGlobalIndices(DM,PetscInt*,PetscInt**);

PETSC_EXTERN PetscErrorCode DMDAGetScatter(DM,VecScatter*,VecScatter*,VecScatter*);
PETSC_EXTERN PetscErrorCode DMDAGetNeighbors(DM,const PetscMPIInt**);

PETSC_EXTERN PetscErrorCode DMDAGetAO(DM,AO*);
PETSC_EXTERN PetscErrorCode DMDASetUniformCoordinates(DM,PetscReal,PetscReal,PetscReal,PetscReal,PetscReal,PetscReal);
PETSC_EXTERN PetscErrorCode DMDAGetBoundingBox(DM,PetscReal[],PetscReal[]);
PETSC_EXTERN PetscErrorCode DMDAGetLocalBoundingBox(DM,PetscReal[],PetscReal[]);
PETSC_EXTERN PetscErrorCode DMDAGetLogicalCoordinate(DM,PetscScalar,PetscScalar,PetscScalar,PetscInt*,PetscInt*,PetscInt*,PetscScalar*,PetscScalar*,PetscScalar*);
/* function to wrap coordinates around boundary */
PETSC_EXTERN PetscErrorCode DMDAMapCoordsToPeriodicDomain(DM,PetscScalar*,PetscScalar*);

PETSC_EXTERN PetscErrorCode DMDAGetReducedDMDA(DM,PetscInt,DM*);

PETSC_EXTERN PetscErrorCode DMDASetFieldName(DM,PetscInt,const char[]);
PETSC_EXTERN PetscErrorCode DMDAGetFieldName(DM,PetscInt,const char**);
PETSC_EXTERN PetscErrorCode DMDASetCoordinateName(DM,PetscInt,const char[]);
PETSC_EXTERN PetscErrorCode DMDAGetCoordinateName(DM,PetscInt,const char**);

PETSC_EXTERN PetscErrorCode DMDASetBoundaryType(DM,DMDABoundaryType,DMDABoundaryType,DMDABoundaryType);
PETSC_EXTERN PetscErrorCode DMDASetDof(DM, PetscInt);
PETSC_EXTERN PetscErrorCode DMDASetOverlap(DM,PetscInt,PetscInt,PetscInt);
PETSC_EXTERN PetscErrorCode DMDAGetOverlap(DM,PetscInt*,PetscInt*,PetscInt*);
PETSC_EXTERN PetscErrorCode DMDASetNumLocalSubDomains(DM,PetscInt);
PETSC_EXTERN PetscErrorCode DMDAGetNumLocalSubDomains(DM,PetscInt*);
PETSC_EXTERN PetscErrorCode DMDAGetOffset(DM,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*);
PETSC_EXTERN PetscErrorCode DMDASetOffset(DM,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt);
PETSC_EXTERN PetscErrorCode DMDAGetNonOverlappingRegion(DM,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*,PetscInt*);
PETSC_EXTERN PetscErrorCode DMDASetNonOverlappingRegion(DM,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt,PetscInt);
PETSC_EXTERN PetscErrorCode DMDASetStencilWidth(DM, PetscInt);
PETSC_EXTERN PetscErrorCode DMDASetOwnershipRanges(DM,const PetscInt[],const PetscInt[],const PetscInt[]);
PETSC_EXTERN PetscErrorCode DMDAGetOwnershipRanges(DM,const PetscInt**,const PetscInt**,const PetscInt**);
PETSC_EXTERN PetscErrorCode DMDASetNumProcs(DM, PetscInt, PetscInt, PetscInt);
PETSC_EXTERN PetscErrorCode DMDASetStencilType(DM, DMDAStencilType);

PETSC_EXTERN PetscErrorCode DMDAVecGetArray(DM,Vec,void *);
PETSC_EXTERN PetscErrorCode DMDAVecRestoreArray(DM,Vec,void *);

PETSC_EXTERN PetscErrorCode DMDAVecGetArrayDOF(DM,Vec,void *);
PETSC_EXTERN PetscErrorCode DMDAVecRestoreArrayDOF(DM,Vec,void *);

PETSC_EXTERN PetscErrorCode DMDASplitComm2d(MPI_Comm,PetscInt,PetscInt,PetscInt,MPI_Comm*);

PETSC_EXTERN PetscErrorCode DMDACreatePatchIS(DM,MatStencil*,MatStencil*,IS*);


/*MC
      DMDACoor2d - Structure for holding 2d (x and y) coordinates.

    Level: intermediate

    Sample Usage:
      DMDACoor2d **coors;
      Vec      vcoors;
      DM       cda;

      DMGetCoordinates(da,&vcoors);
      DMDAGetCoordinateDA(da,&cda);
      DMDAVecGetArray(cda,vcoors,&coors);
      DMDAGetCorners(cda,&mstart,&nstart,0,&m,&n,0)
      for (i=mstart; i<mstart+m; i++) {
        for (j=nstart; j<nstart+n; j++) {
          x = coors[j][i].x;
          y = coors[j][i].y;
          ......
        }
      }
      DMDAVecRestoreArray(dac,vcoors,&coors);

.seealso: DMDACoor3d, DMDAForEachPointBegin(), DMDAGetCoordinateDA(), DMGetCoordinates(), DMDAGetGhostCoordinates()
M*/
typedef struct {PetscScalar x,y;} DMDACoor2d;

/*MC
      DMDACoor3d - Structure for holding 3d (x, y and z) coordinates.

    Level: intermediate

    Sample Usage:
      DMDACoor3d ***coors;
      Vec      vcoors;
      DM       cda;

      DMGetCoordinates(da,&vcoors);
      DMDAGetCoordinateDA(da,&cda);
      DMDAVecGetArray(cda,vcoors,&coors);
      DMDAGetCorners(cda,&mstart,&nstart,&pstart,&m,&n,&p)
      for (i=mstart; i<mstart+m; i++) {
        for (j=nstart; j<nstart+n; j++) {
          for (k=pstart; k<pstart+p; k++) {
            x = coors[k][j][i].x;
            y = coors[k][j][i].y;
            z = coors[k][j][i].z;
          ......
        }
      }
      DMDAVecRestoreArray(dac,vcoors,&coors);

.seealso: DMDACoor2d, DMDAForEachPointBegin(), DMDAGetCoordinateDA(), DMGetCoordinates(), DMDAGetGhostCoordinates()
M*/
typedef struct {PetscScalar x,y,z;} DMDACoor3d;

PETSC_EXTERN PetscErrorCode DMDAGetLocalInfo(DM,DMDALocalInfo*);

PETSC_EXTERN PetscErrorCode MatRegisterDAAD(void);
PETSC_EXTERN PetscErrorCode MatCreateDAAD(DM,Mat*);
PETSC_EXTERN PetscErrorCode MatCreateSeqUSFFT(Vec,DM,Mat*);

PETSC_EXTERN PetscErrorCode DMDASetGetMatrix(DM,PetscErrorCode (*)(DM, MatType,Mat *));
PETSC_EXTERN PetscErrorCode DMDASetBlockFills(DM,const PetscInt*,const PetscInt*);
PETSC_EXTERN PetscErrorCode DMDASetRefinementFactor(DM,PetscInt,PetscInt,PetscInt);
PETSC_EXTERN PetscErrorCode DMDAGetRefinementFactor(DM,PetscInt*,PetscInt*,PetscInt*);

PETSC_EXTERN PetscErrorCode DMDAGetArray(DM,PetscBool ,void*);
PETSC_EXTERN PetscErrorCode DMDARestoreArray(DM,PetscBool ,void*);

PETSC_EXTERN PetscErrorCode DMDACreatePF(DM,PF*);

PETSC_EXTERN PetscErrorCode DMDAGetNumCells(DM, PetscInt *);
PETSC_EXTERN PetscErrorCode DMDAGetNumVertices(DM, PetscInt *, PetscInt *, PetscInt *, PetscInt *);
PETSC_EXTERN PetscErrorCode DMDAGetNumFaces(DM, PetscInt *, PetscInt *, PetscInt *, PetscInt *, PetscInt *, PetscInt *);
PETSC_EXTERN PetscErrorCode DMDAGetHeightStratum(DM, PetscInt, PetscInt *, PetscInt *);
PETSC_EXTERN PetscErrorCode DMDACreateSection(DM, PetscInt[], PetscInt[], PetscInt[], PetscInt[]);
PETSC_EXTERN PetscErrorCode DMDAComputeCellGeometry(DM, PetscInt, PetscQuadrature *, PetscReal [], PetscReal [], PetscReal [], PetscReal []);
PETSC_EXTERN PetscErrorCode DMDAVecGetClosure(DM, PetscSection, Vec, PetscInt, const PetscScalar **);
PETSC_EXTERN PetscErrorCode DMDAVecSetClosure(DM, PetscSection, Vec, PetscInt, const PetscScalar *, InsertMode);
PETSC_EXTERN PetscErrorCode DMDAGetClosure(DM,PetscSection,PetscInt,PetscInt*,const PetscInt**);
PETSC_EXTERN PetscErrorCode DMDARestoreClosure(DM,PetscSection,PetscInt,PetscInt*,const PetscInt**);
PETSC_EXTERN PetscErrorCode DMDAGetClosureScalar(DM,PetscSection,PetscInt,PetscScalar*,const PetscScalar**);
PETSC_EXTERN PetscErrorCode DMDARestoreClosureScalar(DM,PetscSection,PetscInt,PetscScalar*,const PetscScalar**);
PETSC_EXTERN PetscErrorCode DMDASetClosureScalar(DM,PetscSection,PetscInt,PetscScalar*,const PetscScalar*,InsertMode);
PETSC_EXTERN PetscErrorCode DMDAConvertToCell(DM, MatStencil, PetscInt *);

#endif
