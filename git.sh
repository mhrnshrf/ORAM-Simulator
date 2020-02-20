#!/bin/bash

git config --global user.email mhrnshrf@gmail.com
git config --local user.email mhrnshrf@gmail.com


COMMENT=$1
# Disable gnome-ssh-askpass
#unset SSH_ASKPASS;
# File to commit
git add -A;
# Now commit the change
git commit -m "$COMMENT";
# Push the commit to master branch
git push
