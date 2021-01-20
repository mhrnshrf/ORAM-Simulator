#!/bin/bash
#CONFIGILE="$1"
LOGNAME="$1"
cd ../ORAM-Simulator; rm obj/*; rm bin/usimm; git reset --hard HEAD; git pull; rm bin/usimm; cd src; make; cd ..; 
#bin/usimm input/"1channel.cfg" ../oram/trace/perlbench  $LOGNAME > ../oram/log/"$LOGNAME-perlbench.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/gcc  $LOGNAME > ../oram/log/"$LOGNAME-gcc.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/mcf  $LOGNAME > ../oram/log/"$LOGNAME-mcf.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/omnetpp  $LOGNAME > ../oram/log/"$LOGNAME-omnetpp.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/xalancbmk  $LOGNAME > ../oram/log/"$LOGNAME-xalancbmk.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/x264  $LOGNAME > ../oram/log/"$LOGNAME-x264.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/deepsjeng  $LOGNAME > ../oram/log/"$LOGNAME-deepsjeng.txt" &
#bin/usimm input/"1channel.cfg" ../oram/trace/leela  $LOGNAME > ../oram/log/"$LOGNAME-leela.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/bwaves  $LOGNAME > ../oram/log/"$LOGNAME-bwaves.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/lbm  $LOGNAME > ../oram/log/"$LOGNAME-lbm.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/wrf  $LOGNAME > ../oram/log/"$LOGNAME-wrf.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/cam4  $LOGNAME > ../oram/log/"$LOGNAME-cam4.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/imagick  $LOGNAME > ../oram/log/"$LOGNAME-imagick.txt" &
#bin/usimm input/"1channel.cfg" ../oram/trace/nab  $LOGNAME > ../oram/log/"$LOGNAME-nab.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/fotonik3d  $LOGNAME > ../oram/log/"$LOGNAME-fotonik3d.txt" &
bin/usimm input/"1channel.cfg" ../oram/trace/roms  $LOGNAME > ../oram/log/"$LOGNAME-roms.txt" ;
python log/merge.py $LOGNAME;
rm log/$LOGNAME-*.csv
