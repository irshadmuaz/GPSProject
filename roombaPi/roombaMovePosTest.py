''' roombaMovePosTest.py
Purpose: Basic code for running Roomba, Xbee, and IMU
	Sets up Roomba, Xbee, and IMU and calibrates;
IMPORTANT: Must be run using Python 3 (python3)
Code Shell for setup written by Timothy Anglea.
Author of Main Code: Christopher Brant, assisted by Timothy Anglea.
Last Modified: 7/3/2018
'''
## Import libraries ##
import serial
import time
import RPi.GPIO as GPIO

import RoombaCI_lib
from RoombaCI_lib import DHTurn
from RoombaCI_lib import DDSpeed

import math
import random

## Variables and Constants ##
global Xbee # Specifies connection to Xbee
Xbee = serial.Serial('/dev/ttyUSB0', 115200) # Baud rate should be 115200
# LED pin numbers
yled = 5
rled = 6
gled = 13

# Roomba Constants
WHEEL_DIAMETER = 72 # millimeters
WHEEL_SEPARATION = 235 # millimeters
WHEEL_COUNTS = 508.8 # counts per revolution
DISTANCE_CONSTANT = (WHEEL_DIAMETER * math.pi)/(WHEEL_COUNTS) # millimeters/count
TURN_CONSTANT = (WHEEL_DIAMETER * 180)/(WHEEL_COUNTS * WHEEL_SEPARATION) # degrees/count

epsilon = 0.5 # smallest resolution of angle

# GPS/Absolute Positioning Variables #
# originAbs X,Y,Z are all absolute position points of the origin
originAbsX = sys.argv[1]
originAbsY = sys.argv[2]
originAbsZ = sys.argv[3]

## Functions and Definitions ##
''' Displays current date and time to the screen
	'''
def DisplayDateTime():
	# Month day, Year, Hour:Minute:Seconds
	date_time = time.strftime("%B %d, %Y, %H:%M:%S", time.gmtime())
	print("Program run: ", date_time)

## -- Code Starts Here -- ##
# Setup Code #
GPIO.setmode(GPIO.BCM) # Use BCM pin numbering for GPIO
DisplayDateTime() # Display current date and time

# LED Pin setup
GPIO.setup(yled, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(rled, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(gled, GPIO.OUT, initial=GPIO.LOW)

# Wake Up Roomba Sequence
GPIO.output(gled, GPIO.HIGH) # Turn on green LED to say we are alive
print(" Starting ROOMBA... ")
Roomba = RoombaCI_lib.Create_2("/dev/ttyS0", 115200)
Roomba.ddPin = 23 # Set Roomba dd pin number
GPIO.setup(Roomba.ddPin, GPIO.OUT, initial=GPIO.LOW)
Roomba.WakeUp(131) # Start up Roomba in Safe Mode
# 131 = Safe Mode; 132 = Full Mode (Be ready to catch it!)
Roomba.BlinkCleanLight() # Blink the Clean light on Roomba

if Roomba.Available() > 0: # If anything is in the Roomba receive buffer
	x = Roomba.DirectRead(Roomba.Available()) # Clear out Roomba boot-up info
	#print(x) # Include for debugging

print(" ROOMBA Setup Complete")
GPIO.output(yled, GPIO.HIGH) # Indicate within setup sequence
# Initialize IMU
print(" Starting IMU...")
imu = RoombaCI_lib.LSM9DS1_IMU() # Initialize IMU
time.sleep(0.5)
# Calibrate IMU
print(" Calibrating IMU...")
Roomba.Move(0,75) # Start Roomba spinning
imu.CalibrateMag() # Calculate magnetometer offset values
Roomba.Move(0,0) # Stop Roomba spinning
time.sleep(0.5)
imu.CalibrateAccelGyro() # Calculate accelerometer and gyroscope offset values
# Display offset values
# print("mx_offset = {:f}; my_offset = {:f}; mz_offset = {:f}".format(imu.mx_offset, imu.my_offset, imu.mz_offset))
# print("ax_offset = {:f}; ay_offset = {:f}; az_offset = {:f}".format(imu.ax_offset, imu.ay_offset, imu.az_offset))
# print("gx_offset = {:f}; gy_offset = {:f}; gz_offset = {:f}".format(imu.gx_offset, imu.gy_offset, imu.gz_offset))
print(" IMU Setup Complete")
time.sleep(1) # Gives time to read offset values before continuing
GPIO.output(yled, GPIO.LOW) # Indicate setup sequence is complete

if Xbee.inWaiting() > 0: # If anything is in the Xbee receive buffer
	x = Xbee.read(Xbee.inWaiting()).decode() # Clear out Xbee input buffer
	#print(x) # Include for debugging

# Intro Code #
print("This test is for Roomba Movement exclusively angular and distance-based.")
print("In this program, you will specify a heading and/or a distance.")

'''
# Create text file for data storage
data_name = input("File name for data? ")
data_name_string = data_name + ".txt" # Add the ".txt" to the end
datafile = open(data_name_string, "w") # Open a text file for storing data
# Write data header for the data file; includes name of each column
datafile.write("This file will contain the given starting GPS coordinates in ECEF.\n")
datafile.write("Along with calculated final position that the Roomba expects to be located at.\n")
'''

# Get initial angle from IMU
angle = imu.CalculateHeading()

#input the speed
spnspd = 100
speed_step = 20

#times, spin time is from formula
spinTime = (WHEEL_SEPARATION * math.pi) / (4 * spnspd)
backTime = 0.5
#initializes timers
moveHelper = (time.time() - (spinTime + backTime))

# Time to query for data
query_timer = 0.015 # seconds
# Initial conditions
distance = 0.0 # total distance travelled (millimeters)
x_pos = 0.0 # initial x-direction position (millimeters)
y_pos = 0.0 # initial y-direction position (millimeters)
forward_value = 0 # initial forward speed value (mm/s)
spin_value = 0 # initial spin speed value (mm/s)
# Initialization Music #
Roomba.PlayGoT()

# Main Code #
while True:
	try:
		# Read in initial wheel count values from Roomba
		bumper_byte, l_counts_current, r_counts_current, l_speed, r_speed, light_bumper = Roomba.Query(7, 43, 44, 42, 41, 45) # Read new wheel counts

		# Request for the desired angle to turn to
		# Print current angle of Roomba
		print("Current Location: ({0:.3f}, {1:.3f})".format(x_pos, y_pos))
		print("Current heading is {0:.3f}\n".format(angle))
		desired_heading = float(input("Desired heading? "))
		desired_distance = 0

		data_time = 0.0 # 0 seconds initial
		# Restart base timers
		base = time.time()
		query_base = time.time()
		# Setting the original spin value.
		spin_value = DHTurn(angle, desired_heading, epsilon)
		print("\nThe Roomba angle will now be changed.\n")

		# This while loop is for setting direction
		while spin_value != 0:
			try:
				# Have the roomba move to the desired direction to check
				Roomba.Move(forward_value, spin_value) # Spin the Roomba

				# This conditional checks for the new angle and distance changes
				if (time.time() - query_base) > query_timer: # Every (query_timer) seconds...
					bumper_byte, l_counts, r_counts, l_speed, r_speed, light_bumper = Roomba.Query(7, 43, 44, 42, 41, 45) # Read new wheel counts

					# Record the current time since the beginning of loop
					data_time = time.time() - base

					# Calculate the count differences and correct for overflow
					delta_l_count = (l_counts - l_counts_current)
					if delta_l_count > pow(2,15): # 2^15 is somewhat arbitrary
						delta_l_count -= pow(2,16)
					if delta_l_count < -pow(2,15): # 2^15 is somewhat arbitrary
						delta_l_count += pow(2,16)
					delta_r_count = (r_counts - r_counts_current)
					if delta_r_count > pow(2,15): # 2^15 is somewhat arbitrary
						delta_r_count -= pow(2,16)
					if delta_r_count < -pow(2,15): # 2^15 is somewhat arbitrary
						delta_r_count += pow(2,16)
					# Calculated the forward distance traveled since the last counts
					distance_change = DISTANCE_CONSTANT * (delta_l_count + delta_r_count) * 0.5
					# Calculated the turn angle change since the last counts
					angle_change = TURN_CONSTANT * (delta_l_count - delta_r_count)
					distance += distance_change # Updated distance of Roomba
					angle += angle_change # Update angle of Roomba and correct for overflow
					if angle >= 360 or angle < 0:
						angle = (angle % 360) # Normalize the angle value from [0,360)
					# Calculate position data
					delta_x_pos = distance_change * math.cos(math.radians(angle))
					delta_y_pos = distance_change * math.sin(math.radians(angle))
					x_pos += delta_x_pos
					y_pos += delta_y_pos

					# Set the spin value again
					spin_value = DHTurn(angle,desired_heading,epsilon)

					# Print out pertinent data values
					# print("{0:.5f}, {1:.3f}, {2:.3f}, {3:.3f}, {4:.3f};".format(data_time, desired_distance, angle, y_pos, x_pos))
					# Write data values to a text file
					# datafile.write("{0:.5f}, {1:.3f}, {2:.3f}, {3:.3f}, {4:.3f}, {5}, {6}, {7}, {8}, {9:.3f}, {10:.3f}, {11:0>8b}, {12:0>8b}\n".format(data_time, distance, desired_distance, angle, desired_heading, l_counts, r_counts, l_speed, r_speed, y_pos, x_pos, bumper_byte, light_bumper))

					# Update current wheel encoder counts
					l_counts_current = l_counts
					r_counts_current = r_counts
					# Reset base for query
					query_base += query_timer

			except KeyboardInterrupt:
				break  	# Break out of the loop early if necessary

		# Stop the Roomba's movement
		Roomba.Move(0,0)
		# Reset distance counter
		distance = 0

		print("Angle set.\n")
		print("The Roomba will now move the desired distance.\n")
		desired_distance=float(input("Desired distance? "))        

		# This while loop is for setting an moving a distance
		while distance < desired_distance:
			try:
				# Move Roomba to the desired distance
				Roomba.Move(forward_value, spin_value) # Spin the Roomba

				# This conditional checks for the new angle and distance changes
				if (time.time() - query_base) > query_timer: # Every (query_timer) seconds...
					bumper_byte, l_counts, r_counts, l_speed, r_speed, light_bumper = Roomba.Query(7, 43, 44, 42, 41, 45) # Read new wheel counts
					
					# Record the current time since the beginning of loop
					data_time = time.time() - base
					
					# Calculate the count differences and correct for overflow
					delta_l_count = (l_counts - l_counts_current)
					if delta_l_count > pow(2,15): # 2^15 is somewhat arbitrary
						delta_l_count -= pow(2,16)
					if delta_l_count < -pow(2,15): # 2^15 is somewhat arbitrary
						delta_l_count += pow(2,16)
					delta_r_count = (r_counts - r_counts_current)
					if delta_r_count > pow(2,15): # 2^15 is somewhat arbitrary
						delta_r_count -= pow(2,16)
					if delta_r_count < -pow(2,15): # 2^15 is somewhat arbitrary
						delta_r_count += pow(2,16)
					# Calculated the forward distance traveled since the last counts
					distance_change = DISTANCE_CONSTANT * (delta_l_count + delta_r_count) * 0.5
					# Calculated the turn angle change since the last counts
					angle_change = TURN_CONSTANT * (delta_l_count - delta_r_count)
					distance += distance_change # Updated distance of Roomba
					angle += angle_change # Update angle of Roomba and correct for overflow
					if angle >= 360 or angle < 0:
						angle = (angle % 360) # Normalize the angle value from [0,360)
					# Calculate position data
					delta_x_pos = distance_change * math.cos(math.radians(angle))
					delta_y_pos = distance_change * math.sin(math.radians(angle))
					x_pos += delta_x_pos
					y_pos += delta_y_pos

					# Set the spin value again
					spin_value = DHTurn(angle,desired_heading,epsilon)
					# If close, slow down the forward value
					to_travel = desired_distance - distance
					forward_value = DDSpeed(angle, desired_heading, to_travel)

					# Print out pertinent data values
					# print("{0:.5f}, {1:.3f}, {2:.3f}, {3:.3f}, {4:.3f};".format(data_time, to_travel, angle, y_pos, x_pos))
					# Write data values to a text file
					# datafile.write("{0:.5f}, {1:.3f}, {2:.3f}, {3:.3f}, {4:.3f}, {5}, {6}, {7}, {8}, {9:.3f}, {10:.3f}, {11:0>8b}, {12:0>8b}\n".format(data_time, distance, desired_distance, angle, desired_heading, l_counts, r_counts, l_speed, r_speed, y_pos, x_pos, bumper_byte, light_bumper))

					# Update current wheel encoder counts
					l_counts_current = l_counts
					r_counts_current = r_counts
					# Reset base for query
					query_base += query_timer

			except KeyboardInterrupt:
				break  	# Break out of the loop early if necessary

        # Stop the Roomba's movement
		Roomba.Move(0,0)
		print("\nRestarting movement process loop.\n")

	except KeyboardInterrupt:
		break	# Break out of the loop early if necessary

## -- Ending Code Starts Here -- ##
# Make sure this code runs to end the program cleanly
Roomba.PlayMarioDeath()
GPIO.output(gled, GPIO.LOW)
# datafile.close()

Roomba.ShutDown() # Shutdown Roomba serial connection
Xbee.close()
GPIO.cleanup() # Reset GPIO pins for next program
