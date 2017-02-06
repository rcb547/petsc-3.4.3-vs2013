#if !defined(__PETSCVERSION_H)
#define __PETSCVERSION_H

#define PETSC_VERSION_RELEASE    1
#define PETSC_VERSION_MAJOR      3
#define PETSC_VERSION_MINOR      4
#define PETSC_VERSION_SUBMINOR   3
#define PETSC_VERSION_PATCH      0
#define PETSC_RELEASE_DATE       "May, 13, 2013"
#define PETSC_VERSION_DATE       "Oct, 15, 2013"

#if !defined (PETSC_VERSION_GIT)
#define PETSC_VERSION_GIT        "e9715d0b55579104fb4321f3f4f3f0e2ce2d7d6d"
#endif

#if !defined(PETSC_VERSION_DATE_GIT)
#define PETSC_VERSION_DATE_GIT   "2013-10-15 11:50:31 -0500"
#endif

#define PETSC_VERSION_(MAJOR,MINOR,SUBMINOR) \
  ((PETSC_VERSION_MAJOR == (MAJOR)) &&       \
   (PETSC_VERSION_MINOR == (MINOR)) &&       \
   (PETSC_VERSION_SUBMINOR == (SUBMINOR)) && \
   (PETSC_VERSION_RELEASE  == 1))

#define PETSC_VERSION_LT(MAJOR,MINOR,SUBMINOR)          \
  (PETSC_VERSION_RELEASE == 1 &&                        \
   (PETSC_VERSION_MAJOR < (MAJOR) ||                    \
    (PETSC_VERSION_MAJOR == (MAJOR) &&                  \
     (PETSC_VERSION_MINOR < (MINOR) ||                  \
      (PETSC_VERSION_MINOR == (MINOR) &&                \
       (PETSC_VERSION_SUBMINOR < (SUBMINOR)))))))

#define PETSC_VERSION_LE(MAJOR,MINOR,SUBMINOR) \
  (PETSC_VERSION_LT(MAJOR,MINOR,SUBMINOR) || \
   PETSC_VERSION_(MAJOR,MINOR,SUBMINOR))

#define PETSC_VERSION_GT(MAJOR,MINOR,SUBMINOR) \
  (!PETSC_VERSION_LE(MAJOR,MINOR,SUBMINOR))

#define PETSC_VERSION_GE(MAJOR,MINOR,SUBMINOR) \
  (!PETSC_VERSION_LT(MAJOR,MINOR,SUBMINOR))

#endif