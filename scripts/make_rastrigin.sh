#!/bin/bash

cp -arv ~/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
cp -arv ~/RHex_experiments/funexps ${SFERES_DIR}/exp/  # copy wscript to correct folder
cp -arv ~/RHex_experiments/function_optimisation/*  $BOTS_DIR/include/meta-cmaes/

#$HOME/rhex_simu/rhex_dart/include/rhex_dart/

#cd $HOME/rhex_simu/rhex_dart/
#./waf configure --prefix=$BOTS_DIR
#./waf
#./waf install
export BUILD_TYPE=meta
export PERTURB_TYPE=1
cd $SFERES_DIR
#./waf distclean
./waf configure --exp funexps 
./waf --exp funexps 

export PERTURB_TYPE=2
./waf configure --exp funexps 
./waf --exp funexps 

export BUILD_TYPE=control
./waf configure --exp funexps 
./waf --exp funexps 


