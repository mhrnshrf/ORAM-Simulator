#!/bin/bash

git config --global user.name mhrnshrf
git config --local user.email mhrnshrf@gmail.com

git config --global --add safe.directory '/mnt/c/Users/mehrn/OneDrive/Pitt/Year 7/Fall 2023/ORAM-Simulator'

COMMENT=$1
# Disable gnome-ssh-askpass
#unset SSH_ASKPASS;
# File to commit
git add -A;
# Now commit the change
git commit -m "$COMMENT";
# Push the commit to master branch
git push git@github.com:mhrnshrf/ORAM-Simulator.git
