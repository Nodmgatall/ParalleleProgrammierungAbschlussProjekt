#!/bin/bash

# specify the number of nodes
#SBATCH -N 4

# specify the total number of processes
#SBATCH -n 16

# specify how exactly the tasks shall be divided
#SBATCH --ntasks-per-node 4

# specify a file to log stdout output
#SBATCH -o simulatio.out

# run the executable using mpirun
mpirun Simulation.x

# write 'done' into a file to signal that the job has finished
echo done > job_script.out
