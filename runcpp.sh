#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=1
#SBATCH --ntasks=3

srun --exclusive -n 1 ./pocket 84 1 0 0 200000000 20 etmR 50000 t0j0 &
srun --exclusive -n 1 ./pocket 108 1 0 0 200000000 20 etmR 100000 t0j0 &
srun --exclusive -n 1 ./pocket 120 1 0 0 200000000 20 etmR 100000 t0j0 &

# srun --exclusive -n 1 ./pocket 6 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 18 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 30 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 42 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 54 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 66 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 78 0 0 0 20000000 200 eE 10000 disordered &
# srun --exclusive -n 1 ./pocket 90 0 0 0 20000000 200 eE 10000 disordered &

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

# srun --exclusive -n 1 ./pocket 48 1 0.01 0.02 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.03 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.04 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.05 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.06 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.07 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.08 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.09 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.10 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.11 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.12 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.13 20000000 20 etcmo 10000 vsweeps/rough/0.01r3/rt3 &

# srun --exclusive -n 1 ./pocket 48 1 0.01 0.02 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.03 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.04 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.05 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.06 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.07 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.08 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.09 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.10 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.11 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.12 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &
# srun --exclusive -n 1 ./pocket 48 1 0.01 0.13 20000000 20 etcmoR 10000 vsweeps/rough/0.01r3/rand &

wait
