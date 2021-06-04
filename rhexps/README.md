# MAP-Elites Rhex Experiments
### Dependencies

#### Ubuntu 16.04 
This guide was tested on 16.04, and is found to throw less problems than version 18. You may encounter other issues in using version 18.

#### GCC
Recommend using gcc/gpp version 5 for convenience sake. See the following guide for installing and switching compilers: http://tuxamito.com/wiki/index.php/Installing_newer_GCC_versions_in_Ubuntu

#### Setting up the ResiBots environment

We use the environmental `RESIBOTS_DIR` variable for easy configuration and library finding (it basically points to one folder where every local installation happens that is related to the project). Thus, before installing/compiling anything, make sure that you add this line to your
`~/.bashrc` or `~/.zshrc` file:

```bash
export RESIBOTS_DIR=/path/to/installation/folder 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/usr/lib:/usr/local/lib
```

From now on, we assume that the `RESIBOTS_DIR` variable is set.

#### Installing DART

To simulate our rhex we are using the [DART] simulator. To install use the following procedure:

```bash
sudo apt-add-repository ppa:libccd-debs/ppa # for ubuntu versions < 16
sudo apt-add-repository ppa:fcl-debs/ppa    # for ubuntu versions < 16
sudo apt-add-repository ppa:dartsim/ppa
sudo apt-get update

sudo apt-get install build-essential cmake pkg-config git
sudo apt-get install libeigen3-dev libassimp-dev libccd-dev libfcl-dev libboost-regex-dev libboost-system-dev libode-dev
sudo apt-get install libopenscenegraph-dev
sudo apt-get install libxmu-dev libxi-dev
sudo apt-get install freeglut3-dev

sudo apt-get install libtinyxml-dev libtinyxml2-dev
sudo apt-get install liburdfdom-dev liburdfdom-headers-dev


cd /path/to/tmp/folder
git clone git://github.com/dartsim/dart.git
cd dart
git checkout v6.3.0

mkdir build
cd build
cmake -DDART_ENABLE_SIMD=ON ..
make -j4
sudo make install
```

#### Installing the rhex common files

In order to simulate the rhex you nead to get the URDF file and the controller library:

```bash
cd /path/to/tmp/folder
git clone https://github.com/AranBSmith/rhex_common.git
cd rhex_common/rhex_models
./waf configure --prefix=$RESIBOTS_DIR
./waf install
cd ../rhex_controller
./waf configure --prefix=$RESIBOTS_DIR
./waf
./waf install
```

#### Installing the DART wrapper

To facilitate the simulation we have created a simple wrapper over the DART simulator that is specific to our rhex robot:

```bash
cd /path/to/tmp/folder
git clone https://github.com/AranBSmith/rhex_simu.git
cd rhex_simu/rhex_dart
./waf configure --prefix=$RESIBOTS_DIR
./waf
./waf install
```

#### Installing sferes's dependencies

```bash
sudo apt-get update
sudo apt-get install libboost-all-dev
sudo apt-get install libeigen3-dev libboost-serialization-dev libboost-filesystem-dev libboost-test-dev libboost-program-options-dev libboost-thread-dev libboost-regex-dev libboost-graph-dev
sudo apt-get install libtbb-dev
```

### Compiling the experiment

- Get [Sferes2]: `git clone https://github.com/sferes2/sferes2.git`
- Go to your `sferes2` root directory
- Go to the modules folder: `cd modules`
- Get the map elites module: `git clone https://github.com/resilient-swarms/map_elites.git`
  - Add `map_elites` to your modules.conf file in the sferes folder 
- Go back to your `sferes` directory
- Configure for compilation: `./waf configure`
    - You can add other options if you want, like: `--cpp14=yes` to force C++14 compilation
- Compile sferes2 with: `./waf`
- Create an experiment folder (if there's none) and cd to it: `mkdir exp && cd exp`
- Clone map_elites_rhex_v2: `git clone https://github.com/AranBSmith/MAP-Elites-Rhex.git`
- Go back to your `sferes2` root directory
- Configure the experiment: `./waf configure --exp MAP-Elites-Rhex` 
- Compile the experiment: `./waf --exp MAP-Elites-Rhex`

## How to run

- Compile the experiment (as shown above)
- Run it (assuming you are on sferes2 root dir and the `RESIBOTS_DIR` folder is set properly):
    - `./build/exp/MAP-Elites-Rhex/rhex_text` for text archives
    - `./build/exp/MAP-Elites-Rhex/rhex_binary` for binary archives

## Aknowledgement
[Roman Buckley MEng] for initial work on the Rhex skeleton, DART model, and controller. 

## LICENSE

[CeCILL]

[CeCILL]: http://www.cecill.info/index.en.html
[DART]: https://github.com/dartsim/dart
[Sferes2]: https://github.com/sferes2/sferes2
[Roman Buckley MEng]: https://github.com/rb10g16
