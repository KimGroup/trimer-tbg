slurm jupyter notebook:

srun -t 1-0 --mem=4G --pty bash
conda activate env
jupyter notebook

on another terminal:

ssh kz345@{nodename}.infosci.cornell.edu -L 8887:localhost:8888

in vscode: forward port 8887

connect to http://localhost:8887