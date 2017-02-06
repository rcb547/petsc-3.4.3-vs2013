
#ifndef __SNESIMPL_H
#define __SNESIMPL_H

#include <petscsnes.h>
#include <petsc-private/petscimpl.h>

typedef struct _SNESOps *SNESOps;

struct _SNESOps {
  PetscErrorCode (*computeinitialguess)(SNES,Vec,void*);
  PetscErrorCode (*computescaling)(Vec,Vec,void*);
  PetscErrorCode (*update)(SNES, PetscInt);                     /* General purpose function for update */
  PetscErrorCode (*converged)(SNES,PetscInt,PetscReal,PetscReal,PetscReal,SNESConvergedReason*,void*);
  PetscErrorCode (*convergeddestroy)(void*);
  PetscErrorCode (*setup)(SNES);                                /* routine to set up the nonlinear solver */
  PetscErrorCode (*solve)(SNES);                                /* actual nonlinear solver */
  PetscErrorCode (*view)(SNES,PetscViewer);
  PetscErrorCode (*setfromoptions)(SNES);                       /* sets options from database */
  PetscErrorCode (*destroy)(SNES);
  PetscErrorCode (*reset)(SNES);
  PetscErrorCode (*usercompute)(SNES,void**);
  PetscErrorCode (*userdestroy)(void**);
  PetscErrorCode (*computevariablebounds)(SNES,Vec,Vec);        /* user provided routine to set box constrained variable bounds */
  PetscErrorCode (*computepfunction)(SNES,Vec,Vec,void*);
  PetscErrorCode (*computepjacobian)(SNES,Vec,Mat*,Mat*,MatStructure*,void*);
  PetscErrorCode (*load)(SNES,PetscViewer);
};

/*
   Nonlinear solver context
 */
#define MAXSNESMONITORS 5

struct _p_SNES {
  PETSCHEADER(struct _SNESOps);
  DM        dm;
  PetscBool dmAuto;             /* SNES created currently used DM automatically */
  SNES      pc;
  PCSide    pcside;
  PetscBool usespc;

  /*  ------------------------ User-provided stuff -------------------------------*/
  void  *user;                   /* user-defined context */

  Vec  vec_rhs;                  /* If non-null, solve F(x) = rhs */
  Vec  vec_sol;                  /* pointer to solution */

  Vec  vec_func;                 /* pointer to function */

  Mat  jacobian;                 /* Jacobian matrix */
  Mat  jacobian_pre;             /* preconditioner matrix */
  void *initialguessP;           /* user-defined initial guess context */
  KSP  ksp;                      /* linear solver context */
  SNESLineSearch linesearch;     /* line search context */
  PetscBool usesksp;
  MatStructure matstruct;        /* Used by Picard solver */

  Vec  vec_sol_update;           /* pointer to solution update */

  Vec  scaling;                  /* scaling vector */
  void *scaP;                    /* scaling context */

  PetscReal precheck_picard_angle; /* For use with SNESLineSearchPreCheckPicard */

  /* ------------------------Time stepping hooks-----------------------------------*/

  /* ---------------- PETSc-provided (or user-provided) stuff ---------------------*/

  PetscErrorCode      (*monitor[MAXSNESMONITORS])(SNES,PetscInt,PetscReal,void*); /* monitor routine */
  PetscErrorCode      (*monitordestroy[MAXSNESMONITORS])(void**);                 /* monitor context destroy routine */
  void                *monitorcontext[MAXSNESMONITORS];                           /* monitor context */
  PetscInt            numbermonitors;                                             /* number of monitors */
  void                *cnvP;                                                      /* convergence context */
  SNESConvergedReason reason;
  PetscBool           errorifnotconverged;

  /* --- Routines and data that are unique to each particular solver --- */

  PetscBool      setupcalled;                /* true if setup has been called */
  void           *data;                      /* implementation-specific data */

  /* --------------------------  Parameters -------------------------------------- */

  PetscInt    max_its;            /* max number of iterations */
  PetscInt    max_funcs;          /* max number of function evals */
  PetscInt    nfuncs;             /* number of function evaluations */
  PetscInt    iter;               /* global iteration number */
  PetscInt    linear_its;         /* total number of linear solver iterations */
  PetscReal   norm;               /* residual norm of current iterate */
  PetscReal   rtol;               /* relative tolerance */
  PetscReal   abstol;             /* absolute tolerance */
  PetscReal   stol;               /* step length tolerance*/
  PetscReal   deltatol;           /* trust region convergence tolerance */
  PetscBool   printreason;        /* print reason for convergence/divergence after each solve */
  PetscInt    lagpreconditioner;  /* SNESSetLagPreconditioner() */
  PetscInt    lagjacobian;        /* SNESSetLagJacobian() */
  PetscInt    gridsequence;       /* number of grid sequence steps to take; defaults to zero */

  PetscBool   tolerancesset;      /* SNESSetTolerances() called and tolerances should persist through SNESCreate_XXX()*/

  PetscReal   norm_init;          /* the initial norm value */
  PetscBool   norm_init_set;      /* the initial norm has been set */
  PetscBool   vec_func_init_set;  /* the initial function has been set */

  SNESNormType normtype;          /* Norm computation type for SNES instance */

  /* ------------------------ Default work-area management ---------------------- */

  PetscInt    nwork;
  Vec         *work;

  /* ------------------------- Miscellaneous Information ------------------------ */

  PetscReal   *conv_hist;         /* If !0, stores function norm (or
                                    gradient norm) at each iteration */
  PetscInt    *conv_hist_its;     /* linear iterations for each Newton step */
  PetscInt    conv_hist_len;      /* size of convergence history array */
  PetscInt    conv_hist_max;      /* actual amount of data in conv_history */
  PetscBool   conv_hist_reset;    /* reset counter for each new SNES solve */
  PetscBool   conv_malloc;

  /* the next two are used for failures in the line search; they should be put elsewhere */
  PetscInt    numFailures;        /* number of unsuccessful step attempts */
  PetscInt    maxFailures;        /* maximum number of unsuccessful step attempts */

  PetscInt    numLinearSolveFailures;
  PetscInt    maxLinearSolveFailures;

  PetscBool   domainerror;       /* set with SNESSetFunctionDomainError() */

  PetscBool   ksp_ewconv;        /* flag indicating use of Eisenstat-Walker KSP convergence criteria */
  void        *kspconvctx;       /* Eisenstat-Walker KSP convergence context */

  PetscReal   ttol;           /* used by default convergence test routine */

  Vec         *vwork;            /* more work vectors for Jacobian approx */
  PetscInt    nvwork;

  PetscBool   mf;               /* -snes_mf was used on this snes */
  PetscBool   mf_operator;      /* -snes_mf_operator was used on this snes */
  PetscInt    mf_version;       /* The version of snes_mf used */

  Vec         xl,xu;             /* upper and lower bounds for box constrained VI problems */
  PetscInt    ntruebounds;       /* number of non-infinite bounds set for VI box constraints */
  PetscBool   usersetbounds;     /* bounds have been set via SNESVISetVariableBounds(), rather than via computevariablebounds() callback. */
};

typedef struct _p_DMSNES *DMSNES;
typedef struct _DMSNESOps *DMSNESOps;
struct _DMSNESOps {
  PetscErrorCode (*computefunction)(SNES,Vec,Vec,void*);
  PetscErrorCode (*computejacobian)(SNES,Vec,Mat*,Mat*,MatStructure*,void*);

  /* objective */
  PetscErrorCode (*computeobjective)(SNES,Vec,PetscReal*,void*);

  /* Picard iteration functions */
  PetscErrorCode (*computepfunction)(SNES,Vec,Vec,void*);
  PetscErrorCode (*computepjacobian)(SNES,Vec,Mat*,Mat*,MatStructure*,void*);

  /* User-defined smoother */
  PetscErrorCode (*computegs)(SNES,Vec,Vec,void*);

  PetscErrorCode (*destroy)(DMSNES);
  PetscErrorCode (*duplicate)(DMSNES,DMSNES);
};

struct _p_DMSNES {
  PETSCHEADER(struct _DMSNESOps);
  void *functionctx;
  void *gsctx;
  void *pctx;
  void *jacobianctx;
  void *objectivectx;

  void *data;

  /* This is NOT reference counted. The DM on which this context was first created is cached here to implement one-way
   * copy-on-write. When DMGetDMSNESWrite() sees a request using a different DM, it makes a copy. Thus, if a user
   * only interacts directly with one level, e.g., using SNESSetFunction(), then SNESSetUp_FAS() is called to build
   * coarse levels, then the user changes the routine with another call to SNESSetFunction(), it automatically
   * propagates to all the levels. If instead, they get out a specific level and set the function on that level,
   * subsequent changes to the original level will no longer propagate to that level.
   */
  DM originaldm;
};
PETSC_EXTERN PetscErrorCode DMGetDMSNES(DM,DMSNES*);
PETSC_EXTERN PetscErrorCode DMSNESView(DMSNES,PetscViewer);
PETSC_EXTERN PetscErrorCode DMSNESLoad(DMSNES,PetscViewer);
PETSC_EXTERN PetscErrorCode DMGetDMSNESWrite(DM,DMSNES*);
PETSC_EXTERN PetscErrorCode DMCopyDMSNES(DM,DM);


/* Context for Eisenstat-Walker convergence criteria for KSP solvers */
typedef struct {
  PetscInt  version;             /* flag indicating version 1 or 2 of test */
  PetscReal rtol_0;              /* initial rtol */
  PetscReal rtol_last;           /* last rtol */
  PetscReal rtol_max;            /* maximum rtol */
  PetscReal gamma;               /* mult. factor for version 2 rtol computation */
  PetscReal alpha;               /* power for version 2 rtol computation */
  PetscReal alpha2;              /* power for safeguard */
  PetscReal threshold;           /* threshold for imposing safeguard */
  PetscReal lresid_last;         /* linear residual from last iteration */
  PetscReal norm_last;           /* function norm from last iteration */
} SNESKSPEW;

#undef __FUNCT__
#define __FUNCT__ "SNESLogConvergenceHistory"
PETSC_STATIC_INLINE PetscErrorCode SNESLogConvergenceHistory(SNES snes,PetscReal res,PetscInt its)
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscObjectAMSTakeAccess((PetscObject)snes);CHKERRQ(ierr);
  if (snes->conv_hist && snes->conv_hist_max > snes->conv_hist_len) {
    if (snes->conv_hist)     snes->conv_hist[snes->conv_hist_len]     = res;
    if (snes->conv_hist_its) snes->conv_hist_its[snes->conv_hist_len] = its;
    snes->conv_hist_len++;
  }
  ierr = PetscObjectAMSGrantAccess((PetscObject)snes);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

PETSC_EXTERN PetscErrorCode SNESVIProjectOntoBounds(SNES,Vec);
PETSC_INTERN PetscErrorCode SNESVICheckLocalMin_Private(SNES,Mat,Vec,Vec,PetscReal,PetscBool*);
PETSC_INTERN PetscErrorCode SNESReset_VI(SNES);
PETSC_INTERN PetscErrorCode SNESDestroy_VI(SNES);
PETSC_INTERN PetscErrorCode SNESView_VI(SNES,PetscViewer);
PETSC_INTERN PetscErrorCode SNESSetFromOptions_VI(SNES);
PETSC_INTERN PetscErrorCode SNESSetUp_VI(SNES);
PETSC_EXTERN_TYPEDEF typedef PetscErrorCode (*SNESVIComputeVariableBoundsFunction)(SNES,Vec,Vec);
PETSC_INTERN PetscErrorCode SNESVISetComputeVariableBounds_VI(SNES,SNESVIComputeVariableBoundsFunction);
PETSC_INTERN PetscErrorCode SNESVISetVariableBounds_VI(SNES,Vec,Vec);
PETSC_INTERN PetscErrorCode SNESConvergedDefault_VI(SNES,PetscInt,PetscReal,PetscReal,PetscReal,SNESConvergedReason*,void*);

PetscErrorCode SNESScaleStep_Private(SNES,Vec,PetscReal*,PetscReal*,PetscReal*,PetscReal*);

PETSC_EXTERN PetscLogEvent SNES_Solve, SNES_LineSearch, SNES_FunctionEval, SNES_JacobianEval, SNES_GSEval, SNES_NPCSolve;

#endif
