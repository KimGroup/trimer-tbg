#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=16
#SBATCH --ntasks=12

# for i in {36..84..6}
# do
# srun --exclusive -n 1 -c 11 ./pocket $i 0 0.4 0.295:11:0.31 20000000 15 mectoARE 5000 vsweeps/binder/0.4/ &
# srun --exclusive -n 1 -c 11 ./pocket $i 0 0.5 0.31:11:0.33 20000000 15 mectoARE 5000 vsweeps/binder/0.5/ &
# srun --exclusive -n 1 -c 11 ./pocket $i 0 0.6 0.32:11:0.335 20000000 15 mectoARE 5000 vsweeps/binder/0.6/ &
# srun --exclusive -n 1 -c 11 ./pocket $i 0 0.7 0.32:11:0.34 20000000 15 mectoARE 5000 vsweeps/binder/0.7/ &
# done

srun --exclusive -n 1 -c 16 ./pocket 36 0 0.9 0.425:16:0.51 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
srun --exclusive -n 1 -c 16 ./pocket 48 0 0.9 0.425:16:0.50 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
srun --exclusive -n 1 -c 16 ./pocket 60 0 0.9 0.425:16:0.50 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
srun --exclusive -n 1 -c 16 ./pocket 72 0 0.9 0.425:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
srun --exclusive -n 1 -c 16 ./pocket 84 0 0.9 0.425:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &
srun --exclusive -n 1 -c 16 ./pocket 96 0 0.9 0.425:16:0.485 20000000 15 chAR 15000 vsweeps/binder-slp/0.9 &

srun --exclusive -n 1 -c 16 ./pocket 36 0 0.8 0.395:16:0.48 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
srun --exclusive -n 1 -c 16 ./pocket 48 0 0.8 0.395:16:0.47 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
srun --exclusive -n 1 -c 16 ./pocket 60 0 0.8 0.395:16:0.47 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
srun --exclusive -n 1 -c 16 ./pocket 72 0 0.8 0.395:16:0.46 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
srun --exclusive -n 1 -c 16 ./pocket 84 0 0.8 0.395:16:0.46 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &
srun --exclusive -n 1 -c 16 ./pocket 96 0 0.8 0.395:16:0.455 20000000 15 chAR 15000 vsweeps/binder-slp/0.8 &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1 &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 1 0.44:16:0.49 20000000 15 chAR 15000 vsweeps/binder-slp/1/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.5 0.315:16:0.33 20000000 15 eocARE 15000 vsweeps/binder/0.5b/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.5 0.315:16:0.325 20000000 15 eocARE 15000 vsweeps/binder/0.5b/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.5 0.315:16:0.324 20000000 15 eocARE 15000 vsweeps/binder/0.5b/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.5 0.315:16:0.323 20000000 15 eocARE 15000 vsweeps/binder/0.5b/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.5 0.315:16:0.3225 20000000 15 eocARE 15000 vsweeps/binder/0.5b/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.5 0.315:16:0.322 20000000 15 eocARE 15000 vsweeps/binder/0.5b/ &

# srun --exclusive -n 1 -c 16 ./pocket 36 0 0.6 0.323:16:0.335 20000000 15 mectoARE 15000 vsweeps/binder/0.6b/ &
# srun --exclusive -n 1 -c 16 ./pocket 48 0 0.6 0.323:16:0.333 20000000 15 mectoARE 15000 vsweeps/binder/0.6b/ &
# srun --exclusive -n 1 -c 16 ./pocket 60 0 0.6 0.323:16:0.331 20000000 15 mectoARE 15000 vsweeps/binder/0.6b/ &
# srun --exclusive -n 1 -c 16 ./pocket 72 0 0.6 0.323:16:0.330 20000000 15 mectoARE 15000 vsweeps/binder/0.6b/ &
# srun --exclusive -n 1 -c 16 ./pocket 84 0 0.6 0.323:16:0.3295 20000000 15 mectoARE 15000 vsweeps/binder/0.6b/ &
# srun --exclusive -n 1 -c 16 ./pocket 96 0 0.6 0.323:16:0.329 20000000 15 mectoARE 15000 vsweeps/binder/0.6b/ &

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

# srun --exclusive -n 1 -c 16 ./pocket 48 0 2 0.33:16:0.348 30000000 15 mectoAR 5000 dsweeps/0 &

wait