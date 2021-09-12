# QD-Meta evolution and adaptation experiments on RHex robot and Rastrigin function

This repository is used to conduct evolution and adaptation experiments with QD-Meta, a system for customising quality-diversity archives to a meta-objective. 

![diagram_fig](https://user-images.githubusercontent.com/8057857/132875526-2ec8e0ba-ae32-45f9-99b4-67c861d78cc0.png)


The work is available as a preprint:

Bossens, D. M., & Tarapore, D. (2021). Quality-Diversity Meta-Evolution: customising behaviour spaces to a meta-objective. ArXiv Preprint ArXiv:2109.03918, 1–17. http://arxiv.org/abs/2109.03918


## Dependencies
While dependencies will be installed if you are using the singularity scripts in singularity/, if you do not choose singularity/ it is worth knowing the dependencies:
- sferes2 https://github.com/sferes2/sferes2 [evolutionary framework for all experiments]
- meta-cmaes https://github.com/resilient-swarms/meta-cmaes [QD-Meta and RHex robot]
- cvt-map-elites https://github.com/resilient-swarms/cvt_map_elites [CVT-MAP-Elites]
- AURORA https://github.com/adaptive-intelligent-robotics/AURORA [AURORA]


## Installation and running

For installation, we advise using singularity. To build singularity containers please see the singularity/ folder.

For running experiments, please see scripts/ and the run_* files in singularity/ for a range of examples.

The wscript files show all the binaries that can be generated. You can also modify the wscript files to setup your own experiments if needed. 




