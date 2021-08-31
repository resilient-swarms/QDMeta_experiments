#!/bin/bash

for i in `seq 0 9`; do 
	echo $i
	$SFERES_DIR/build/exp/rhexps/rhex_metaCMAES_testgenotype_binary $1 $i

	ffmpeg -r 24 -i ~/ToyData/video_world${i}_damage_none%4d.png -vcodec libx264 -y -an ~/ToyData/${2}_world${i}.mp4 -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2"
	rm ~/ToyData/video_world${i}_damage_none*
done


