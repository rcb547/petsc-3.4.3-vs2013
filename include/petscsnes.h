/*
    User interface for the nonlinear solvers package.
*/
#if !defined(__PETSCSNES_H)
#define __PETSCSNES_H
#include <petscksp.h>
#include <petscdmtypes.h>
#include <petscdmdatypes.h>

/*S
     SNES - Abstract PETSc object that manages all nonlinear solves

   Level: beginner

  Concepts: nonlinear solvers

.seealso:  SNESCreate(), SNESSetType(), SNESType, TS, KSP, KSP, PC, SNESDestroy()
S*/
typedef struct _p_SNES* SNES;

/*J
    SNESType - String with the name of a PETSc SNES method.

   Level: beginner

.seealso: SNESSetType(), SNES, SNESCreate(), SNESDestroy(), SNESSetFromOptions()
J*/
typedef const char* SNESType;
#define SNESNEWTONLS     "newtonls"
#define SNESNEWTONTR     "newtontr"
#define SNESPYTHON       "python"
#define SNESTEST         "test"
#define SNESNRICHARDSON  "nrichardson"
#define SNESKSPONLY      "ksponly"
#define SNESVINEWTONRSLS "vinewtonrsls"
#define SNESVINEWTONSSLS "vinewtonssls"
#define SNESNGMRES       "ngmres"
#define SNESQN           "qn"
#define SNESSHELL        "shell"
#define SNESGS           "gs"
#define SNESNCG          "ncg"
#define SNESFAS          "fas"
#define SNESMS           "ms"
#define SNESNASM         "nasm"
#define SNESANDERSON     "anderson"
#define SNESASPIN        "aspin"

/* Logging support */
PETSC_EXTERN PetscClassId SNES_CLASSID;
PETSC_EXTERN PetscClassId DMSNES_CLASSID;

PETSC_EXTERN PetscErrorCode SNESInitializePackage(void);

PETSC_EXTERN PetscErrorCode SNESCreate(MPI_Comm,SNES*);
PETSC_EXTERN PetscErrorCode SNESReset(SNES);
PETSC_EXTERN PetscErrorCode SNESDestroy(SNES*);
PETSC_EXTERN PetscErrorCode SNESSetType(SNES,SNESType);
PETSC_EXTERN PetscErrorCode SNESMonitor(SNES,PetscInt,PetscReal);
PETSC_EXTERN PetscErrorCode SNESMonitorSet(SNES,PetscErrorCode(*)(SNES,PetscInt,PetscReal,void*),void *,PetscErrorCode (*)(void**));
PETSC_EXTERN PetscErrorCode SNESMonitorCancel(SNES);
PETSC_EXTERN PetscErrorCode SNESSetConvergenceHistory(SNES,PetscReal[],PetscInt[],PetscInt,PetscBool );
PETSC_EXTERN PetscErrorCode SNESGetConvergenceHistory(SNES,PetscReal*[],PetscInt *[],PetscInt *);
PETSC_EXTERN PetscErrorCode SNESSetUp(SNES);
PETSC_EXTERN PetscErrorCode SNESSolve(SNES,Vec,Vec);
PETSC_EXTERN PetscErrorCode SNESSetErrorIfNotConverged(SNES,PetscBool );
PETSC_EXTERN PetscErrorCode SNESGetErrorIfNotConverged(SNES,PetscBool  *);

PETSC_EXTERN PetscErrorCode SNESSetWorkVecs(SNES,PetscInt);

PETSC_EXTERN PetscErrorCode SNESAddOptionsChecker(PetscErrorCode (*)(SNES));

PETSC_EXTERN PetscErrorCode SNESSetUpdate(SNES, PetscErrorCode (*)(SNES, PetscInt));

PETSC_EXTERN PetscErrorCode SNESRegisterAll(void);

PETSC_EXTERN PetscErrorCode SNESRegister(const char[],PetscErrorCode (*)(SNES));

PETSC_EXTERN PetscErrorCode SNESGetKSP(SNES,KSP*);
PETSC_EXTERN PetscErrorCode SNESSetKSP(SNES,KSP);
PETSC_EXTERN PetscErrorCode SNESGetSolution(SNES,Vec*);
PETSC_EXTERN PetscErrorCode SNESGetSolutionUpdate(SNES,Vec*);
PETSC_EXTERN PetscErrorCode SNESGetRhs(SNES,Vec*);
PETSC_EXTERN PetscErrorCode SNESView(SNES,PetscViewer);
PETSC_EXTERN PetscErrorCode SNESLoad(SNES,PetscViewer);

#define SNES_FILE_CLASSID 1211224

PETSC_EXTERN PetscErrorCode SNESSetOptionsPrefix(SNES,const char[]);
PETSC_EXTERN PetscErrorCode SNESAppendOptionsPrefix(SNES,const char[]);
PETSC_EXTERN PetscErrorCode SNESGetOptionsPrefix(SNES,const char*[]);
PETSC_EXTERN PetscErrorCode SNESSetFromOptions(SNES);

PETSC_EXTERN PetscErrorCode MatCreateSNESMF(SNES,Mat*);
PETSC_EXTERN PetscErrorCode MatMFFDComputeJacobian(SNES,Vec,Mat*,Mat*,MatStructure*,void*);

PETSC_EXTERN PetscErrorCode MatDAADSetSNES(Mat,SNES);

PETSC_EXTERN PetscErrorCode SNESGetType(SNES,SNESType*);
PETSC_EXTERN PetscErrorCode SNESMonitorDefault(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorRange(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorRatio(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorSetRatio(SNES,PetscViewer);
PETSC_EXTERN PetscErrorCode SNESMonitorSolution(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorResidual(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorSolutionUpdate(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorDefaultShort(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESMonitorJacUpdateSpectrum(SNES,PetscInt,PetscReal,void *);
PETSC_EXTERN PetscErrorCode SNESSetTolerances(SNES,PetscReal,PetscReal,PetscReal,PetscInt,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGetTolerances(SNES,PetscReal*,PetscReal*,PetscReal*,PetscInt*,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESSetTrustRegionTolerance(SNES,PetscReal);
PETSC_EXTERN PetscErrorCode SNESGetFunctionNorm(SNES,PetscReal*);
PETSC_EXTERN PetscErrorCode SNESSetFunctionNorm(SNES,PetscReal);
PETSC_EXTERN PetscErrorCode SNESGetIterationNumber(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESSetIterationNumber(SNES,PetscInt);

PETSC_EXTERN PetscErrorCode SNESGetNonlinearStepFailures(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESSetMaxNonlinearStepFailures(SNES,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGetMaxNonlinearStepFailures(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESGetNumberFunctionEvals(SNES,PetscInt*);

PETSC_EXTERN PetscErrorCode SNESSetLagPreconditioner(SNES,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGetLagPreconditioner(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESSetLagJacobian(SNES,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGetLagJacobian(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESSetGridSequence(SNES,PetscInt);

PETSC_EXTERN PetscErrorCode SNESGetLinearSolveIterations(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESGetLinearSolveFailures(SNES,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESSetMaxLinearSolveFailures(SNES,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGetMaxLinearSolveFailures(SNES,PetscInt*);

PETSC_EXTERN PetscErrorCode SNESKSPSetUseEW(SNES,PetscBool );
PETSC_EXTERN PetscErrorCode SNESKSPGetUseEW(SNES,PetscBool *);
PETSC_EXTERN PetscErrorCode SNESKSPSetParametersEW(SNES,PetscInt,PetscReal,PetscReal,PetscReal,PetscReal,PetscReal,PetscReal);
PETSC_EXTERN PetscErrorCode SNESKSPGetParametersEW(SNES,PetscInt*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*);

#include <petscdrawtypes.h>
PETSC_EXTERN PetscErrorCode SNESMonitorLGCreate(const char[],const char[],int,int,int,int,PetscDrawLG*);
PETSC_EXTERN PetscErrorCode SNESMonitorLGResidualNorm(SNES,PetscInt,PetscReal,void*);
PETSC_EXTERN PetscErrorCode SNESMonitorLGDestroy(PetscDrawLG*);
PETSC_EXTERN PetscErrorCode SNESMonitorLGRange(SNES,PetscInt,PetscReal,void*);

PETSC_EXTERN PetscErrorCode SNESSetApplicationContext(SNES,void *);
PETSC_EXTERN PetscErrorCode SNESGetApplicationContext(SNES,void *);
PETSC_EXTERN PetscErrorCode SNESSetComputeApplicationContext(SNES,PetscErrorCode (*)(SNES,void**),PetscErrorCode (*)(void**));

PETSC_EXTERN PetscErrorCode SNESPythonSetType(SNES,const char[]);

PETSC_EXTERN PetscErrorCode SNESSetFunctionDomainError(SNES);
PETSC_EXTERN PetscErrorCode SNESGetFunctionDomainError(SNES, PetscBool *);

/*E
    SNESConvergedReason - reason a SNES method was said to
         have converged or diverged

   Level: beginner

   The two most common reasons for divergence are
$   1) an incorrectly coded or computed Jacobian or
$   2) failure or lack of convergence in the linear system (in this case we recommend
$      testing with -pc_type lu to eliminate the linear solver as the cause of the problem).

   Diverged Reasons:
.    SNES_DIVERGED_LOCAL_MIN - this can only occur when using the line-search variant of SNES.
       The line search wants to minimize Q(alpha) = 1/2 || F(x + alpha s) ||^2_2  this occurs
       at Q'(alpha) = s^T F'(x+alpha s)^T F(x+alpha s) = 0. If s is the Newton direction - F'(x)^(-1)F(x) then
       you get Q'(alpha) = -F(x)^T F'(x)^(-1)^T F'(x+alpha s)F(x+alpha s); when alpha = 0
       Q'(0) = - ||F(x)||^2_2 which is always NEGATIVE if F'(x) is invertible. This means the Newton
       direction is a descent direction and the line search should succeed if alpha is small enough.

       If F'(x) is NOT invertible AND F'(x)^T F(x) = 0 then Q'(0) = 0 and the Newton direction
       is NOT a descent direction so the line search will fail. All one can do at this point
       is change the initial guess and try again.

       An alternative explanation: Newton's method can be regarded as replacing the function with
       its linear approximation and minimizing the 2-norm of that. That is F(x+s) approx F(x) + F'(x)s
       so we minimize || F(x) + F'(x) s ||^2_2; do this using Least Squares. If F'(x) is invertible then
       s = - F'(x)^(-1)F(x) otherwise F'(x)^T F'(x) s = -F'(x)^T F(x). If F'(x)^T F(x) is NOT zero then there
       exists a nontrival (that is F'(x)s != 0) solution to the equation and this direction is
       s = - [F'(x)^T F'(x)]^(-1) F'(x)^T F(x) so Q'(0) = - F(x)^T F'(x) [F'(x)^T F'(x)]^(-T) F'(x)^T F(x)
       = - (F'(x)^T F(x)) [F'(x)^T F'(x)]^(-T) (F'(x)^T F(x)). Since we are assuming (F'(x)^T F(x)) != 0
       and F'(x)^T F'(x) has no negative eigenvalues Q'(0) < 0 so s is a descent direction and the line
       search should succeed for small enough alpha.

       Note that this RARELY happens in practice. Far more likely the linear system is not being solved
       (well enough?) or the Jacobian is wrong.

   SNES_DIVERGED_MAX_IT means that the solver reached the maximum number of iterations without satisfying any
   convergence criteria. SNES_CONVERGED_ITS means that SNESSkipConverged() was chosen as the convergence test;
   thus the usual convergence criteria have not been checked and may or may not be satisfied.

   Developer Notes: this must match finclude/petscsnes.h

       The string versions of these are in SNESConvergedReason, if you change any value here you must
     also adjust that array.

   Each reason has its own manual page.

.seealso: SNESSolve(), SNESGetConvergedReason(), KSPConvergedReason, SNESSetConvergenceTest()
E*/
typedef enum {/* converged */
              SNES_CONVERGED_FNORM_ABS         =  2, /* ||F|| < atol */
              SNES_CONVERGED_FNORM_RELATIVE    =  3, /* ||F|| < rtol*||F_initial|| */
              SNES_CONVERGED_SNORM_RELATIVE    =  4, /* Newton computed step size small; || delta x || < stol || x ||*/
              SNES_CONVERGED_ITS               =  5, /* maximum iterations reached */
              SNES_CONVERGED_TR_DELTA          =  7,
              /* diverged */
              SNES_DIVERGED_FUNCTION_DOMAIN     = -1, /* the new x location passed the function is not in the domain of F */
              SNES_DIVERGED_FUNCTION_COUNT      = -2,
              SNES_DIVERGED_LINEAR_SOLVE        = -3, /* the linear solve failed */
              SNES_DIVERGED_FNORM_NAN           = -4,
              SNES_DIVERGED_MAX_IT              = -5,
              SNES_DIVERGED_LINE_SEARCH         = -6, /* the line search failed */
              SNES_DIVERGED_INNER               = -7, /* inner solve failed */
              SNES_DIVERGED_LOCAL_MIN           = -8, /* || J^T b || is small, implies converged to local minimum of F() */
              SNES_CONVERGED_ITERATING          =  0} SNESConvergedReason;
PETSC_EXTERN const char *const*SNESConvergedReasons;

/*MC
     SNES_CONVERGED_FNORM_ABS - 2-norm(F) <= abstol

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_CONVERGED_FNORM_RELATIVE - 2-norm(F) <= rtol*2-norm(F(x_0)) where x_0 is the initial guess

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_CONVERGED_SNORM_RELATIVE - The 2-norm of the last step <= stol * 2-norm(x) where x is the current
          solution and stol is the 4th argument to SNESSetTolerances()

     Options Database Keys:
      -snes_stol <stol> - the step tolerance

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_DIVERGED_FUNCTION_COUNT - The user provided function has been called more times then the final
         argument to SNESSetTolerances()

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_DIVERGED_FNORM_NAN - the 2-norm of the current function evaluation is not-a-number (NaN), this
      is usually caused by a division of 0 by 0.

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_DIVERGED_MAX_IT - SNESSolve() has reached the maximum number of iterations requested

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_DIVERGED_LINE_SEARCH - The line search has failed. This only occurs for a SNES solvers that use a line search

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances(), SNESLineSearch

M*/

/*MC
     SNES_DIVERGED_LOCAL_MIN - the algorithm seems to have stagnated at a local minimum that is not zero.
        See the manual page for SNESConvergedReason for more details

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

/*MC
     SNES_CONERGED_ITERATING - this only occurs if SNESGetConvergedReason() is called during the SNESSolve()

   Level: beginner

.seealso:  SNESSolve(), SNESGetConvergedReason(), SNESConvergedReason, SNESSetTolerances()

M*/

PETSC_EXTERN PetscErrorCode SNESSetConvergenceTest(SNES,PetscErrorCode (*)(SNES,PetscInt,PetscReal,PetscReal,PetscReal,SNESConvergedReason*,void*),void*,PetscErrorCode (*)(void*));
PETSC_EXTERN PetscErrorCode SNESConvergedDefault(SNES,PetscInt,PetscReal,PetscReal,PetscReal,SNESConvergedReason*,void*);
PETSC_EXTERN PetscErrorCode SNESSkipConverged(SNES,PetscInt,PetscReal,PetscReal,PetscReal,SNESConvergedReason*,void*);
PETSC_EXTERN PetscErrorCode SNESGetConvergedReason(SNES,SNESConvergedReason*);

PETSC_EXTERN PetscErrorCode SNESDMMeshComputeFunction(SNES,Vec,Vec,void*);
PETSC_EXTERN PetscErrorCode SNESDMMeshComputeJacobian(SNES,Vec,Mat*,Mat*,MatStructure*,void*);

/* --------- Solving systems of nonlinear equations --------------- */
PETSC_EXTERN PetscErrorCode SNESSetFunction(SNES,Vec,PetscErrorCode (*SNESFunction)(SNES,Vec,Vec,void*),void*);
PETSC_EXTERN PetscErrorCode SNESGetFunction(SNES,Vec*,PetscErrorCode (**SNESFunction)(SNES,Vec,Vec,void*),void**);
PETSC_EXTERN PetscErrorCode SNESComputeFunction(SNES,Vec,Vec);
PETSC_EXTERN PetscErrorCode SNESSetJacobian(SNES,Mat,Mat,PetscErrorCode (*SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void*);
PETSC_EXTERN PetscErrorCode SNESGetJacobian(SNES,Mat*,Mat*,PetscErrorCode (**SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void**);
PETSC_EXTERN PetscErrorCode SNESObjectiveComputeFunctionDefaultFD(SNES,Vec,Vec,void*);
PETSC_EXTERN PetscErrorCode SNESComputeJacobianDefault(SNES,Vec,Mat*,Mat*,MatStructure*,void*);
PETSC_EXTERN PetscErrorCode SNESComputeJacobianDefaultColor(SNES,Vec,Mat*,Mat*,MatStructure*,void*);
PETSC_EXTERN PetscErrorCode SNESSetComputeInitialGuess(SNES,PetscErrorCode (*)(SNES,Vec,void*),void*);
PETSC_EXTERN PetscErrorCode SNESSetPicard(SNES,Vec,PetscErrorCode (*SNESFunction)(SNES,Vec,Vec,void*),Mat,Mat,PetscErrorCode (*SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void*);
PETSC_EXTERN PetscErrorCode SNESGetPicard(SNES,Vec*,PetscErrorCode (**SNESFunction)(SNES,Vec,Vec,void*),Mat*,Mat*,PetscErrorCode (**SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void**);
PETSC_EXTERN PetscErrorCode SNESSetInitialFunction(SNES,Vec);
PETSC_EXTERN PetscErrorCode SNESSetInitialFunctionNorm(SNES,PetscReal);

PETSC_EXTERN PetscErrorCode SNESSetObjective(SNES,PetscErrorCode (*SNESObjectiveFunction)(SNES,Vec,PetscReal *,void*),void*);
PETSC_EXTERN PetscErrorCode SNESGetObjective(SNES,PetscErrorCode (**SNESObjectiveFunction)(SNES,Vec,PetscReal *,void*),void**);
PETSC_EXTERN PetscErrorCode SNESComputeObjective(SNES,Vec,PetscReal *);

/*E
    SNESNormType - Norm that is passed in the Krylov convergence
       test routines.

   Level: advanced

   Support for these is highly dependent on the solver.

   Notes:
   This is primarily used to turn off extra norm and function computation
   when the solvers are composed.

.seealso: SNESSolve(), SNESGetConvergedReason(), KSPSetNormType(),
          KSPSetConvergenceTest(), KSPSetPCSide()
E*/

typedef enum {SNES_NORM_DEFAULT            = -1,
              SNES_NORM_NONE               =  0,
              SNES_NORM_FUNCTION           =  1,
              SNES_NORM_INITIAL_ONLY       =  2,
              SNES_NORM_FINAL_ONLY         =  3,
              SNES_NORM_INITIAL_FINAL_ONLY =  4} SNESNormType;
PETSC_EXTERN const char *const*const SNESNormTypes;
/*MC
    SNES_NORM_NONE - Don't compute function and its L2 norm.

   Level: advanced

    Notes:
    This is most useful for stationary solvers with a fixed number of iterations used as smoothers.

.seealso: SNESNormType, SNESSetNormType(), SNES_NORM_DEFAULT
M*/

/*MC
    SNES_NORM_FUNCTION - Compute the function and its L2 norm at each iteration.

   Level: advanced

    Notes:
    Most solvers will use this no matter what norm type is passed to them.

.seealso: SNESNormType, SNESSetNormType(), SNES_NORM_NONE
M*/

/*MC
    SNES_NORM_INITIAL_ONLY - Compute the function and its L2 at iteration 0, but do not update it.

   Level: advanced

   Notes:
   This method is useful in composed methods, when a true solution might actually be found before SNESSolve() is called.
   This option enables the solve to abort on the zeroth iteration if this is the case.

   For solvers that require the computation of the L2 norm of the function as part of the method, this merely cancels
   the norm computation at the last iteration (if possible).

.seealso: SNESNormType, SNESSetNormType(), SNES_NORM_FINAL_ONLY, SNES_NORM_INITIAL_FINAL_ONLY
M*/

/*MC
    SNES_NORM_FINAL_ONLY - Compute the function and its L2 norm on only the final iteration.

   Level: advanced

   Notes:
   For solvers that require the computation of the L2 norm of the function as part of the method, behaves
   exactly as SNES_NORM_DEFAULT.  This method is useful when the function is gotten after SNESSolve and
   used in subsequent computation for methods that do not need the norm computed during the rest of the
   solution procedure.

.seealso: SNESNormType, SNESSetNormType(), SNES_NORM_INITIAL_ONLY, SNES_NORM_INITIAL_FINAL_ONLY
M*/

/*MC
    SNES_NORM_INITIAL_FINAL_ONLY - Compute the function and its L2 norm on only the initial and final iterations.

   Level: advanced

   Notes:
   This method combines the benefits of SNES_NORM_INITIAL_ONLY and SNES_NORM_FINAL_ONLY.

.seealso: SNESNormType, SNESSetNormType(), SNES_NORM_SNES_NORM_INITIAL_ONLY, SNES_NORM_FINAL_ONLY
M*/


PETSC_EXTERN PetscErrorCode SNESSetNormType(SNES,SNESNormType);
PETSC_EXTERN PetscErrorCode SNESGetNormType(SNES,SNESNormType*);

PETSC_EXTERN PetscErrorCode SNESSetGS(SNES,PetscErrorCode (*SNESGSFunction)(SNES,Vec,Vec,void*),void*);
PETSC_EXTERN PetscErrorCode SNESGetGS(SNES,PetscErrorCode (**SNESGSFunction)(SNES,Vec,Vec,void*),void**);
PETSC_EXTERN PetscErrorCode SNESSetUseGS(SNES,PetscBool);
PETSC_EXTERN PetscErrorCode SNESGetUseGS(SNES,PetscBool *);
PETSC_EXTERN PetscErrorCode SNESComputeGS(SNES,Vec,Vec);

PETSC_EXTERN PetscErrorCode SNESGSSetSweeps(SNES,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGSGetSweeps(SNES,PetscInt *);
PETSC_EXTERN PetscErrorCode SNESGSSetTolerances(SNES,PetscReal,PetscReal,PetscReal,PetscInt);
PETSC_EXTERN PetscErrorCode SNESGSGetTolerances(SNES,PetscReal*,PetscReal*,PetscReal*,PetscInt*);

PETSC_EXTERN PetscErrorCode SNESUpdateCheckJacobian(SNES,PetscInt);

PETSC_EXTERN PetscErrorCode SNESShellGetContext(SNES,void**);
PETSC_EXTERN PetscErrorCode SNESShellSetContext(SNES,void*);
PETSC_EXTERN PetscErrorCode SNESShellSetSolve(SNES,PetscErrorCode (*)(SNES,Vec));

/* --------- Routines specifically for line search methods --------------- */

typedef struct _p_LineSearch* SNESLineSearch;

/*S
     SNESLineSearch - Abstract PETSc object that manages line-search operations

   Level: beginner

  Concepts: nonlinear solvers, line search

.seealso:  SNESLineSearchCreate(), SNESLineSearchSetType(), SNES
S*/

/*J
    SNESLineSearchType - String with the name of a PETSc line search method

   Level: beginner

.seealso: SNESLineSearchSetType(), SNES
J*/

typedef const char* SNESLineSearchType;
#define SNESLINESEARCHBT                 "bt"
#define SNESLINESEARCHBASIC              "basic"
#define SNESLINESEARCHL2                 "l2"
#define SNESLINESEARCHCP                 "cp"
#define SNESLINESEARCHSHELL              "shell"

PETSC_EXTERN PetscBool         SNESRegisterAllCalled;
PETSC_EXTERN PetscFunctionList SNESList;
PETSC_EXTERN PetscClassId      SNESLINESEARCH_CLASSID;
PETSC_EXTERN PetscBool         SNESLineSearchRegisterAllCalled;
PETSC_EXTERN PetscFunctionList SNESLineSearchList;
PETSC_EXTERN PetscLogEvent     SNESLineSearch_Apply;

#define SNES_LINESEARCH_ORDER_LINEAR    1
#define SNES_LINESEARCH_ORDER_QUADRATIC 2
#define SNES_LINESEARCH_ORDER_CUBIC     3

PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*SNESLineSearchVIProjectFunc)(SNES,Vec);
PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*SNESLineSearchVINormFunc)(SNES,Vec,Vec,PetscReal *);
PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*SNESLineSearchApplyFunc)(SNESLineSearch);
PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*SNESLineSearchUserFunc)(SNESLineSearch, void *);

PETSC_EXTERN PetscErrorCode SNESLineSearchCreate(MPI_Comm, SNESLineSearch*);
PETSC_EXTERN PetscErrorCode SNESLineSearchReset(SNESLineSearch);
PETSC_EXTERN PetscErrorCode SNESLineSearchView(SNESLineSearch,PetscViewer);
PETSC_EXTERN PetscErrorCode SNESLineSearchDestroy(SNESLineSearch *);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetType(SNESLineSearch, SNESLineSearchType);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetFromOptions(SNESLineSearch);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetUp(SNESLineSearch);
PETSC_EXTERN PetscErrorCode SNESLineSearchApply(SNESLineSearch, Vec, Vec, PetscReal *, Vec);
PETSC_EXTERN PetscErrorCode SNESLineSearchPreCheck(SNESLineSearch,Vec,Vec,PetscBool *);
PETSC_EXTERN PetscErrorCode SNESLineSearchPostCheck(SNESLineSearch,Vec,Vec,Vec,PetscBool *,PetscBool *);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetWorkVecs(SNESLineSearch, PetscInt);

/* set the functions for precheck and postcheck */

PETSC_EXTERN PetscErrorCode SNESLineSearchSetPreCheck(SNESLineSearch, PetscErrorCode (*SNESLineSearchPreCheckFunction)(SNESLineSearch,Vec,Vec,PetscBool*,void*), void *ctx);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetPostCheck(SNESLineSearch, PetscErrorCode (*SNESLineSearchPostCheckFunction)(SNESLineSearch,Vec,Vec,Vec,PetscBool *,PetscBool *,void*), void *ctx);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetPreCheck(SNESLineSearch, PetscErrorCode (**SNESLineSearchPreCheckFunction)(SNESLineSearch,Vec,Vec,PetscBool*,void*), void **ctx);
PETSC_EXTERN PetscErrorCode SNESLineSearchGetPostCheck(SNESLineSearch, PetscErrorCode (**SNESLineSearchPostCheckFunction)(SNESLineSearch,Vec,Vec,Vec,PetscBool *,PetscBool *,void*), void **ctx);

/* set the functions for VI-specific line search operations */

PETSC_EXTERN PetscErrorCode SNESLineSearchSetVIFunctions(SNESLineSearch, SNESLineSearchVIProjectFunc, SNESLineSearchVINormFunc);
PETSC_EXTERN PetscErrorCode SNESLineSearchGetVIFunctions(SNESLineSearch, SNESLineSearchVIProjectFunc*, SNESLineSearchVINormFunc*);

/* pointers to the associated SNES in order to be able to get the function evaluation out */
PETSC_EXTERN PetscErrorCode SNESLineSearchSetSNES(SNESLineSearch,SNES);
PETSC_EXTERN PetscErrorCode SNESLineSearchGetSNES(SNESLineSearch,SNES*);

/* set and get the parameters and vectors */
PETSC_EXTERN PetscErrorCode SNESLineSearchGetTolerances(SNESLineSearch,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscReal*,PetscInt*);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetTolerances(SNESLineSearch,PetscReal,PetscReal,PetscReal,PetscReal,PetscReal,PetscInt);

PETSC_EXTERN PetscErrorCode SNESLineSearchPreCheckPicard(SNESLineSearch,Vec,Vec,PetscBool*,void*);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetLambda(SNESLineSearch,PetscReal*);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetLambda(SNESLineSearch,PetscReal);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetDamping(SNESLineSearch,PetscReal*);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetDamping(SNESLineSearch,PetscReal);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetOrder(SNESLineSearch,PetscInt *order);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetOrder(SNESLineSearch,PetscInt order);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetSuccess(SNESLineSearch, PetscBool*);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetSuccess(SNESLineSearch, PetscBool);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetVecs(SNESLineSearch,Vec*,Vec*,Vec*,Vec*,Vec*);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetVecs(SNESLineSearch,Vec,Vec,Vec,Vec,Vec);

PETSC_EXTERN PetscErrorCode SNESLineSearchGetNorms(SNESLineSearch, PetscReal *, PetscReal *, PetscReal *);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetNorms(SNESLineSearch, PetscReal, PetscReal, PetscReal);
PETSC_EXTERN PetscErrorCode SNESLineSearchComputeNorms(SNESLineSearch);
PETSC_EXTERN PetscErrorCode SNESLineSearchSetComputeNorms(SNESLineSearch, PetscBool);

PETSC_EXTERN PetscErrorCode SNESLineSearchSetMonitor(SNESLineSearch, PetscBool);
PETSC_EXTERN PetscErrorCode SNESLineSearchGetMonitor(SNESLineSearch, PetscViewer*);

PETSC_EXTERN PetscErrorCode SNESLineSearchAppendOptionsPrefix(SNESLineSearch, const char prefix[]);
PETSC_EXTERN PetscErrorCode SNESLineSearchGetOptionsPrefix(SNESLineSearch, const char *prefix[]);


/* Shell interface functions */
PETSC_EXTERN PetscErrorCode SNESLineSearchShellSetUserFunc(SNESLineSearch,SNESLineSearchUserFunc,void*);
PETSC_EXTERN PetscErrorCode SNESLineSearchShellGetUserFunc(SNESLineSearch,SNESLineSearchUserFunc*,void**);

/* BT interface functions */
PETSC_EXTERN PetscErrorCode SNESLineSearchBTSetAlpha(SNESLineSearch, PetscReal);
PETSC_EXTERN PetscErrorCode SNESLineSearchBTGetAlpha(SNESLineSearch, PetscReal*);

/*register line search types */
PETSC_EXTERN PetscErrorCode SNESLineSearchRegister(const char[],PetscErrorCode(*)(SNESLineSearch));
PETSC_EXTERN PetscErrorCode SNESLineSearchRegisterAll(void);

/* Routines for VI solver */
PETSC_EXTERN PetscErrorCode SNESVISetVariableBounds(SNES,Vec,Vec);
PETSC_EXTERN PetscErrorCode SNESVISetComputeVariableBounds(SNES, PetscErrorCode (*)(SNES,Vec,Vec));
PETSC_EXTERN PetscErrorCode SNESVIGetInactiveSet(SNES,IS*);
PETSC_EXTERN PetscErrorCode SNESVIGetActiveSetIS(SNES,Vec,Vec,IS*);
PETSC_EXTERN PetscErrorCode SNESVIComputeInactiveSetFnorm(SNES,Vec,Vec,PetscReal*);
PETSC_EXTERN PetscErrorCode SNESVISetRedundancyCheck(SNES,PetscErrorCode(*)(SNES,IS,IS*,void*),void*);
#define SNES_VI_INF   1.0e20
#define SNES_VI_NINF -1.0e20

PETSC_EXTERN PetscErrorCode SNESTestLocalMin(SNES);

/* Should this routine be private? */
PETSC_EXTERN PetscErrorCode SNESComputeJacobian(SNES,Vec,Mat*,Mat*,MatStructure*);

PETSC_EXTERN PetscErrorCode SNESSetDM(SNES,DM);
PETSC_EXTERN PetscErrorCode SNESGetDM(SNES,DM*);
PETSC_EXTERN PetscErrorCode SNESSetPC(SNES,SNES);
PETSC_EXTERN PetscErrorCode SNESGetPC(SNES,SNES*);
PETSC_EXTERN PetscErrorCode SNESSetPCSide(SNES,PCSide);
PETSC_EXTERN PetscErrorCode SNESGetPCSide(SNES,PCSide*);
PETSC_EXTERN PetscErrorCode SNESSetLineSearch(SNES,SNESLineSearch);
PETSC_EXTERN PetscErrorCode SNESGetLineSearch(SNES,SNESLineSearch*);
PETSC_EXTERN PetscErrorCode SNESRestrictHookAdd(SNES,PetscErrorCode (*)(SNES,SNES,void*),void*);
PETSC_EXTERN PetscErrorCode SNESRestrictHooksRun(SNES,SNES);

PETSC_DEPRECATED("Use SNESGetLineSearch()") PETSC_STATIC_INLINE PetscErrorCode SNESGetSNESLineSearch(SNES snes,SNESLineSearch *ls) {return SNESGetLineSearch(snes,ls);}
PETSC_DEPRECATED("Use SNESSetLineSearch()") PETSC_STATIC_INLINE PetscErrorCode SNESSetSNESLineSearch(SNES snes,SNESLineSearch ls) {return SNESSetLineSearch(snes,ls);}

PETSC_EXTERN PetscErrorCode SNESSetUpMatrices(SNES);
PETSC_EXTERN PetscErrorCode DMSNESSetFunction(DM,PetscErrorCode(*SNESFunction)(SNES,Vec,Vec,void*),void*);
PETSC_EXTERN PetscErrorCode DMSNESGetFunction(DM,PetscErrorCode(**SNESSetFunction)(SNES,Vec,Vec,void*),void**);
PETSC_EXTERN PetscErrorCode DMSNESSetGS(DM,PetscErrorCode(*SNESGSFunction)(SNES,Vec,Vec,void*),void*);
PETSC_EXTERN PetscErrorCode DMSNESGetGS(DM,PetscErrorCode(**SNESGSFunction)(SNES,Vec,Vec,void*),void**);
PETSC_EXTERN PetscErrorCode DMSNESSetJacobian(DM,PetscErrorCode(*SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void*);
PETSC_EXTERN PetscErrorCode DMSNESGetJacobian(DM,PetscErrorCode(**SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void**);
PETSC_EXTERN PetscErrorCode DMSNESSetPicard(DM,PetscErrorCode(*SNESFunction)(SNES,Vec,Vec,void*),PetscErrorCode(*SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void*);
PETSC_EXTERN PetscErrorCode DMSNESGetPicard(DM,PetscErrorCode(**SNESFunction)(SNES,Vec,Vec,void*),PetscErrorCode(**SNESJacobianFunction)(SNES,Vec,Mat*,Mat*,MatStructure*,void*),void**);
PETSC_EXTERN PetscErrorCode DMSNESSetObjective(DM,PetscErrorCode (*SNESObjectiveFunction)(SNES,Vec,PetscReal *,void*),void*);
PETSC_EXTERN PetscErrorCode DMSNESGetObjective(DM,PetscErrorCode (**SNESObjectiveFunction)(SNES,Vec,PetscReal *,void*),void**);

PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*DMDASNESFunction)(DMDALocalInfo*,void*,void*,void*);
PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*DMDASNESJacobian)(DMDALocalInfo*,void*,Mat,Mat,MatStructure*,void*);
PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*DMDASNESObjective)(DMDALocalInfo*,void*,PetscReal*,void*);

PETSC_EXTERN PetscErrorCode DMDASNESSetFunctionLocal(DM,InsertMode,DMDASNESFunction,void*);
PETSC_EXTERN PetscErrorCode DMDASNESSetJacobianLocal(DM,DMDASNESJacobian,void*);
PETSC_EXTERN PetscErrorCode DMDASNESSetObjectiveLocal(DM,DMDASNESObjective,void*);
PETSC_EXTERN PetscErrorCode DMDASNESSetPicardLocal(DM,InsertMode,PetscErrorCode (*)(DMDALocalInfo*,void*,void*,void*),PetscErrorCode (*)(DMDALocalInfo*,void*,Mat,Mat,MatStructure*,void*),void*);

PETSC_EXTERN PetscErrorCode DMSNESSetFunctionLocal(DM,PetscErrorCode (*)(DM,Vec,Vec,void*),void*);
PETSC_EXTERN PetscErrorCode DMSNESSetJacobianLocal(DM,PetscErrorCode (*)(DM,Vec,Mat,Mat,MatStructure*,void*),void*);

/* Routines for Multiblock solver */
PETSC_EXTERN PetscErrorCode SNESMultiblockSetFields(SNES, const char [], PetscInt, const PetscInt *);
PETSC_EXTERN PetscErrorCode SNESMultiblockSetIS(SNES, const char [], IS);
PETSC_EXTERN PetscErrorCode SNESMultiblockSetBlockSize(SNES, PetscInt);
PETSC_EXTERN PetscErrorCode SNESMultiblockSetType(SNES, PCCompositeType);

/*J
    SNESMSType - String with the name of a PETSc SNESMS method.

   Level: intermediate

.seealso: SNESMSSetType(), SNES
J*/
typedef const char* SNESMSType;
#define SNESMSM62       "m62"
#define SNESMSEULER     "euler"
#define SNESMSJAMESON83 "jameson83"
#define SNESMSVLTP21    "vltp21"
#define SNESMSVLTP31    "vltp31"
#define SNESMSVLTP41    "vltp41"
#define SNESMSVLTP51    "vltp51"
#define SNESMSVLTP61    "vltp61"

PETSC_EXTERN PetscErrorCode SNESMSRegister(SNESMSType,PetscInt,PetscInt,PetscReal,const PetscReal[],const PetscReal[],const PetscReal[]);
PETSC_EXTERN PetscErrorCode SNESMSSetType(SNES,SNESMSType);
PETSC_EXTERN PetscErrorCode SNESMSFinalizePackage(void);
PETSC_EXTERN PetscErrorCode SNESMSInitializePackage(void);
PETSC_EXTERN PetscErrorCode SNESMSRegisterDestroy(void);
PETSC_EXTERN PetscErrorCode SNESMSRegisterAll(void);

/* routines for NGMRES solver */

typedef enum {
  SNES_NGMRES_RESTART_NONE       = 0,
  SNES_NGMRES_RESTART_PERIODIC   = 1,
  SNES_NGMRES_RESTART_DIFFERENCE = 2} SNESNGMRESRestartType;
PETSC_EXTERN const char *const SNESNGMRESRestartTypes[];

typedef enum {
  SNES_NGMRES_SELECT_NONE       = 0,
  SNES_NGMRES_SELECT_DIFFERENCE = 1,
  SNES_NGMRES_SELECT_LINESEARCH = 2} SNESNGMRESSelectType;
PETSC_EXTERN const char *const SNESNGMRESSelectTypes[];

PETSC_EXTERN PetscErrorCode SNESNGMRESSetRestartType(SNES, SNESNGMRESRestartType);
PETSC_EXTERN PetscErrorCode SNESNGMRESSetSelectType(SNES, SNESNGMRESSelectType);

/* routines for NCG solver */

typedef enum {
  SNES_NCG_FR    = 0,
  SNES_NCG_PRP   = 1,
  SNES_NCG_HS    = 2,
  SNES_NCG_DY    = 3,
  SNES_NCG_CD    = 4} SNESNCGType;
PETSC_EXTERN const char *const SNESNCGTypes[];

PETSC_EXTERN PetscErrorCode SNESNCGSetType(SNES, SNESNCGType);

typedef enum {SNES_QN_SCALE_NONE       = 0,
              SNES_QN_SCALE_SHANNO     = 1,
              SNES_QN_SCALE_LINESEARCH = 2,
              SNES_QN_SCALE_JACOBIAN   = 3} SNESQNScaleType;
PETSC_EXTERN const char *const SNESQNScaleTypes[];
typedef enum {SNES_QN_RESTART_NONE     = 0,
              SNES_QN_RESTART_POWELL   = 1,
              SNES_QN_RESTART_PERIODIC = 2} SNESQNRestartType;
PETSC_EXTERN const char *const SNESQNRestartTypes[];

PETSC_EXTERN PetscErrorCode SNESQNSetScaleType(SNES, SNESQNScaleType);
PETSC_EXTERN PetscErrorCode SNESQNSetRestartType(SNES, SNESQNRestartType);

PETSC_EXTERN PetscErrorCode SNESNASMGetSubdomains(SNES,PetscInt*,SNES**,VecScatter**,VecScatter**,VecScatter**);
PETSC_EXTERN PetscErrorCode SNESNASMSetSubdomains(SNES,PetscInt,SNES*,VecScatter*,VecScatter*,VecScatter*);
PETSC_EXTERN PetscErrorCode SNESNASMSetDamping(SNES,PetscReal);
PETSC_EXTERN PetscErrorCode SNESNASMGetDamping(SNES,PetscReal*);
PETSC_EXTERN PetscErrorCode SNESNASMGetSubdomainVecs(SNES,PetscInt*,Vec**,Vec**,Vec**,Vec**);
PETSC_EXTERN PetscErrorCode SNESNASMSetComputeFinalJacobian(SNES,PetscBool);

#endif
