#!/bin/bash

#SBATCH --ntasks-per-node=1  #Tasks per node
#SBATCH --nodes=1                # Number of nodes requested
#SBATCH --time=02:00:00         # walltime
#SBATCH --mem-per-cpu=80G  # actually no need to specify when nodes > 20

source activate py3.8

cd $SFERES_DIR/modules/cvt_map_elites
python cvt.py -d $1 -p $2 -k 10000 -f $3/meta-cmaes/centroids -l rastri_batch -b 100000
