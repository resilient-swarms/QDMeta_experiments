#!/bin/bash
IMAGENAME='rhex_metacmaes_aurorafinal.sif'

sudo singularity shell -w --bind $(pwd)/../../:/home/ $IMAGENAME
