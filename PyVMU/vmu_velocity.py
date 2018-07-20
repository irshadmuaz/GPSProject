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

# Beginning of Parser code
with VMU931Parser(accelerometer=True) as vp:

	vp.set_accelerometer_resolution(16) # Set resolution of accelerometer to 8g

	while True:
		pkt = vp.parse()

		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			ts, x, y, z = pkt
			ts_points.append(ts)
			x_points.append(x)
			y_points.append(y)
			z_points.append(z)