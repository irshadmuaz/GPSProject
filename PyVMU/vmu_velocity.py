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
ts_next = 0
ax = 0
ay = 0
az = 0
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
			ts_last = ts_next
			ts_next, ax, ay, az = pkt
			dt = ts_next - ts_last
			vx = (ax * dt) + vx
			vy = (ay * dt) + vy
			vz = (az * dt) + vz

			print("X velocity is {0:.3f}\n".format(vx))
			print("Y velocity is {0:.3f}\n".format(vy))
			print("Z velocity is {0:.3f}\n".format(vz))


