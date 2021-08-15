#!/bin/bash


#SBATCH --tasks-per-node=1   # Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=60:00:00         # walltime
#SBATCH --job-name=rastri_au

singularity exec rastri_metacmaes_aurorafinal.sif bash run_metacmaes_evolutionaurora_rastri.sh "$1" "$2" "$3"



