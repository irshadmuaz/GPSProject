''' roombaMovePosTest.py
Purpose: Basic code for running Roomba, Xbee, and IMU
	Sets up Roomba, Xbee, and IMU and calibrates;
IMPORTANT: Must be run using Python 3 (python3)
Code Shell for setup written by Timothy Anglea.
Author of Main Code: Christopher Brant, assisted by Timothy Anglea.
Last Modified: 6/29/2018
'''
## Import libraries ##
import serial
import time
import RPi.GPIO as GPIO

import RoombaCI_lib

## Variables and Constants ##
global Xbee # Specifies connection to Xbee
Xbee = serial.Serial('/dev/ttyUSB0', 115200) # Baud rate should be 115200
# LED pin numbers
yled = 5
rled = 6
gled = 13

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
print("mx_offset = {:f}; my_offset = {:f}; mz_offset = {:f}".format(imu.mx_offset, imu.my_offset, imu.mz_offset))
print("ax_offset = {:f}; ay_offset = {:f}; az_offset = {:f}".format(imu.ax_offset, imu.ay_offset, imu.az_offset))
print("gx_offset = {:f}; gy_offset = {:f}; gz_offset = {:f}".format(imu.gx_offset, imu.gy_offset, imu.gz_offset))
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
forward_value = 0 # initial forward speed value (mm/s)
spin_value = 0 # initial spin speed value (mm/s)

# Initialization Music #
Roomba.PlayGoT()

# Main Code #
while True:
	try:
		# Read in initial wheel count values from Roomba
		bumper_byte, l_counts_current, r_counts_current, l_speed, r_speed, light_bumper = Roomba.Query(7, 43, 44, 42, 41, 45) # Read new wheel counts

		# Print the current angle/position of the Roomba




















	except KeyboardInterrupt:
		break	# Break out of the loop early if necessary





## -- Ending Code Starts Here -- ##
# Make sure this code runs to end the program cleanly
Roomba.PlayMarioDeath()

Roomba.ShutDown() # Shutdown Roomba serial connection
Xbee.close()
GPIO.cleanup() # Reset GPIO pins for next program
