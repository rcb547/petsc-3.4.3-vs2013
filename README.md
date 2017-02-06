# Precompiled PETSc 3.4.3 Windows 64 bit libraries

## Source code 

The source code was developed by the PETSc project based at Argonne National Laboratory. See https://www.mcs.anl.gov/petsc for details. The repository does not include the source code, it only includes the libraries, header and config files.

## Compiler 

These libraries were compiled with Microsoft Visual Studio 2013 Express.  The debug version of the library has not been add to the repository because it is too large for GitHUB at 401 Mb.

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


