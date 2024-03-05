#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=2G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=2
#SBATCH --ntasks=36

eval "$(conda shell.bash hook)"
conda activate env

srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/24*/histogram.dat" new-data/mhr/24_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/36*/histogram.dat" new-data/mhr/36_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/48*/histogram.dat" new-data/mhr/48_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/60*/histogram.dat" new-data/mhr/60_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/72*/histogram.dat" new-data/mhr/72_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/84*/histogram.dat" new-data/mhr/84_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/96*/histogram.dat" new-data/mhr/96_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/108*/histogram.dat" new-data/mhr/108_0.5c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.5c/120*/histogram.dat" new-data/mhr/120_0.5c.json 500 k &

srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/24*/histogram.dat" new-data/mhr/24_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/36*/histogram.dat" new-data/mhr/36_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/48*/histogram.dat" new-data/mhr/48_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/60*/histogram.dat" new-data/mhr/60_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/72*/histogram.dat" new-data/mhr/72_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/84*/histogram.dat" new-data/mhr/84_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/96*/histogram.dat" new-data/mhr/96_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/108*/histogram.dat" new-data/mhr/108_0.6c.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/0.6c/120*/histogram.dat" new-data/mhr/120_0.6c.json 500 k &

# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/36*/histogram.dat" new-data/mhr/36_slp_1.5.json 100 s &
# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/48*/histogram.dat" new-data/mhr/48_slp_1.5.json 100 s &
# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/60*/histogram.dat" new-data/mhr/60_slp_1.5.json 100 s &
# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/72*/histogram.dat" new-data/mhr/72_slp_1.5.json 100 s &
# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/84*/histogram.dat" new-data/mhr/84_slp_1.5.json 100 s &
# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/96*/histogram.dat" new-data/mhr/96_slp_1.5.json 100 s &

srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/24*/histogram.dat" new-data/mhr/24_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/36*/histogram.dat" new-data/mhr/36_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/48*/histogram.dat" new-data/mhr/48_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/60*/histogram.dat" new-data/mhr/60_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/72*/histogram.dat" new-data/mhr/72_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/84*/histogram.dat" new-data/mhr/84_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/96*/histogram.dat" new-data/mhr/96_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/108*/histogram.dat" new-data/mhr/108_slp_2b.json 500 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/2b/120*/histogram.dat" new-data/mhr/120_slp_2b.json 500 s &

srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/24*/histogram.dat" new-data/mhr/24_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/36*/histogram.dat" new-data/mhr/36_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/48*/histogram.dat" new-data/mhr/48_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/60*/histogram.dat" new-data/mhr/60_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/72*/histogram.dat" new-data/mhr/72_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/84*/histogram.dat" new-data/mhr/84_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/96*/histogram.dat" new-data/mhr/96_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/108*/histogram.dat" new-data/mhr/108_2b.json 500 k &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder/2b/120*/histogram.dat" new-data/mhr/120_2b.json 500 k &

wait
