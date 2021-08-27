#!/bin/bash


#SBATCH --tasks-per-node=1   # Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=08:00:00         # walltime
#SBATCH --job-name=rhexp_test

singularity exec rhex_metacmaes_aurorafinal.sif bash run_metacmaes_test_rhex_aurora.sh "$1" "$2" "$3" 
