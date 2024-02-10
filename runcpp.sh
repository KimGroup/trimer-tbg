#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=24

# srun --exclusive -n 1 ./pocket 12 1 -0.3 0.06 20000000 20 etcmd 10000 finite-t/j-0.3t0.06 &
# srun --exclusive -n 1 ./pocket 24 1 -0.3 0.06 20000000 20 etcmd 10000 finite-t/j-0.3t0.06 &
# srun --exclusive -n 1 ./pocket 36 1 -0.3 0.06 20000000 20 etcmd 10000 finite-t/j-0.3t0.06 &
# srun --exclusive -n 1 ./pocket 48 1 -0.3 0.06 20000000 20 etcmd 10000 finite-t/j-0.3t0.06 &
# srun --exclusive -n 1 ./pocket 60 1 -0.3 0.06 20000000 20 etcmd 10000 finite-t/j-0.3t0.06 &
# srun --exclusive -n 1 ./pocket 12 1 -0.3 0.08 20000000 20 etcmd 10000 finite-t/j-0.3t0.08 &
# srun --exclusive -n 1 ./pocket 24 1 -0.3 0.08 20000000 20 etcmd 10000 finite-t/j-0.3t0.08 &
# srun --exclusive -n 1 ./pocket 36 1 -0.3 0.08 20000000 20 etcmd 10000 finite-t/j-0.3t0.08 &
# srun --exclusive -n 1 ./pocket 48 1 -0.3 0.08 20000000 20 etcmd 10000 finite-t/j-0.3t0.08 &
# srun --exclusive -n 1 ./pocket 60 1 -0.3 0.08 20000000 20 etcmd 10000 finite-t/j-0.3t0.08 &
# srun --exclusive -n 1 ./pocket 12 1 0.05 0.1 20000000 20 etcmd 10000 finite-t/j0.05t0.1 &
# srun --exclusive -n 1 ./pocket 24 1 0.05 0.1 20000000 20 etcmd 10000 finite-t/j0.05t0.1 &
# srun --exclusive -n 1 ./pocket 36 1 0.05 0.1 20000000 20 etcmd 10000 finite-t/j0.05t0.1 &
# srun --exclusive -n 1 ./pocket 48 1 0.05 0.1 20000000 20 etcmd 10000 finite-t/j0.05t0.1 &
# srun --exclusive -n 1 ./pocket 60 1 0.05 0.1 20000000 20 etcmd 10000 finite-t/j0.05t0.1 &
# srun --exclusive -n 1 ./pocket 12 1 0.05 0.15 20000000 20 etcmd 10000 finite-t/j0.05t0.15 &
# srun --exclusive -n 1 ./pocket 24 1 0.05 0.15 20000000 20 etcmd 10000 finite-t/j0.05t0.15 &
# srun --exclusive -n 1 ./pocket 36 1 0.05 0.15 20000000 20 etcmd 10000 finite-t/j0.05t0.15 &
# srun --exclusive -n 1 ./pocket 48 1 0.05 0.15 20000000 20 etcmd 10000 finite-t/j0.05t0.15 &
# srun --exclusive -n 1 ./pocket 60 1 0.05 0.15 20000000 20 etcmd 10000 finite-t/j0.05t0.15 &
# srun --exclusive -n 1 ./pocket 12 1 0.0 0.1 20000000 20 etcmd 10000 finite-t/j0t0.1 &
# srun --exclusive -n 1 ./pocket 24 1 0.0 0.1 20000000 20 etcmd 10000 finite-t/j0t0.1 &
# srun --exclusive -n 1 ./pocket 36 1 0.0 0.1 20000000 20 etcmd 10000 finite-t/j0t0.1 &
# srun --exclusive -n 1 ./pocket 48 1 0.0 0.1 20000000 20 etcmd 10000 finite-t/j0t0.1 &
# srun --exclusive -n 1 ./pocket 60 1 0.0 0.1 20000000 20 etcmd 10000 finite-t/j0t0.1 &
# srun --exclusive -n 1 ./pocket 12 1 0.0 0.15 20000000 20 etcmd 10000 finite-t/j0t0.15 &
# srun --exclusive -n 1 ./pocket 24 1 0.0 0.15 20000000 20 etcmd 10000 finite-t/j0t0.15 &
# srun --exclusive -n 1 ./pocket 36 1 0.0 0.15 20000000 20 etcmd 10000 finite-t/j0t0.15 &
# srun --exclusive -n 1 ./pocket 48 1 0.0 0.15 20000000 20 etcmd 10000 finite-t/j0t0.15 &
# srun --exclusive -n 1 ./pocket 60 1 0.0 0.15 20000000 20 etcmd 10000 finite-t/j0t0.15 &

srun --exclusive -n 1 ./pocket 72 1 0 0.04 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.05 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.06 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.07 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.08 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.09 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.10 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.11 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.12 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.13 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.14 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 72 1 0 0.15 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.04 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.05 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.06 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.07 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.08 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.09 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.10 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.11 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.12 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.13 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.14 20000000 20 etcm 10000 finite-t/j0 &
srun --exclusive -n 1 ./pocket 48 1 0 0.15 20000000 20 etcm 10000 finite-t/j0 &

# srun --exclusive -n 1 ./pocket 84 0 0 0.05 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.06 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.07 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.08 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.09 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.10 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.12 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.14 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.16 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.18 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.20 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.25 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.30 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.40 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.50 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.60 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.70 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 0.85 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 1.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 1.50 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 2.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 3.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 4.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 5.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 10.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 20.00 20000000 20 etc 10000 vsweeps/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 50.00 20000000 20 etc 10000 vsweeps/0 &

# srun --exclusive -n 1 ./pocket 24 0 0.2 0.15 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.24 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2425 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.245 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.246 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2465 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.247 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2475 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.248 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2485 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.249 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.25 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2525 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.255 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.2575 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.26 20000000 20 etc 10000 vsweeps/0.2 &
# srun --exclusive -n 1 ./pocket 24 0 0.2 0.3 20000000 20 etc 10000 vsweeps/0.2 &

# srun --exclusive -n 1 ./pocket 24 0 0.3 0.2 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.23 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.26 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.265 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.27 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.275 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.28 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.285 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.29 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.295 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.3 20000000 20 etc 10000 vsweeps/0.3 &
# srun --exclusive -n 1 ./pocket 24 0 0.3 0.4 20000000 20 etc 10000 vsweeps/0.3 &

wait
