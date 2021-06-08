#!/bin/bash

cp -arv ~/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
cp -arv ~/RHex_experiments/rhexps ${SFERES_DIR}/exp/  # copy wscript to correct folder
cp -arv ~/RHex_experiments/rhex_dart/*  $BOTS_DIR/include/rhex_dart/

#$HOME/rhex_simu/rhex_dart/include/rhex_dart/

#cd $HOME/rhex_simu/rhex_dart/
#./waf configure --prefix=$BOTS_DIR
#./waf
#./waf install
cd $SFERES_DIR
./waf distclean
./waf configure --exp rhexps
./waf --exp rhexps


