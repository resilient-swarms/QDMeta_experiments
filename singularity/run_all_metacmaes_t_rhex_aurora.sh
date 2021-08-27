#!/bin/bash


sbatch run_metacmaes_t_rhex_aurora.sh test_damage "$1" "$2"

sbatch run_metacmaes_t_rhex_aurora.sh test_envir "$1" "$2"

sbatch run_metacmaes_t_rhex_aurora.sh test_max_damage "$1" "$2"

sbatch run_metacmaes_t_rhex_aurora.sh test_max_envir "$1" "$2"
