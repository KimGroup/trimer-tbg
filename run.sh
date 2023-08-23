#!/bin/bash
#SBATCH --partition=kim
#SBATCH --mem-per-cpu=1G

source /home/kz345/.bashrc
conda activate env
python3.11 -u mc/run_pocket.py
