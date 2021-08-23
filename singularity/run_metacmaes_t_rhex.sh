#!/bin/bash


#SBATCH --tasks-per-node=1   # Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=08:00:00         # walltime
#SBATCH --job-name=rhexp_test

if [[ $2 == *meta ]]; then
source activate py3.7
maxfunevals=12000000
echo "getting the generation file for evals=$maxfunevals via epochs_log"
max_gen=$(python analysis/check_metagens.py -d $5/exp$4 -e $maxfunevals)
else
max_gen=30000 #when runs stopped at desired time this is ok
fi
echo $max_gen
singularity exec rhex_metacmaes_installed.sif bash run_metacmaes_test_rhex.sh "$1" "$2" "$3" "$4" "$5" "$max_gen"
