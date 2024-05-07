#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=2

# srun --exclusive -n 1 ./pocket 48 48 0.01 0.01 20000000 20 tOR 10000 openbc/0.01o &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.01 20000000 20 tR 10000 openbc/0.01p &
# srun --exclusive -n 1 ./pocket 48 48 0.01 0.01 20000000 20 tR 10000 openbc/0.01p &
# srun --exclusive -n 1 ./pocket 48 1 -0.1 0.01 20000000 20 tR 10000 openbc/-0.1p &
# srun --exclusive -n 1 ./pocket 48 48 -0.1 0.01 20000000 20 tR 10000 openbc/-0.1p &
# srun --exclusive -n 1 ./pocket 48 96 -0.1 0.01 20000000 20 tR 10000 openbc/-0.1p &
# srun --exclusive -n 1 ./pocket 48 144 -0.1 0.01 20000000 20 tR 10000 openbc/-0.1p &
srun --exclusive -n 1 ./pocket 48 192 -0.1 0.01 20000000 20 tR 10000 openbc/-0.1p &
srun --exclusive -n 1 ./pocket 48 240 -0.1 0.01 20000000 20 tR 10000 openbc/-0.1p &

# srun --exclusive -n 1 ./pocket 24 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 36 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 48 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 60 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 72 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 84 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 96 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 108 0 2 0.5 50000000 20 htR 30000 slp &
# srun --exclusive -n 1 ./pocket 120 0 2 0.5 50000000 20 htR 30000 slp &

# srun --exclusive -n 1 ./pocket 48 1 0 0.04 100000000 20 emtc 40000 finite-t-ergodic/0.04 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.04 100000000 20 emtc 40000 finite-t-ergodic/0.04 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.04 100000000 20 emtc 40000 finite-t-ergodic/0.04 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.04 100000000 20 emtc 40000 finite-t-ergodic/0.04 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.06 100000000 20 emtc 40000 finite-t-ergodic/0.06 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.06 100000000 20 emtc 40000 finite-t-ergodic/0.06 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.06 100000000 20 emtc 40000 finite-t-ergodic/0.06 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.06 100000000 20 emtc 40000 finite-t-ergodic/0.06 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.08 100000000 20 emtc 40000 finite-t-ergodic/0.08 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.08 100000000 20 emtc 40000 finite-t-ergodic/0.08 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.08 100000000 20 emtc 40000 finite-t-ergodic/0.08 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.08 100000000 20 emtc 40000 finite-t-ergodic/0.08 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.1 100000000 20 emtc 40000 finite-t-ergodic/0.1 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.1 100000000 20 emtc 40000 finite-t-ergodic/0.1 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.1 100000000 20 emtc 40000 finite-t-ergodic/0.1 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.1 100000000 20 emtc 40000 finite-t-ergodic/0.1 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.12 100000000 20 emtc 40000 finite-t-ergodic/0.12 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.12 100000000 20 emtc 40000 finite-t-ergodic/0.12 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.12 100000000 20 emtc 40000 finite-t-ergodic/0.12 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.12 100000000 20 emtc 40000 finite-t-ergodic/0.12 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.15 100000000 20 emtc 40000 finite-t-ergodic/0.15 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.15 100000000 20 emtc 40000 finite-t-ergodic/0.15 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.15 100000000 20 emtc 40000 finite-t-ergodic/0.15 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.15 100000000 20 emtc 40000 finite-t-ergodic/0.15 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.2 100000000 20 emtc 40000 finite-t-ergodic/0.2 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.2 100000000 20 emtc 40000 finite-t-ergodic/0.2 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.2 100000000 20 emtc 40000 finite-t-ergodic/0.2 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.2 100000000 20 emtc 40000 finite-t-ergodic/0.2 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.25 100000000 20 emtc 40000 finite-t-ergodic/0.25 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.25 100000000 20 emtc 40000 finite-t-ergodic/0.25 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.25 100000000 20 emtc 40000 finite-t-ergodic/0.25 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.25 100000000 20 emtc 40000 finite-t-ergodic/0.25 &

# srun --exclusive -n 1 ./pocket 48 1 0 0.3 100000000 20 emtc 40000 finite-t-ergodic/0.3 &
# srun --exclusive -n 1 ./pocket 72 1 0 0.3 100000000 20 emtc 40000 finite-t-ergodic/0.3 &
# srun --exclusive -n 1 ./pocket 96 1 0 0.3 100000000 20 emtc 40000 finite-t-ergodic/0.3 &
# srun --exclusive -n 1 ./pocket 120 1 0 0.3 100000000 20 emtc 40000 finite-t-ergodic/0.3 &

# srun --exclusive -n 1 ./pocket 72 1 -1 0.1 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 96 1 -1 0.1 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 120 1 -1 0.1 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 48 1 -1 0.1 50000000 10 htmRI 20000 infinite-u/-1 &

# srun --exclusive -n 1 ./pocket 72 1 -1 0.2 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 96 1 -1 0.2 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 120 1 -1 0.2 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 48 1 -1 0.2 50000000 10 htmRI 20000 infinite-u/-1 &

# srun --exclusive -n 1 ./pocket 72 1 -1 2 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 3 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 4 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 5 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 6 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 7 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 8 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 9 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 10 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 15 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 20 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 25 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 30 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 35 50000000 10 htmRI 20000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 40 50000000 10 htmRI 20000 infinite-u/-1 &

# srun --exclusive -n 1 ./pocket 24 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 36 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 48 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 60 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 72 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 84 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 96 1 1 15 50000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 108 1 1 15 100000000 20 htmRI 30000 infinite-u/1 &
# srun --exclusive -n 1 ./pocket 120 1 1 15 100000000 20 htmRI 30000 infinite-u/1 &

# srun --exclusive -n 1 ./pocket 24 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 36 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 48 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 60 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 84 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 96 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 108 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 120 1 -1 1.5 50000000 20 htmRI 30000 infinite-u/-1 &

# srun --exclusive -n 1 ./pocket 24 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 36 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 48 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 60 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 72 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 84 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 96 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 108 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &
# srun --exclusive -n 1 ./pocket 120 1 -1 0.5 50000000 20 htmRI 30000 infinite-u/-1 &

# srun --exclusive -n 1 ./pocket 24 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 36 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 48 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 60 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 72 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 84 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 96 1 0 0 50000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 108 1 0 0 100000000 10 htmRI 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 120 1 0 0 100000000 10 htmRI 30000 t0j0-ergodic &

# srun --exclusive -n 1 ./pocket 24 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 36 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 48 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 60 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 72 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 84 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 96 2 0 0 50000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 108 2 0 0 100000000 10 tm 30000 t0j0-ergodic &
# srun --exclusive -n 1 ./pocket 120 2 0 0 100000000 10 tm 30000 t0j0-ergodic &

# srun --exclusive -n 1 ./pocket 48 0 0 0 2000000 5 H 1000 dimers-hex/48x48 &

# srun --exclusive -n 1 ./pocket 6 0 0 0 20000000 1 D 1 dimers/6x6 &
# srun --exclusive -n 1 ./pocket 12 0 0 0 20000000 1 D 1 dimers/12x12 &
# srun --exclusive -n 1 ./pocket 24 0 0 0 20000000 1 D 1 dimers/24x24 &
# srun --exclusive -n 1 ./pocket 36 0 0 0 20000000 1 D 1 dimers/36x36 &
# srun --exclusive -n 1 ./pocket 48 0 0 0 20000000 1 D 1 dimers/48x48 &
# srun --exclusive -n 1 ./pocket 60 0 0 0 20000000 1 D 1 dimers/60x60 &
# srun --exclusive -n 1 ./pocket 72 0 0 0 20000000 1 D 1 dimers/72x72 &
# srun --exclusive -n 1 ./pocket 84 0 0 0 20000000 1 D 1 dimers/84x84 &
# srun --exclusive -n 1 ./pocket 108 0 0 0 20000000 1 D 1 dimers/108x108 &
# srun --exclusive -n 1 ./pocket 120 0 0 0 20000000 1 D 1 dimers/120x120 &

# srun --exclusive -n 1 ./pocket 12 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 24 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 36 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 48 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 60 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 72 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 84 0 0 1 50000000 20 hwI 10000 measure-k/0 &
# srun --exclusive -n 1 ./pocket 96 0 0 1 50000000 20 hwI 10000 measure-k/0 &

# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.03 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.05 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.07 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.09 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.11 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.13 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.15 20000000 20 emtc 10000 finite-t/j-0.01 &
# srun --exclusive -n 1 ./pocket 72 1 -0.01 0.17 20000000 20 emtc 10000 finite-t/j-0.01 &

# srun --exclusive -n 1 ./pocket 72 1 0.001 0.03 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.05 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.07 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.09 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.11 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.13 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.15 20000000 20 emtc 10000 finite-t/j-0.001 &
# srun --exclusive -n 1 ./pocket 72 1 0.001 0.17 20000000 20 emtc 10000 finite-t/j-0.001 &

# srun --exclusive -n 1 ./pocket 48 16 1 3.1 20000000 20 etcmoRIE 10000 dsweeps/1.48/rand &

# srun --exclusive -n 1 ./pocket 120 1 0 0 200000000 20 etm 100000 t0j0 &
# srun --exclusive -n 1 ./pocket 120 0 0 0 20000000 200 eE 10000 disordered &

# srun --exclusive -n 1 ./pocket 12 0 0.05 0.155 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.156 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.157 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.158 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.159 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.160 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.161 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.162 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.163 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.164 20000000 20 etc 10000 vsweeps/rough/0.05 &
# srun --exclusive -n 1 ./pocket 12 0 0.05 0.165 20000000 20 etc 10000 vsweeps/rough/0.05 &

# srun --exclusive -n 1 ./pocket 12 0 0.1 0.19 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.195 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.20 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.205 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.21 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.215 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.22 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.225 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.23 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.235 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.24 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.245 20000000 20 etc 10000 vsweeps/rough/0.1 &
# srun --exclusive -n 1 ./pocket 12 0 0.1 0.25 20000000 20 etc 10000 vsweeps/rough/0.1 &

# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.030 20000000 20 etco 10000 vsweeps/rough/-0.1/bw &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.032 20000000 20 etco 10000 vsweeps/rough/-0.1/bw &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.034 20000000 20 etco 10000 vsweeps/rough/-0.1/bw &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.036 20000000 20 etco 10000 vsweeps/rough/-0.1/bw &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.038 20000000 20 etco 10000 vsweeps/rough/-0.1/bw &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.040 20000000 20 etco 10000 vsweeps/rough/-0.1/bw &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.030 20000000 20 etcoR 10000 vsweeps/rough/-0.1/rand &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.032 20000000 20 etcoR 10000 vsweeps/rough/-0.1/rand &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.034 20000000 20 etcoR 10000 vsweeps/rough/-0.1/rand &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.036 20000000 20 etcoR 10000 vsweeps/rough/-0.1/rand &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.038 20000000 20 etcoR 10000 vsweeps/rough/-0.1/rand &
# srun --exclusive -n 1 ./pocket 24 0 -0.1 0.040 20000000 20 etcoR 10000 vsweeps/rough/-0.1/rand &

# srun --exclusive -n 1 ./pocket 48 0 0.01 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 0.008 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 0.006 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 0.004 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 0.002 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 0 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 -0.002 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 -0.004 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 -0.006 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 -0.008 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &
# srun --exclusive -n 1 ./pocket 48 0 -0.01 0.1 20000000 20 etcoR 10000 hsweeps/0.1 &

wait
