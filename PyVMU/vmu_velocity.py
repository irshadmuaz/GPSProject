#!/usr/bin/env python3
''' vmu_velocity.py
Purpose: Basic code for reading velocity from VMU931
IMPORTANT: Must be run using Python 3 (python3)
Authors of Main Code: Christopher Brant, David Lynge, and Heath Gerrald.
Last Modified: 7/20/2018
'''

## Import Libraries ##
import numpy
import math
import serial
import time
import sys
import dato
import vmu931_driver as vmu
from pyvmu.vmu931 import VMU931Parser
from pyvmu import messages

# Declare variables
ACCEL_CONSTANT = 9.81	# raw data comes in as G values
TIME_CONSTANT = 1/1000	# raw data comes in as milliseconds
ts_next = 0
ax = 0
ay = 0
az = 0
vx = 0
vy = 0
vz = 0

# Beginning of Parser code
with VMU931Parser(accelerometer=True, euler=True) as vp:

	vp.set_accelerometer_resolution(16) # Set resolution of accelerometer to 8g

	while True:
		pkt = vp.parse()

		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			ts_last = ts_next
			ts_next, ax, ay, az = pkt

		if isinstance(pkt, messages.Euler):
			ets, ex, ey, ez = pkt



		# Calculate acceleration and velocity here based on the new ax,ay,az values
		dt = (ts_next - ts_last) * TIME_CONSTANT
		ax_metric = ax * ACCEL_CONSTANT
		ay_metric = ay * ACCEL_CONSTANT
		az_metric = az * ACCEL_CONSTANT
		vx = (ax_metric * dt) + vx
		vy = (ay_metric * dt) + vy
		vz = (az_metric * dt) + vz



