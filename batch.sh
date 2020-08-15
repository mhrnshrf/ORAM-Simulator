#!/bin/bash
CONFIGFILE="$1"
LOGNAME="$2"
git reset --hard HEAD; git pull; rm bin/usimm; cd src; make; cd ..; 
#bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/perlbench  > ../oram/log/"$LOGNAME-perlbench.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/gcc  > ../oram/log/"$LOGNAME-gcc.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/mcf  > ../oram/log/"$LOGNAME-mcf.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/omnetpp  > ../oram/log/"$LOGNAME-omnetpp.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/xalancbmk  > ../oram/log/"$LOGNAME-xalancbmk.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/x264  > ../oram/log/"$LOGNAME-x264.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/deepsjeng  > ../oram/log/"$LOGNAME-deepsjeng.txt" &
#bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/leela  > ../oram/log/"$LOGNAME-leela.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/bwaves  > ../oram/log/"$LOGNAME-bwaves.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/lbm  > ../oram/log/"$LOGNAME-lbm.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/wrf  > ../oram/log/"$LOGNAME-wrf.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/cam4  > ../oram/log/"$LOGNAME-cam4.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/imagick  > ../oram/log/"$LOGNAME-imagick.txt" &
#bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/nab  > ../oram/log/"$LOGNAME-nab.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/fotonik3d  > ../oram/log/"$LOGNAME-fotonik3d.txt" &
bin/usimm input/"$CONFIGFILE.cfg" ../oram/trace/roms  > ../oram/log/"$LOGNAME-roms.txt" 
