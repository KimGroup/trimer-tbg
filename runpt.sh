#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=16
#SBATCH --ntasks=12

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.5 0.3125:16:0.335 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.5 0.3125:16:0.33 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.5 0.3125:16:0.325 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.5 0.3125:16:0.324 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.5 0.3125:16:0.323 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.5 0.3125:16:0.3225 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.5 0.3125:16:0.322 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 108 0 0.5 0.3125:16:0.3218 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 120 0 0.5 0.3125:16:0.3216 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.6 0.320:16:0.338 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.6 0.320:16:0.335 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.6 0.321:16:0.333 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.6 0.322:16:0.331 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.6 0.323:16:0.330 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.6 0.323:16:0.329 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.6 0.323:16:0.3285 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 108 0 0.6 0.323:16:0.3282 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 120 0 0.6 0.323:16:0.328 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.6 0.32:16:0.41 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.6 0.32:16:0.40 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.6 0.32:16:0.40 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.6 0.32:16:0.39 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.6 0.32:16:0.39 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.6 0.32:16:0.385 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.8 0.395:16:0.48 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.8 0.395:16:0.47 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.8 0.395:16:0.47 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.8 0.395:16:0.46 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.8 0.395:16:0.46 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.8 0.395:16:0.455 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.9 0.425:16:0.51 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.9 0.425:16:0.50 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.9 0.425:16:0.50 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.9 0.425:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.9 0.425:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.9 0.425:16:0.485 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &


# srun --exclusive -n 1 -c 16 ./pocket 24 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1 &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 108 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 1.5 0.55:16:0.75 20000000 15 chAR 15000 vsweeps/binder-slp/1.5 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1.5 0.55:16:0.73 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 1.5 0.55:16:0.71 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 1.5 0.55:16:0.69 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 1.5 0.55:16:0.67 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 1.5 0.55:16:0.65 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 2 0.32:16:0.36 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 2 0.32:16:0.355 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.32:16:0.35 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 2 0.32:16:0.348 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 2 0.32:16:0.346 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 2 0.32:16:0.345 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 2 0.32:16:0.345 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 108 0 2 0.32:16:0.3445 40000000 15 chAR 15000 vsweeps/binder/2b &
# srun --exclusive -n 1 -c 16 ./pocket 120 0 2 0.32:16:0.344 40000000 15 chAR 15000 vsweeps/binder/2b &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 2 0.57:16:0.86 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 2 0.57:16:0.86 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.57:16:0.82 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 2 0.58:16:0.80 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 2 0.58:16:0.78 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 2 0.58:16:0.76 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 2 0.58:16:0.74 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 108 0 2 0.58:16:0.72 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
# srun --exclusive -n 1 -c 16 ./pocket 120 0 2 0.58:16:0.71 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &

wait