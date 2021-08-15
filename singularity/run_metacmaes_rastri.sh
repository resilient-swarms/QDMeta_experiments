#!/bin/bash
#SBATCH --tasks-per-node=1   # Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=60:00:00         # walltime
#SBATCH --job-name=rastri_evol

singularity exec rastrigin_metacmaes_installed.sif bash run_metacmaes_evorastri.sh "$1" "$2" "$3" "$4"
