#!/usr/bin/env python3
''' vmu_velocity.py
Purpose: Basic code for reading velocity from VMU931
IMPORTANT: Must be run using Python 3 (python3)
Authors of Main Code: Christopher Brant, David Lynge, and Heath Gerrald.
Last Modified: 7/20/2018
'''

## Import Libraries ##
import numpy as np
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
rot_matrix = np.matrix([[0,0,0],[0,0,0],[0,0,0]])
accel_matrix = np.matrix([[0],[0],[0]])

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
			ax_metric = ax * ACCEL_CONSTANT
			ay_metric = ay * ACCEL_CONSTANT
			az_metric = az * ACCEL_CONSTANT

			# value of acceleration measured
			accel_matrix = np.matrix([[ax_metric],[ay_metric],[az_metric]])

		if isinstance(pkt, messages.Euler):
			ets, ex, ey, ez = pkt

			# Calculate all 3 matrices before multiplying them together
			# Calculate D matrix with x value of eulers angles
			rx_22 = math.cos(ex)
			rx_23 = -(math.sin(ex))
			rx_32 = math.sin(ex)
			rx_33 = math.cos(ex)
			rx_matrix = np.matrix([[1, 0, 0], [0, rx_22, rx_23],[0, rx_32, rx_33]])
			# Calculate C matrix with y value of eulers angles
			ry_11 = math.cos(ey)
			ry_13 = math.sin(ey)
			ry_31 = -(math.sin(ey))
			ry_33 = math.cos(ey)
			ry_matrix = np.matrix([[ry_11, 0, ry_13],[0, 1, 0],[ry_31, 0, ry_33]])
			# Calculate B matrix with z value of eulers angles
			rz_11 = math.cos(ez)
			rz_12 = -(math.sin(ez))
			rz_21 = math.sin(ez)
			rz_22 = math.cos(ez)
			rz_matrix = np.matrix([[rz_11, rz_12, 0], [rz_21, rz_22, 0],[0, 0, 1]])

			rot_matrix = rz_matrix * ry_matrix * rx_matrix

		# gravitational matrix to add 
		grav_matrix = np.matrix([[0],[0],[ACCEL_CONSTANT]])

		rot_by_accel = rot_matrix * accel_matrix

		# Cancel out gravity
		new_accel_matrix = rot_by_accel + grav_matrix

		print("\n")
		print(new_accel_matrix)
		print("\n")

		# Calculate acceleration and velocity here based on the new ax,ay,az values
		#dt = (ts_next - ts_last) * TIME_CONSTANT
		#ax_metric = ax * ACCEL_CONSTANT
		#ay_metric = ay * ACCEL_CONSTANT
		#az_metric = az * ACCEL_CONSTANT
		#vx = (ax_metric * dt) + vx
		#vy = (ay_metric * dt) + vy
		#vz = (az_metric * dt) + vz



