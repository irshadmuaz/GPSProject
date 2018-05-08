#!/bin/bash
# Christopher Brant
# This bash script is used to automate
# the conversion of files from .ubx to .obs and .nav

# .ubx file is the first argument
UBXFILE=$1
FILENAME="${UBXFILE%%.ubx}"
# .obs file name is the second
OBSFILE="$FILENAME.obs"
# .nav file name is the third
NAVFILE="$FILENAME.nav"

./convbin -r ubx -o $OBSFILE -n $NAVFILE -d . -v 2.10 -od $UBXFILE
