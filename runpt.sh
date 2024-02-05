#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=3-0
#SBATCH --cpus-per-task=14
#SBATCH --ntasks=1

# ./pocket 48 0 -0.1 0.02,0.025,0.03,0.032,0.034,0.036,0.038,0.04,0.045,0.05 5000000 20 mtec 1000 pttest/-0.1-b-noswap &
# ./pocket 48 0 -0.1 0.055,0.06,0.065,0.07,0.08,0.09,0.1,0.2 50000000 20 mtecP 1000 pttest/-0.1-b &
# ./pocket 48 0 -0.1 0.25,0.3,0.35,0.4,0.45,0.5,0.6,0.7 50000000 20 mtecP 1000 pttest/-0.1-b &
./pocket 48 0 0.01 0.6,0.08,0.085,0.09,0.095,0.098,0.1,0.102,0.105,0.11,0.115,0.13,0.15,0.2 5000000 20 mtec 1000 pttest/0.01 &

wait
