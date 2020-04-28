#!/bin/bash
TRACEFILE="$1"
LOGNAME="$2"
/mnt/e/Pitt/Spring\ 2020/Usimm-ORAM/bin/usimm input/1channel.cfg /mnt/c/trace/"$TRACEFILE"  > /mnt/e/Pitt/Spring\ 2020/Usimm-ORAM/log/"$LOGNAME.txt"
