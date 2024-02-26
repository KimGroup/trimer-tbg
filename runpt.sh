#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=16
#SBATCH --ntasks=12

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.9 0.425:16:0.51 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.9 0.425:16:0.50 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.9 0.425:16:0.50 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.9 0.425:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.9 0.425:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.9 0.425:16:0.485 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.8 0.395:16:0.48 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.8 0.395:16:0.47 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.8 0.395:16:0.47 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.8 0.395:16:0.46 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.8 0.395:16:0.46 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.8 0.395:16:0.455 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.6 0.32:16:0.41 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.6 0.32:16:0.40 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.6 0.32:16:0.40 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.6 0.32:16:0.39 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.6 0.32:16:0.39 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.6 0.32:16:0.385 20000000 15 chAR 15000 vsweeps/binder-slp/0.6 &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 1.5 0.55:16:0.75 20000000 15 chAR 15000 vsweeps/binder-slp/1.5 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1.5 0.55:16:0.73 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 1.5 0.55:16:0.71 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 1.5 0.55:16:0.69 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 1.5 0.55:16:0.67 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 1.5 0.55:16:0.65 20000000 15 chAR 15000 vsweeps/binder-slp/1.5/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.5 0.3125:16:0.33 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.5 0.3125:16:0.325 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.5 0.3125:16:0.324 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.5 0.3125:16:0.323 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.5 0.3125:16:0.3225 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.5 0.3125:16:0.322 40000000 15 chAR 15000 vsweeps/binder/0.5c/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.6 0.320:16:0.335 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.6 0.321:16:0.333 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.6 0.322:16:0.331 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.6 0.323:16:0.330 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.6 0.323:16:0.329 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.6 0.323:16:0.3285 40000000 15 chAR 15000 vsweeps/binder/0.6c/ &

# srun --exclusive -n 1 -c 11 ./pocket 48 0 0.5 0.31:11:0.33 10000000 15 mectoARE 5000 ehist/0.5/ &
# srun --exclusive -n 1 -c 11 ./pocket 48 0 0.6 0.32:11:0.335 10000000 15 mectoARE 5000 ehist/0.6/ &
# srun --exclusive -n 1 -c 11 ./pocket 48 0 0.7 0.32:11:0.34 10000000 15 mectoARE 5000 ehist/0.7/ &
# srun --exclusive -n 1 -c 11 ./pocket 48 0 0.8 0.325:11:0.344 10000000 15 mectoARE 5000 ehist/0.8/ &
# srun --exclusive -n 1 -c 11 ./pocket 48 0 0.9 0.325:11:0.344 10000000 15 mectoARE 5000 ehist/0.9/ &

# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.3 0.275:16:0.293 10000000 15 mectoARE 5000 ehist/0.3/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1 0.325:16:0.348 10000000 15 mectoARE 5000 ehist/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.1 0.1953:16:0.198 20000000 15 mectoARE 5000 ehist/0.1/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.3 0.275:16:0.29 20000000 15 mectoARE 5000 ehist/0.3/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 1 0.325:16:0.345 20000000 15 mectoARE 5000 ehist/1/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.2 0.246:16:0.253 30000000 15 mectoARE 5000 vsweeps/binder/0.2f/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.1 0.1956:16:0.205 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 0.1 0.1956:16:0.205 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.1 0.1956:16:0.2025 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 0.1 0.1956:16:0.2025 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.1 0.1956:16:0.199 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 0.1 0.1956:16:0.199 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.1 0.1956:16:0.1985 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 0.1 0.1956:16:0.1985 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.1 0.1956:16:0.1985 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 0.1 0.1956:16:0.1985 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.1 0.1956:16:0.1985 50000000 15 mectoAR 5000 vsweeps/binder/0.1b/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 0.3 0.275:16:0.30 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 0.3 0.275:16:0.30 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.3 0.275:16:0.29 30000000 15 mectoAR 5000 vsweeps/binder/0.3c/ &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 1 0.325:16:0.36 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 1 0.325:16:0.36 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 1 0.325:16:0.345 30000000 15 mectoAR 5000 vsweeps/binder/1b &

# srun --exclusive -n 1 -c 16 ./pocket 24 0 2 0.33:16:0.36 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 30 0 2 0.33:16:0.36 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 36 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 42 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 54 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 66 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 78 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 vsweeps/binder/2 &

srun --exclusive -n 1 -c 16 ./pocket 36 0 2 0.32:16:0.355 40000000 15 chAR 15000 vsweeps/binder/2b &
srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.32:16:0.35 40000000 15 chAR 15000 vsweeps/binder/2b &
srun --exclusive -n 1 -c 16 ./pocket 60 0 2 0.32:16:0.348 40000000 15 chAR 15000 vsweeps/binder/2b &
srun --exclusive -n 1 -c 16 ./pocket 72 0 2 0.32:16:0.346 40000000 15 chAR 15000 vsweeps/binder/2b &
srun --exclusive -n 1 -c 16 ./pocket 84 0 2 0.32:16:0.345 40000000 15 chAR 15000 vsweeps/binder/2b &
srun --exclusive -n 1 -c 16 ./pocket 96 0 2 0.32:16:0.345 40000000 15 chAR 15000 vsweeps/binder/2b &

srun --exclusive -n 1 -c 16 ./pocket 36 0 2 0.57:16:0.86 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.57:16:0.82 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
srun --exclusive -n 1 -c 16 ./pocket 60 0 2 0.58:16:0.80 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
srun --exclusive -n 1 -c 16 ./pocket 72 0 2 0.58:16:0.78 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
srun --exclusive -n 1 -c 16 ./pocket 84 0 2 0.58:16:0.76 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &
srun --exclusive -n 1 -c 16 ./pocket 96 0 2 0.58:16:0.74 40000000 15 chAR 15000 vsweeps/binder-slp/2b/ &


# srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 dsweeps/0 &

wait