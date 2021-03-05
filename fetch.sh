#!/bin/bash
#CONFIGILE="$1"
LOGNAME="$1"
cd ../ORAM-Simulator; rm obj/*; rm bin/usimm; git reset --hard HEAD; git pull; rm bin/usimm; rm obj/*; cd src; make; # cd ..; 
#bin/usimm input/"1channel.cfg" ../oram/trace/perlbench  $LOGNAME > ../oram/log/"$LOGNAME-perlbench.txt" &
