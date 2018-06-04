#!/bin/bash
# nvsBinrInit_Jenkins.sh
# Finalized on 6/1/18
# Christopher Brant
# This script automates initialization of the RasPiGNSS module
# into binary nvs mode on a Raspberry Pi that already has
# all dependent software and modules installed

# initialize the NVS08C-CSM chipset
nvsctl -v init

# set nvsmode to binary
nvsmode -v binr

# reboot with erasing saved parameters
binrcmd -v COLDSTART

# select satellite systems (1 for GPS, 2 for GLONASS, defaults as both)
binrcmd -v SELGNSS 1

# bit information transmitted by satellites
binrcmd -v BITINF 1

# set navgation rate in Hz (1,2,5,10 Hz)
binrcmd -v NAVRATE 2

# differential correction SBAS w/ RTCA troposphere model
binrcmd -v DIFFCOR 2 1

# request pvt vector data
binrcmd -v PVT 1

# raw data output in intervals of dezi-secs (100ms)
# 1 -> 10Hz, 2 -> 5Hz, 5 -> 2Hz, 10 -> 1Hz (inverse
# setting but must be the same or greater than NAVRATE !!)
binrcmd -v RAWDATA 5

# reboot without erasing saved parameters
binrcmd -v WARMSTART

# After this just use the following to write the stream to an nvs file
# cat /dev/ttyAMA0 > binrfile.nvs