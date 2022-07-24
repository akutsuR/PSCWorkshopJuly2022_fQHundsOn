#!/bin/bash

source env_wcsim_fitqun.sh

DATA_SET_DIR=/disk1/disk02/usr6/rakutsu/hk/workshop_july2022/data_set
WCSIMFILE=$DATA_SET_DIR/wcsim/pdg13_E600MeV_wcsim.*.root
FITQUNFILE=$DATA_SET_DIR/fitqun/pdg13_E600MeV_fitqun.*.root
OUTFILE=out_analysis_particlegun_muon.root

./analysis_particlegun_muon "$WCSIMFILE" "$FITQUNFILE" $OUTFILE
