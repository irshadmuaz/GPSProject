#!/bin/bash
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
HOUR=$(($HOUR-1))

# The following lines are used to convert the hour to a letter value
CharValue="a"
AscValue=$(printf "%d" "'$CharValue")
HRascii=$(($AscValue+$HOUR))

# HRAL is the hour value as a letter from a-z
HRAL=$(echo $HRascii | awk '{printf("%c",$1)}')

URL="ftp://cddis.gsfc.nasa.gov/gnss/data/hourly/$FULLYEAR/$DOY/$HOUR/zeck$DOY$HRAL.$ABRVYEAR"
URL+="n.Z"

# use wget to save this file to current directory or given directory
wget -P pullData/ $URL

