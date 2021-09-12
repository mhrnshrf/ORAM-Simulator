#!/bin/bash
LOGNAME="$1"
TRACEFILE="wrf"
#cd log;
#rm obj/*; rm bin/usimm; git reset --hard HEAD; git pull; rm bin/usimm; cd src; make; cd ..;
bin/usimm input/4channel.cfg /mnt/c/trace/"$TRACEFILE" $LOGNAME > log/"$LOGNAME-$TRACEFILE.txt"

