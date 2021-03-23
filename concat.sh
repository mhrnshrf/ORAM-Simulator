#!/bin/bash
#CONFIGILE="$1"
LOGNAME="$1"
cd ../oram/log; 
python ../../ORAM-Simulator/merge-intermed.py $LOGNAME &&
rm $LOGNAME-*.csv
