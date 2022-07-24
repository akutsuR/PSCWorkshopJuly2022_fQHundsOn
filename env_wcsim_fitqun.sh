#!/bin/bash

#--- ROOT
root_bin_dir=/usr/local/sklib_gcc8/root_v5.34.38_python2.7
source $root_bin_dir/bin/thisroot.sh

#--- WCSim & Geant4
g4_bin_dir=/usr/local/sklib_gcc8/geant4.10.05.p01/bin/
g4_install_dir=/usr/local/sklib_gcc8/geant4.10.05.p01/share/Geant4-10.5.1/geant4make/
source $g4_bin_dir/geant4.sh
source $g4_install_dir/geant4make.sh

export WCSIMDIR=/disk1/disk02/usr6/rakutsu/hk/workshop_july2022/WCSim/WCSim-build/ROOT5/nuPRISM/develop
export LD_LIBRARY_PATH=${WCSIMDIR}:$LD_LIBRARY_PATH

#--- fiTQun
export FITQUN_ROOT=/disk1/disk02/usr6/rakutsu/hk/workshop_july2022/fiTQun/fiTQun-6.0.1
