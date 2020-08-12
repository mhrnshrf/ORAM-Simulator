#!/bin/bash
CONFIGFILE="$1"
LOGNAME="$2"
bin/usimm input/"$CONFIGFILE.cfg" trace/perlbench  > log/"$LOGNAME-perlbench.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/gcc  > log/"$LOGNAME-gcc.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/mcf  > log/"$LOGNAME-mcf.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/omnetpp  > log/"$LOGNAME-omnetpp.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/xalancbmk  > log/"$LOGNAME-xalancbmk.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/x264  > log/"$LOGNAME-x264.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/deepsjeng  > log/"$LOGNAME-deepsjeng.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/leela  > log/"$LOGNAME-leela.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/bwaves  > log/"$LOGNAME-bwaves.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/lbm  > log/"$LOGNAME-lbm.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/wrf  > log/"$LOGNAME-wrf.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/cam4  > log/"$LOGNAME-cam4.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/imagick  > log/"$LOGNAME-imagick.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/nab  > log/"$LOGNAME-nab.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/fotonik3d  > log/"$LOGNAME-fotonik3d.txt" &
bin/usimm input/"$CONFIGFILE.cfg" trace/roms  > log/"$LOGNAME-roms.txt" 
