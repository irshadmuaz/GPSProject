#!/bin/bash
# nvsCollectData.sh
# Finalized on 5/30/18
# Christopher Brant

# todays date and time in UTC time
TODAY="$(date -u +%F)"
HOUR="$(date -u +%H)"
MINUTE="$(date -u +%M)"

# create the filename for the nvs file
FILENAME="$TODAY-$HOUR$MINUTE"
# .nvs file is the first argument
NVSFILE="$FILENAME.nvs"
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