#!/bin/bash
# navFetch.sh
# Finalized on 5/18/18
# Christopher Brant
# This script automatically downloads the file at the following url
# ftp://cddis.gsfc.nasa.gov/gnss/data/hourly/{YYYY}/{DOY}/{HR}/zeck{DOY}{&}.{YY}n.Z
# "&" variable is for the letter corresponding to the hour of the day.

# FULLYEAR is 4 digit year and ABRVYEAR is the 2 digit year
FULLYEAR=$(date -u +%Y)
ABRVYEAR=$(date -u +%y)

# DOY is the 3 digit day of the year
DOY=$(date -u +%j)

# HOUR is the UTC hour of the day in 24 hour time
HOUR=$(date -u +%H)
HOUR=$(($HOUR-3))

# The following lines are used to convert the hour to a letter value
CharValue="a"
AscValue=$(printf "%d" "'$CharValue")
HRascii=$(($AscValue+$HOUR))

# HRAL is the hour value as a letter from a-z
HRAL=$(echo $HRascii | awk '{printf("%c",$1)}')

URL="ftp://cddis.gsfc.nasa.gov/gnss/data/hourly/$FULLYEAR/$DOY/$HOUR/zeck$DOY$HRAL.$ABRVYEAR"
URL+="n.Z"

# Pull most recent data from git to RasPi
git pull

# Download most recent data to git directory
wget -P pullData/ $URL

# Push new data to git
# git add .
# git commit -m "Added nav data for $ABRVYEAR : $DOY : $HOUR"
# git push

# The following commands will be used to save the GPSdopplarBot git credentials on RasPi
# git config credential.helper store
# git push http://example.com/repo.git
# Username: <type your username>
# Password: <type your password>