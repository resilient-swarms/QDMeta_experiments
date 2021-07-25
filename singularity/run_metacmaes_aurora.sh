#!/bin/bash


#SBATCH --tasks-per-node=40   # Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=60:00:00         # walltime
#SBATCH --job-name=rhex_evol_au

singularity exec rhex_metacmaes_aurorafinal.sif bash run_metacmaes_evolutionaurora.sh "$1" "$2" "$3"



