#!/bin/bash
# ubxCollectData_TimeParam.sh
# Finalized on 6/12/18
# Christopher Brant

# Time length of test from command line including the minutes/hours/days
# letter after the number for the parameter
TIMEOUT=$1

# todays date and time in UTC time
TODAY="$(date -u +%F)"
HOUR="$(date -u +%H)"
MINUTE="$(date -u +%M)"

# create the filename for the nvs file
FILENAME="$TODAY-$HOUR$MINUTE"
# .obs file name is the second
OBSFILE="$FILENAME.obs"
# .nav file name is the third
NAVFILE="$FILENAME.nav"
# name of the jenkins directory
CURDIR=$(pwd)
# name of current day's data directory
DAYDIR="${TODAY}-data"

# make the current day's directory if not already created
mkdir -p $CURDIR/ubxData/$DAYDIR

# the timeout value will be a parameterized value so that
# it can be tested at different time amounts
timeout $TIMEOUT $CURDIR/ublox_muaz/bin/record_pseudoranges /dev/ttyACM0 9600

cp $CURDIR/ublox_muaz/bin/range_data.log_doppler $CURDIR/ubxData/$DAYDIR

# Enable git settings for the build to update git with
git config --global user.name "GPSdopplarBot"
git config --global user.email "gpsdopplar@gmail.com"
