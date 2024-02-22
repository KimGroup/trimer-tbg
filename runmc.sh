#!/bin/bash
#SBATCH --partition=kim,default_partition
#SBATCH --mem-per-cpu=1G
#SBATCH --time=2-0
#SBATCH --cpus-per-task=16
#SBATCH --ntasks=1

./pocket 36 0 1 0 20000000 20 C 16 muca

wait
