#!/bin/bash
#SBATCH --job-name="myHELLO"
#SBATCH --partition=debug
#SBATCH --nodes=10
#SBATCH --time=0-00:02:00
#SBATCH --ntasks-per-node=1
#SBATCH --mem=1992

# mpirun -np 4 a.out input.txt

mpiexec -n 1 a.out input.txt
mpiexec -n 2 a.out input.txt
mpiexec -n 3 a.out input.txt
mpiexec -n 4 a.out input.txt
mpiexec -n 5 a.out input.txt
mpiexec -n 6 a.out input.txt
mpiexec -n 7 a.out input.txt
mpiexec -n 8 a.out input.txt