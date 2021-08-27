#!/bin/bash


sbatch run_metacmaes_t_rhex.sh test_damage "$1" "$2" "$3" "$4"

sbatch run_metacmaes_t_rhex.sh test_envir "$1" "$2" "$3" "$4"

sbatch run_metacmaes_t_rhex.sh test_max_damage "$1" "$2" "$3" "$4"

sbatch run_metacmaes_t_rhex.sh test_max_envir "$1" "$2" "$3" "$4"
