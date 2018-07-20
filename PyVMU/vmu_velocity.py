#!/usr/bin/env python3
''' vmu_velocity.py
Purpose: Basic code for reading velocity from VMU931
IMPORTANT: Must be run using Python 3 (python3)
Authors of Main Code: Christopher Brant, David Lynge, and Heath Gerrald.
Last Modified: 7/20/2018
'''

## Import Libraries ##
import serial
import time
import sys
import matplotlib.pyplot as pyplot
from pyvmu.vmu931 import VMU931Parser
from pyvmu import messages

# Declare variables
ats_next = 0
ax_next = 0
ay_next = 0
az_next = 0
vx = 0
vy = 0
vz = 0

# Beginning of Parser code
with VMU931Parser(accelerometer=True) as vp:

	vp.set_accelerometer_resolution(16) # Set resolution of accelerometer to 8g

	while True:
		pkt = vp.parse()

		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			ats_last = ats_next
			ax_last = ax_next
			ay_last = ay_next
			az_last = az_next
			ats_next, ax_next, ay_next, az_next = pkt
			adt = ats_next - ats_last
			adx = ax_next - ax_last
			ady = ay_next - ay_last
			adz = az_next - az_last


