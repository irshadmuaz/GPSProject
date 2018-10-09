#!/usr/bin/env python
import sys

iFile = open(sys.argv[1], "r")
oFile = open("out.nmea", "w")

#print iFile.read()

prevTime = 0
prevLat = 0
prevLon = 0

for line in iFile:
	if (line[:6] == "$GNGGA"):
		oFile.write("$GPGGA," + line[7:])
	elif (line[:6] == "$GNRMC"):
		oFile.write("$GPRMC," + line[7:])


