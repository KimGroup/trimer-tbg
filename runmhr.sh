#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=2G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=2
#SBATCH --ntasks=11

eval "$(conda shell.bash hook)"
conda activate env

srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/0.6/36*/histogram.dat" new-data/mhr/36_slp_0.6.json 100 s &
# srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/0.6/48*/histogram.dat" new-data/mhr/48_slp_0.6.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/0.6/60*/histogram.dat" new-data/mhr/60_slp_0.6.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/0.6/72*/histogram.dat" new-data/mhr/72_slp_0.6.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/0.6/84*/histogram.dat" new-data/mhr/84_slp_0.6.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/0.6/96*/histogram.dat" new-data/mhr/96_slp_0.6.json 100 s &

srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/36*/histogram.dat" new-data/mhr/36_slp_1.5.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/48*/histogram.dat" new-data/mhr/48_slp_1.5.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/60*/histogram.dat" new-data/mhr/60_slp_1.5.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/72*/histogram.dat" new-data/mhr/72_slp_1.5.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/84*/histogram.dat" new-data/mhr/84_slp_1.5.json 100 s &
srun --exclusive -n 1 -c 2 python3 mc/interpolate.py "new-data/vsweeps/binder-slp/1.5/96*/histogram.dat" new-data/mhr/96_slp_1.5.json 100 s &

wait
