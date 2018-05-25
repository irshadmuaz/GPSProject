#!/bin/bash
# nvsConv.sh
# Finalized on 5/23/18
# Christopher Brant
# This bash script is used to automate
# the conversion of files from .nvs to .obs and .nav
# and remove possible whitespace lines in the .obs file

# .nvs file is the first argument
NVSFILE=$1
FILENAME="${NVSFILE%%.nvs}"
# .obs file name is the second
OBSFILE="$FILENAME.obs"
# .nav file name is the third
NAVFILE="$FILENAME.nav"

./convbin -r nvs -o $OBSFILE -n $NAVFILE -d . -v 2.10 -od -os $NVSFILE

sed '/^[ \t]*$/d' $OBSFILE > $OBSFILE