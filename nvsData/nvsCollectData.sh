#!/bin/bash
# nvsCollectData.sh
# Finalized on 5/30/18
# Christopher Brant

# todays date and time in UTC time
TODAY="$(date -u +%F)"
HOUR="$(date -u +%H)"
MINUTE="$(date -u +%M)"

# .nvs file is the first argument
NVSFILE=$1
FILENAME="${NVSFILE%%.nvs}"
# .obs file name is the second
OBSFILE="$FILENAME.obs"
# .nav file name is the third
NAVFILE="$FILENAME.nav"
# name of current day's data directory
DAYDIR="${TODAY}-data"

~/GPSdopplar/nvsData/convbin -r nvs -o $OBSFILE -n $NAVFILE -d ~/GPSdopplar/nvsData/$DAYDIR -v 2.10 -od -os ~/GPSdopplar/nvsData/$DAYDIR/$NVSFILE

# the next 3 commands ensure there are no white space lines in .obs file
sed '/^[ \t]*$/d' ~/GPSdopplar/nvsData/$DAYDIR/$OBSFILE > ~/GPSdopplar/nvsData/$DAYDIR/temp.obs

cat ~/GPSdopplar/nvsData/$DAYDIR/temp.obs > ~/GPSdopplar/nvsData/$DAYDIR/$OBSFILE

rm ~/GPSdopplar/nvsData/$DAYDIR/temp.obs