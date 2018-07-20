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
import dato
import vmu931_driver as vmu
from pyvmu.vmu931 import VMU931Parser
from pyvmu import messages

def printValue (val):
	print "setTimestam = {} w = {} x = {} y = {} z = {} heading ={} msg ={}".format(val.timestamp, val.w, val.x, val.y, val.z, val.heading, val.msg)
	
d = vmu.vmu931("/dev/ttyUSB_VMU931")
d.connectToVMU931()
d.isConnected()
d.enableStreamingAccelerometers()
d.enableStreamingGyroscopes()
d.enableStreamingQuaternions()
d.enableStreamingHeading()
d.enableStreamingEulerAngles()
d.enableStreamingMagnetometers()
#for x in range(0,500000):
#	print x
#	d.readOneTime()
d.calibrate()
for x in range(0,200):
	print "-------------------------------"
	d.readOneTime()
	d.printAllValue()

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
with VMU931Parser(accelerometer=True) as vp:

	vp.set_accelerometer_resolution(16) # Set resolution of accelerometer to 8g

	while True:
		pkt = vp.parse()

		if isinstance(pkt, messages.Status):
			print(pkt)

		if isinstance(pkt, messages.Accelerometer):
			ts_last = ts_next
			ts_next, ax, ay, az = pkt
			dt = (ts_next - ts_last) * TIME_CONSTANT
			ax_metric = ax * ACCEL_CONSTANT
			ay_metric = ay * ACCEL_CONSTANT
			az_metric = az * ACCEL_CONSTANT
			vx = (ax_metric * dt) + vx
			vy = (ay_metric * dt) + vy
			vz = (az_metric * dt) + vz

			print("Time diff is {0:.3f} s, X acceleration is {1:.3f} m/s\n".format(dt, ax_metric))
			#print("Y velocity is {0:.3f}\n".format(vy))
			#print("Z velocity is {0:.3f}\n".format(vz))


