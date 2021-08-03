#!/bin/bash
 export HOME_CONTAINER=/singularity_home/ 
 export BOTS_DIR=$HOME_CONTAINER/Resibots 
 export SFERES_DIR=$HOME_CONTAINER/sferes2
 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib/x86_64-linux-gnu:/lib:/usr/lib:/usr/local/lib:/usr/lib/x86_64-linux-gnu:$BOTS_DIR/lib:/opt/miniconda3/envs/py3.8/lib/python3.8/site-packages/torch/lib

export BUILD_DEBUG=False
cp -arv $HOME_CONTAINER/RHex_experiments/meta-cmaes/ ${BOTS_DIR}/include
cp -arv $HOME_CONTAINER/RHex_experiments/rhexps/* ${SFERES_DIR}/exp/Rhexps/  # copy wscript to correct folder
cp -arv $HOME_CONTAINER/RHex_experiments/rhex_dart/*  $BOTS_DIR/include/rhex_dart/
cp -arv $HOME_CONTAINER/RHex_experiments/AURORA ${BOTS_DIR}/include/
cp -av kd_tree.h /usr/local/include/ssrc/spatial/
cp -av meta-cmaes/network_loader_pytorch.hpp $BOTS_DIR/include/AURORA/cpp/modifier/
#export NUM_CORES=5
export LIBTORCH_LOCATION="/opt/miniconda3/envs/py3.8/lib/python3.8/site-packages/torch"
export BUILD_TYPE=AURORA
# compile
cd $SFERES_DIR
./waf distclean
./waf configure --exp Rhexps --no-mpi #--boost-includes=/usr/local/include --boost-libs=/usr/local/lib
./waf --exp Rhexps --no-mpi #--boost-includes=/usr/local/include --boost-libs=/usr/local/lib
