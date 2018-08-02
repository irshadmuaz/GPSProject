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
acc = np.array([0,0,0])
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

has_ran = 0

with VMU931Parser(euler=True, accelerometer=True) as vp:

	while True:

		pkt = vp.parse()

		# Print any important status messages (if any)
		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			#ts_last = ts_next
			ts_okay, acc[0], acc[1], acc[2] = pkt

		        if isinstance(pkt, messages.Euler):
            if has_ran == 1:
                ts_old = ts_euler
            ts_euler, theta[0], theta[1], theta[2] = pkt
            if has_ran == 1:
                ts_diff = (ts_euler - ts_old) / 1000

                theta[0] *= 0.0174533
                theta[1] *= 0.0174533
                theta[2] *= 0.0174533

                acc[0] *= 9.81
                acc[1] *= 9.81
                acc[2] *= 9.81

                acc_i = np.dot(eulerAnglesToRotationMatrix(theta), acc)
                acc_i[2] -= 9.81

                velo[0] += ts_diff * acc_i[0]
                velo[1] += ts_diff * acc_i[1]
                velo[2] += ts_diff * acc_i[2]

            has_ran = 1
