#!/bin/bash

# this will run inside the singularity container

 export HOME_CONTAINER=/singularity_home/ 
 export BOTS_DIR=$HOME_CONTAINER/Resibots 
 export SFERES_DIR=$HOME_CONTAINER/sferes2
 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib/x86_64-linux-gnu:/lib:/usr/lib:/usr/local/lib:/usr/lib/x86_64-linux-gnu:$BOTS_DIR/lib


 test_type=$1       #  test for meta; test_envir or test_damage for control conditions
condition_prefix=$2  # damage_meta, envir_meta, cvt50D
control_type=$3   # need to supply argument with the method, e.g. b1p1 for meta, tra for cvt50D
replicate_number=$4
RESULTS_DIR=$5 # destination folder

mkdir -p $RESULTS_DIR/exp${replicate_number}
outputdir="${RESULTS_DIR}/exp${replicate_number}"

mkdir ${outputdir}

currentdir=$PWD
# get the last filename
max="-1"
get_last_filename()
{
echo "running last_filename; WARNING: make sure this is what you want; otherwise get the generation for the maxfunevals"
# get the last filename
shopt -s extglob
cd ${outputdir}
files=`ls -d !(*.*)` # gen_files don't have extension
for gen_file in $files; do
	number=${gen_file#"gen_"}
	#echo $number
	if (( $number > $max )); then
    		max=$number
	fi

done
}

get_last_filename    # when runs stopped at desired time this is ok
echo "max generation found is $max"
cd $currentdir 
echo "am now in $PWD"
binary=${SFERES_DIR}/build/exp/funexps/${test_type}_${condition_prefix}_binary
${binary} ${replicate_number} --load ${outputdir}/gen_${max} --d ${outputdir} -o ${outputdir}/${test_type}_damage_performance >> ${outputdir}/log_${test_type}.txt	

