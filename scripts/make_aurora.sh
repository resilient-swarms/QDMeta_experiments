#!/bin/bash

export BUILD_TYPE=aurora
cp -arv $HOME/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
cp -arv $HOME/RHex_experiments/rhexps ${SFERES_DIR}/exp/  # copy wscript to correct folder
cp -arv $HOME/RHex_experiments/rhex_dart/* ${BOTS_DIR}/include/rhex_dart

cp -arv $HOME/RHex_experiments/aurora/ ${BOTS_DIR}/include

cd $SFERES_DIR
./waf distclean
./waf configure --exp rhexps 
./waf --exp rhexps



