#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=3-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=1

# srun --exclusive -n 1 ./a.out 12 1 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 24 4 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 36 9 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 48 16 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 60 25 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 72 36 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 84 49 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 96 64 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 108 81 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &
# srun --exclusive -n 1 ./a.out 120 100 0 0 50000000 20 mtd 5000 t0j0/doped/1.48 &

# srun --exclusive -n 1 ./a.out 108 1 0 0 50000000 20 mtd 5000 t0j0/doped/-1 &
# srun --exclusive -n 1 ./a.out 120 1 0 0 50000000 20 mtd 5000 t0j0/doped/-1 &
srun --exclusive -n 1 ./a.out 96 1 0 0 200000000 10 mtd 2000 t0j0/doped/-1 &

# srun --exclusive -n 1 ./a.out 24 1 0 0 50000000 20 mtd 5000 t0j0/doped/1.196 &
# srun --exclusive -n 1 ./a.out 48 4 0 0 50000000 20 mtd 5000 t0j0/doped/1.196 &
# srun --exclusive -n 1 ./a.out 72 9 0 0 50000000 20 mtd 5000 t0j0/doped/1.196 &
# srun --exclusive -n 1 ./a.out 96 16 0 0 50000000 20 mtd 5000 t0j0/doped/1.196 &
# srun --exclusive -n 1 ./a.out 120 25 0 0 50000000 20 mtd 5000 t0j0/doped/1.196 &

# srun --exclusive -n 1 ./a.out 6 1 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 12 4 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 18 9 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 24 16 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 30 25 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 36 36 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 42 49 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 48 64 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 54 81 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &
# srun --exclusive -n 1 ./a.out 60 100 0 0 50000000 20 mtd 5000 t0j0/doped/1.12 &

# srun --exclusive -n 1 ./a.out 18 1 0 0 50000000 20 mtd 5000 t0j0/doped/1.108 &
# srun --exclusive -n 1 ./a.out 36 4 0 0 50000000 20 mtd 5000 t0j0/doped/1.108 &
# srun --exclusive -n 1 ./a.out 48 9 0 0 50000000 20 mtd 5000 t0j0/doped/1.108 &
# srun --exclusive -n 1 ./a.out 72 16 0 0 50000000 20 mtd 5000 t0j0/doped/1.108 &
# srun --exclusive -n 1 ./a.out 90 25 0 0 50000000 20 mtd 5000 t0j0/doped/1.108 &

# srun --exclusive -n 1 ./a.out 12 1 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 24 2 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 36 3 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 48 4 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 60 5 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 72 6 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 84 7 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &
# srun --exclusive -n 1 ./a.out 96 8 0 0 50000000 20 mtd 5000 t0j0/doped/lin1.48 &

wait
