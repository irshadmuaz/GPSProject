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
# name of current day's data directory
DAYDIR="$(date -u +%F)-data"

~/GPSdopplar/nvsData/convbin -r nvs -o $OBSFILE -n $NAVFILE -d ~/GPSdopplar/nvsData/$DAYDIR -v 2.10 -od -os ~/GPSdopplar/nvsData/$DAYDIR/$NVSFILE

# the next 3 commands ensure there are no white space lines in .obs file
sed '/^[ \t]*$/d' ~/GPSdopplar/nvsData/$DAYDIR/$OBSFILE > ~/GPSdopplar/nvsData/$DAYDIR/temp.obs

cat ~/GPSdopplar/nvsData/$DAYDIR/temp.obs > ~/GPSdopplar/nvsData/$DAYDIR/$OBSFILE

rm ~/GPSdopplar/nvsData/$DAYDIR/temp.obs