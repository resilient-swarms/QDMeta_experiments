#!/bin/bash

cp -arv ~/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
mkdir ${SFERES_DIR}/exp/Rhexps
cp -arv ~/RHex_experiments/Rhexps ${SFERES_DIR}/exp/Rhexps  # copy wscript to correct folder
cd $SFERES_DIR
./waf distclean
./waf configure --exp Rhexps
./waf --exp Rhexps


