#!/bin/bash
# nvsConv.sh
# Finalized on 5/29/18
# Christopher Brant
# This bash script is used to automate
# the conversion of files from .nvs to .obs and .nav
# and remove possible whitespace lines in the .obs file

# todays date and time in UTC time
TODAY="$(date -u +%F)"

# .nvs file is the first argument
NVSFILE=$1
FILENAME="${NVSFILE%%.nvs}"
# .obs file name is the second
OBSFILE="$FILENAME.obs"
# .nav file name is the third
NAVFILE="$FILENAME.nav"
# name of the jenkins directory
CURDIR=$(pwd)
# name of current day's data directory
DAYDIR="${TODAY}-data"

$CURDIR/nvsData/convbin -r nvs -o $OBSFILE -n $NAVFILE -d $CURDIR/nvsData/$DAYDIR -v 2.10 -od -os $CURDIR/nvsData/$DAYDIR/$NVSFILE

# the next 3 commands ensure there are no white space lines in .obs file
sed '/^[ \t]*$/d' $CURDIR/nvsData/$DAYDIR/$OBSFILE > $CURDIR/nvsData/$DAYDIR/temp.obs

cat $CURDIR/nvsData/$DAYDIR/temp.obs > $CURDIR/nvsData/$DAYDIR/$OBSFILE

rm $CURDIR/nvsData/$DAYDIR/temp.obs