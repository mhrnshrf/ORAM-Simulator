#!/bin/bash
#CONFIGILE="$1"
cd ../ORAM-Simulator; rm obj/*; rm bin/usimm; git reset --hard HEAD; git pull; rm bin/usimm; cd src; make; cd ..; 
