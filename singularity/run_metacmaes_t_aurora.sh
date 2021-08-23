#!/bin/bash


#SBATCH --tasks-per-node=1   # Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=24:00:00         # walltime
#SBATCH --job-name=funexp_test

singularity exec rastri_metacmaes_aurorafinal.sif bash run_metacmaes_test_aurora.sh "$1" "$2" "$3" 
