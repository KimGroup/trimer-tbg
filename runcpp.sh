#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=11

srun --exclusive -n 1 ./a.out 48 16 0.01 0.0320 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0322 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0324 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0326 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0328 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0330 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0332 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0334 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0336 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0338 5000000 5 etc 2000 temp &
srun --exclusive -n 1 ./a.out 48 16 0.01 0.0340 5000000 5 etc 2000 temp &

wait
