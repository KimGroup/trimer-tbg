#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=4

# srun --exclusive -n 1 ./a.out 48 inf 0.02 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.03 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.04 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.05 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.07 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.09 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.11 20000000 5 etmd 4000 rt3trans &
# srun --exclusive -n 1 ./a.out 48 inf 0.13 20000000 5 etmd 4000 rt3trans &

srun --exclusive -n 1 ./a.out 60 inf -4 40000000 5 etm 4000 bwtrans &
srun --exclusive -n 1 ./a.out 60 inf -5 40000000 5 etm 4000 bwtrans &
srun --exclusive -n 1 ./a.out 60 inf -6 40000000 5 etm 4000 bwtrans &
srun --exclusive -n 1 ./a.out 60 inf -7 40000000 5 etm 4000 bwtrans &
wait
