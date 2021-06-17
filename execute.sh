#!/bin/bash

# pthread

### compilation using gcc
# gcc -pthread -o GoL gameOfLife_pthread.c

# ### execution
# ./GoL 2048 2048 1
# ./GoL 512 512 4
# ./GoL 1024 1024 4
# ./GoL 2048 2048 4
# ./GoL 1024 127 1
# ./GoL 1024 127 2
# ./GoL 1024 127 3
# ./GoL 1024 127 4
# ./GoL 1024 127 5
# ./GoL 1024 127 6

# # openmp

# ### compilation using gcc
gcc -fopenmp -o GoL gameOfLife_openmp.c

# # ### execution
./GoL 2048 2048 4
# ./GoL 1024 127 1
# ./GoL 1024 127 2
# ./GoL 1024 127 3
# ./GoL 1024 127 4
# ./GoL 1024 127 5
# ./GoL 1024 127 6

# # openmpi

# ### compilation using mpicc
# mpicc -o GoL gameOfLife_mpi.c

# ### execution
# mpirun -np 1 GoL 1024 127
# mpirun -np 2 GoL 1024 127
# mpirun -np 3 GoL 1024 127
# mpirun -np 4 GoL 1024 127

# # hybrid

# ### compilation using mpicc
# mpicc -fopenmp -o GoL gameOfLife_hybrid.c

# ### execution
# mpirun -np 1 GoL 1024 127 1
# mpirun -np 1 GoL 1024 127 2
# mpirun -np 1 GoL 1024 127 3
# mpirun -np 1 GoL 1024 127 4
# mpirun -np 2 GoL 1024 127 1
# mpirun -np 2 GoL 1024 127 2
# mpirun -np 2 GoL 1024 127 3
# mpirun -np 2 GoL 1024 127 4
# mpirun -np 3 GoL 1024 127 1
# mpirun -np 3 GoL 1024 127 2
# mpirun -np 3 GoL 1024 127 3
# mpirun -np 4 GoL 1024 127 1
# mpirun -np 4 GoL 1024 127 2