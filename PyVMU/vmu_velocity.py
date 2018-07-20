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
x_next = 0
y_next = 0
z_next = 0
x_velocity = 0
y_velocity = 0
z_velocity = 0

# Beginning of Parser code
with VMU931Parser(accelerometer=True) as vp:

	vp.set_accelerometer_resolution(16) # Set resolution of accelerometer to 8g

	while True:
		pkt = vp.parse()

		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			ts_last = ts_next
			x_last = x_next
			y_last = y_next
			z_last = z_next
			ts_next, x_next, y_next, z_next = pkt
			dt = ts_next - ts_last
			dx = x_next - x_last
			dy = y_next - y_last
			dz = z_next - z_last

			
