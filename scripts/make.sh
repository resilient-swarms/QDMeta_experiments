#!/bin/bash

cp -arv ~/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
cp -arv ~/RHex_experiments/rhexps ${SFERES_DIR}/exp/  # copy wscript to correct folder
cp -arv ~/RHex_experiments/rhex_dart/*  $BOTS_DIR/include/rhex_dart/
cp -av ~/RHex_experiments/function_optimisation/aurora_params_fun.hpp  $BOTS_DIR/include/meta-cmaes/aurora_params_fun.hpp 
cp -av ~/RHex_experiments/meta-cmaes/aurora_qd_container.hpp ${SFERES_DIR}/sferes/stat/qd_container.hpp

#$HOME/rhex_simu/rhex_dart/include/rhex_dart/

#cd $HOME/rhex_simu/rhex_dart/
#./waf configure --prefix=$BOTS_DIR
#./waf
#./waf install
cd $SFERES_DIR
./waf distclean
./waf configure --exp rhexps
./waf --exp rhexps


