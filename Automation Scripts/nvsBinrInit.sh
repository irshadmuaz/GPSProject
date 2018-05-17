#!/bin/bash
# nvsBinrInit.sh
# Christopher Brant
# This script automates initialization of the RasPiGNSS module
# into binary nvs mode on a Raspberry Pi that already has
# all dependent software and modules installed

sudo nvsctl -v init

sudo nvsmode -v binr

# reboot without erasing saved parameters
sudo binrcommand -v WARMSTART
sudo binrcommand -v WAIT 200

# set navgation rate in Hz (1,2,5,10 Hz)
sudo binrcommand -v NAVRATE 2
sudo binrcommand -v WAIT 200

# differential correction SBAS w/ RTCA troposphere model
sudo binrcommand -v DIFFCOR 2 1
sudo binrcommand -v WAIT 1000

# raw data output in intervals of dezi-secs (100ms)
# 1 -> 10Hz, 2 -> 5Hz, 5 -> 2Hz, 10 -> 1Hz (inverse
# setting but must be the same or greater than NAVRATE !!)
sudo binrcommand -v RAWDATA 5
sudo binrcommand -v WAIT 200

# bit information transmitted by satellites
sudo binrcommand -v BITINF 1
sudo binrcommand -v WAIT 200

# reboot without erasing saved parameters
sudo binrcommand -v WARMSTART
sudo binrcommand -v WAIT 200

# After this just use the following to write the stream to an nvs file
# cat /dev/ttyAMA0 > binrfile.nvs