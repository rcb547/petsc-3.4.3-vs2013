# Precompiled PETSc 3.4.3 Windows 64 bit libraries

## Source code 

The source code was developed by the PETSc project based at Argonne National Laboratory. See https://www.mcs.anl.gov/petsc for details. The repository does not include the source code, it only includes the libraries, header and config files.

## Compiler 

These libraries were compiled with Microsoft Visual Studio 2013 Express under cygwin.  The debug version of the library has not been add to the repository because it is too large for GitHUB at 401 Mb.

## Include files 

For release builds you should have these directories in your include path  
	- ...\include\  
	- ...\win64\include\  

## Library files 

For release builds you should have this directory in your library path  
	- ...\win64\lib\  

Then link with these libraries  
	- libf2clapack.lib (the non-optimised LAPACK library)  
	- libf2cblas.lib (the non-optimised BLAS library)  
	- libpetsc.lib (the PETSc library)  

## MPI flavour  
	The libraries were compiled using the Microsoft_HPC_Pack_2012 MPI library.  Therefore you need to install Microsoft_HPC_Pack_2012 and link to ...\Microsoft_HPC_Pack_2012\Lib\amd64\msmpi.lib.

## Cygwin build script

For anyone who cares, here is the cygwin build script.

```bash  

#!/bin/bash

#Run "vcvarsall.bat x86_amd64" from the VS2013 command prompt to set up the command line compile environment for windows 64 bit builds.  Then launch cygwin from that command prompt window.

export PETSC_DIR=/cygdrive/z/code/third_party/petsc-3.4.3

./configure PETSC_ARCH=win64_vs2013 \
  --with-cc='win32fe cl' \
  --with-cxx='win32fe cl' \
  --with-clanguage=C++ \
  --with-fc=0 \
  --download-f2cblaslapack=1 \
  --with-mpi-include=/cygdrive/c/Microsoft_HPC_Pack_2012/Inc \
  --with-mpi-lib=/cygdrive/c/Microsoft_HPC_Pack_2012/Lib/amd64/msmpi.lib \
  --with-mpiexec=/cygdrive/c/Microsoft_HPC_Pack_2012/Bin/mpiexec.exe \
  --with-64-bit-indices=1 \
  --with-single-library=1 \
  --with-endian=little \
  --with-debugging=0 \
  --with-x=0 \
  --with-windows-graphics=0

make PETSC_DIR=/cygdrive/z/code/third_party/petsc-3.4.3 PETSC_ARCH=win64_vs2013 all

#In Visual Studio must link with the Multi-Thread static runtime library /MT option see Configuration Properties | C/C++ | Code Generation | Runtime Libraries | Multi-thread (/MT)

```

