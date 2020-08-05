#!/bin/bash
TRACEFILE="$1"
LOGNAME="$2"
/mnt/e/Pitt/Summer\ 2020/ORAM-Simulator/bin/usimm input/1channel.cfg /mnt/c/trace/"$TRACEFILE"  > /mnt/e/Pitt/Summer\ 2020/ORAM-Simulator/log/"$LOGNAME-$TRACEFILE.txt"
