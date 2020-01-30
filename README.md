# About #

*FastPCA* is a PCA-calculator programmed in C++11 parallelized with OpenMP.

The *FastPCA* package is an implementation of the principal component analysis of large MD data sets, using either Cartesian atom coordinates, interatom distances or backbone dihedral angles as input coordinates. In particular, it features the dihedral angle PCA on a torus (dPCA+) by Sittel et al., 2017, which performs maximal gap shifting to treat periodic data correctly. It is optimized and parallelized with constant memory consumption for large data sets.

For fast matrix diagonalization, LAPACK is used (and needed, of course).

The project includes the 'xdrfile' library of GROMACS. Thus, you can
use data files written as ASCII data as well as .xtc-trajectories.

For bug-reports just open an issue.

Happy Computing.

# Licensing #
The code is published "AS IS" under the simplified BSD license.
For details, please see LICENSE.txt

If you use the code for published works, please cite as

- F. Sittel et al., *Principal component analysis on a torus: Theory and application to protein dynamics*, J. Chem. Phys 147, 244101, 2017; DOI:10.1063/1.4998259


# Compilation #

Create a build-directory in the project root and change into
that directory:
```
    # mkdir build
    # cd build
```
Run cmake, based on the underlying project:
```
    # cmake ..
```
Hopefully, everything went right.
If not, carefully read the error messages.
Typical errors are missing dependencies...

If everything is o.k., run make (on multicore machines, use '-j' to parallelize
compilation, e.g. 'make -j 4' for up to four parallel jobs):
```
    # make
```
Now, you should find the 'fastca' binary in the 'src' folder.

## Requirements ##
  * LAPACK
  * Boost (program_options), min. version 1.49
  * cmake, min. version 2.8
  * g++

