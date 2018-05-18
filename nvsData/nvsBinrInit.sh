#!/bin/bash
# nvsBinrInit.sh
# Christopher Brant
# This script automates initialization of the RasPiGNSS module
# into binary nvs mode on a Raspberry Pi that already has
# all dependent software and modules installed

# initialize the NVS08C-CSM chipset
sudo nvsctl -v init

# set nvsmode to binary
sudo nvsmode -v binr

# reboot with erasing saved parameters
sudo binrcmd -v COLDSTART

# believe this turns on receiver channels
# sudo binrcmd -v RXCHANNELS 1

# bit information transmitted by satellites
# sudo binrcmd -v BITINF 1

# set navgation rate in Hz (1,2,5,10 Hz)
sudo binrcmd -v NAVRATE 2

# differential correction SBAS w/ RTCA troposphere model
sudo binrcmd -v DIFFCOR 2 1

# Assisted messages turned on (I think)
# sudo binrcmd -v ASSMSG 1

# raw data output in intervals of dezi-secs (100ms)
# 1 -> 10Hz, 2 -> 5Hz, 5 -> 2Hz, 10 -> 1Hz (inverse
# setting but must be the same or greater than NAVRATE !!)
sudo binrcmd -v RAWDATA 5

# reboot without erasing saved parameters
sudo binrcmd -v WARMSTART

# After this just use the following to write the stream to an nvs file
# cat /dev/ttyAMA0 > binrfile.nvs