#!/bin/bash

for i in `seq 0 9`; do 
	echo $i
	$SFERES_DIR/build/exp/rhexps/rhex_metaCMAES_testgenotype_binary $1 $i
done

