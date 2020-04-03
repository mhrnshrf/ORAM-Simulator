#!/bin/bash
#LOGNAME=$1
#cd /mnt/c/Users/mer140/Usimm-ORAM; 
#bin/usimm input/1channel.cfg input/comm2 > /mnt/c/Users/mer140/Google\ Drive/Backup/log/"$LOGNAME".txt
TRACEFILE=$1
echo $TRACEFILE
/mnt/e/Pitt/2020\ Spring/Usimm-ORAM/bin/usimm input/1channel.cfg /mnt/c/trace/$TRACEFILE