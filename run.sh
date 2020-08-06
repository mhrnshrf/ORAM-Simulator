#!/bin/bash
TRACEFILE="$2"
LOGNAME="$1"
/mnt/e/Pitt/Summer\ 2020/ORAM-Simulator/bin/usimm input/1channel.cfg /mnt/c/trace/"$TRACEFILE"  > /mnt/e/Pitt/Summer\ 2020/ORAM-Simulator/log/"$LOGNAME-$TRACEFILE.txt"
