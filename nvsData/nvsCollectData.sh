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

# make the current day's directory if not already created
mkdir -p $CURDIR/nvsData/$DAYDIR

# edit the timeout value to 20m when fully working
timeout 45m cat /dev/ttyAMA0 > $CURDIR/nvsData/$DAYDIR/$NVSFILE

# sleep for 15s to ensure the file has correctly been written fully
sleep 15s

# convert the nvs binary file and save the .obs and .nav files to the correct location
$CURDIR/nvsData/convbin -r nvs -o $OBSFILE -n $NAVFILE -d $CURDIR/nvsData/$DAYDIR -v 2.10 -od -os $CURDIR/nvsData/$DAYDIR/$NVSFILE

# remove whitespace lines in .obs file and write that to temp.obs
sed '/^[ \t]*$/d' $CURDIR/nvsData/$DAYDIR/$OBSFILE > $CURDIR/nvsData/$DAYDIR/temp.obs
# write the temp.obs file to the correct .obs file name
cat $CURDIR/nvsData/$DAYDIR/temp.obs > $CURDIR/nvsData/$DAYDIR/$OBSFILE
# remove the temp.obs file from the directory
rm $CURDIR/nvsData/$DAYDIR/temp.obs

# Enable git settings for the build to update git with
git config --global user.name "GPSdopplarBot"
git config --global user.email "gpsdopplar@gmail.com"