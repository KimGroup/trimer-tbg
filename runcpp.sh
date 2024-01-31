#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=12

# srun --exclusive -n 1 ./a.out 60 inf 0.01 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.02 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.03 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.04 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.05 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.06 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.07 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.08 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.09 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.1 40000000 5 etmd 5000 rt3trans2 &
# srun --exclusive -n 1 ./a.out 60 inf 0.15 40000000 5 etmd 5000 rt3trans2 &

# srun --exclusive -n 1 ./a.out 60 inf -2 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -2.2 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -2.4 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -2.6 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -2.8 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -3 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -3.2 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -3.4 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -3.6 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -3.8 40000000 5 etm 5000 bwtrans2 &
# srun --exclusive -n 1 ./a.out 60 inf -4 40000000 5 etm 5000 bwtrans2 &

# srun --exclusive -n 1 ./a.out 6 1 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 2 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 2.5 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 3 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 3.25 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 3.5 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 3.75 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 4 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 4.25 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 4.5 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 4.75 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 5 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 6 0 40000000 5 etc 5000 utrans &
# srun --exclusive -n 1 ./a.out 6 8 0 40000000 5 etc 5000 utrans &

srun --exclusive -n 1 ./a.out 60 0 1 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 3 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 3.5 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 4 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 4.5 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 5 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 5.5 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 6 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 6.5 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 7 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 8 0 40000000 5 etc 5000 utrans &
srun --exclusive -n 1 ./a.out 60 0 10 0 40000000 5 etc 5000 utrans &

wait
