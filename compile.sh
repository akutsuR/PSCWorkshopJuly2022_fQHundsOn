#!/bin/bash

source env_wcsim_fitqun.sh
echo $ROOTSYS
rm -f analysis_particlegun_muon
make analysis_particlegun_muon
