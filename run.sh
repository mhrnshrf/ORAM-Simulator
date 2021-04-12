#!/bin/bash
LOGNAME="$1"
TRACEFILE="$2"
cd ../ORAM-Simulator;
#rm obj/*; rm bin/usimm; git reset --hard HEAD; git pull; rm bin/usimm; cd src; make; cd ..;
bin/usimm input/4channel.cfg ../oram/trace/"$TRACEFILE" $LOGNAME > ../oram/log/"$LOGNAME-$TRACEFILE.txt"

