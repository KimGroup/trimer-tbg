#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=16
#SBATCH --ntasks=11

# ./pocket 48 0 -0.1 0.036,0.037,0.038,0.04,0.042,0.044,0.046,0.05 10000000 20 mectA 1000 pt-adaptive
# ./pocket 48 6 -0.1 0.033,0.035,0.0357,0.0359,0.0435,0.0461,0.05 10000000 15 mect 1000 temp/pt-doped5
# ./pocket 48 0 0.01 0.09,0.1,0.101,0.1023,0.1039,0.1062,0.1086,0.111,0.114,0.117,0.12 10000000 15 mect 1000 pt-adapted/0.01/2

# ./pocket 48 0 0.2 0.24,0.245,0.246,0.2465,0.247,0.2475,0.248,0.2485,0.249,0.25,0.255 10000000 15 mectA 2000 pt/0.2/48b
# ./pocket 48 0 -0.8 0.24,0.245,0.25,0.255,0.26,0.27,0.28,0.29,0.3 10000000 15 mectA 2000 rough-pt/-0.8/2
# ./pocket 60 0 0.2 0.24:12:0.255 10000000 15 mectA 5000 pt/0.2/60/1

# ./pocket 24 0 0.3 0.27:12:0.30 20000000 10 mectA 2000 vsweeps/pt/0.3/24/1
# ./pocket 60 0 0.1 0.195:12:0.200 20000000 10 mectA 5000 vsweeps/roughpt/0.1/60/1

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.2 0.246:16:0.253 30000000 15 mectoAE 5000 vsweeps/binder/0.2f/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.1 0.195:16:0.205 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 0.1 0.195:16:0.205 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.1 0.195:16:0.2025 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 0.1 0.195:16:0.2025 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.1 0.195:16:0.199 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 0.1 0.195:16:0.199 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.1 0.195:16:0.198 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 0.1 0.195:16:0.198 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.1 0.195:16:0.198 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 0.1 0.195:16:0.198 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.1 0.195:16:0.198 30000000 15 mectoA 5000 vsweeps/binder/0.1/ &

srun --exclusive -n 1 -c 16 ./pocket 24 0 0.3 0.265:16:0.30 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 30 0 0.3 0.265:16:0.30 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 36 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 42 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 48 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 54 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 60 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 66 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 72 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 78 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &
srun --exclusive -n 1 -c 16 ./pocket 84 0 0.3 0.265:16:0.29 30000000 15 mectoA 5000 vsweeps/binder/0.3b/ &

wait