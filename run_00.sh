#!/bin/bash

source env_wcsim_fitqun.sh

DATA_SET_DIR=/disk1/disk02/usr6/rakutsu/hk/workshop_july2022/data_set
INFILE=$DATA_SET_DIR/wcsim/pdg13_E600MeV_wcsim.0000.root
FQCARD=`pwd`/Official_HyperK_HybridmPMT4Workshop.parameters.dat
OUTFILE=pdg13_E600MeV_fitqun.0000.root

$FITQUN_ROOT/runfiTQunWC $INFILE\
                         -r $OUTFILE\
                         -p $FQCARD\
                         -s 0\
                         -n 1
