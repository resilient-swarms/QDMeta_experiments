#!/bin/bash

cp -arv ~/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
cp -arv ~/RHex_experiments/rhexps ${SFERES_DIR}/exp/  # copy wscript to correct folder
cd $SFERES_DIR
./waf distclean
./waf configure --exp rhexps
./waf --exp rhexps


