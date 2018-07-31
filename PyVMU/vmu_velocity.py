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
acc_bod = np.array([0,0,0])
velo = np.array([0,0,0])

# Calculates Rotation Matrix given euler angles.
def eulerAnglesToRotationMatrix(theta) :

	R_x = np.array([[1,         0,                  0                   ],
					[0,         math.cos(theta[0]), -math.sin(theta[0]) ],
					[0,         math.sin(theta[0]), math.cos(theta[0])  ]
					])

	R_y = np.array([[math.cos(theta[1]),    0,      math.sin(theta[1])  ],
					[0,                     1,      0                   ],
					[-math.sin(theta[1]),   0,      math.cos(theta[1])  ]
					])

	R_z = np.array([[math.cos(theta[2]),    -math.sin(theta[2]),    0],
					[math.sin(theta[2]),    math.cos(theta[2]),     0],
					[0,                     0,                      1]
					])

	R = np.dot(R_z, np.dot( R_y, R_x ))
 
	return R

# Hardcoded values are necessary
theta = [-178.761, 87.207, -29.288]

acc_bod = [-10.019, -0.21, -0.49]


with VMU931Parser(euler=True, accelerometer=True) as vp:

	while True:

		pkt = vp.parse()

		# Print any important status messages (if any)
		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			#ts_last = ts_next
			ts_next, acc_bod[0], acc_bod[1], acc_bod[2] = pkt

		if isinstance(pkt, messages.Euler):
			ts_last = ts_next
			ts_next, theta[0], theta[1], theta[2] = pkt

			theta[0] *= 0.0174533
			theta[1] *= 0.0174533
			theta[2] *= 0.0174533

			acc_bod[0] *= ACCEL_CONSTANT
			acc_bod[1] *= ACCEL_CONSTANT
			acc_bod[2] *= ACCEL_CONSTANT

			acc_inertial = np.dot(eulerAnglesToRotationMatrix(theta), acc_bod)
			acc_inertial[2] -= ACCEL_CONSTANT
			#print("Acceleration m/s^2 in X:{0:0.3f},Y:{1:0.3f},Z:{2:0.3f}".format(acc_inertial[0],acc_inertial[1],acc_inertial[2]))

			# Calculate acceleration and velocity here based on the new ax,ay,az values
			dt = (ts_next - ts_last) * TIME_CONSTANT
			velo[0] = (acc_inertial[0] * dt) + velo[0]
			velo[1] = (acc_inertial[1] * dt) + velo[1]
			velo[2] = (acc_inertial[2] * dt) + velo[2]
			#print("Velocity in m/s X:{0:0.3f},Y:{1:0.3f},Z:{2:0.3f}".format(velo[0],velo[1],velo[2]))
			print(dt)

