#!/usr/bin/env python3


from pyvmu.vmu931 import VMU931Parser
from pyvmu import messages

f = open("mag_euler.txt", "w")
f.write("Mag X | Mag Y | Mag Z | Euler X | Euler Y | Euler Z | Time (sec)\n")

mag = [0, 0, 0]
euler = [0, 0, 0]
ts_mag = 0
ts_euler = 0

with VMU931Parser(euler=True, magnetometer=True) as vp:

    while True:
        
        pkt = vp.parse()

        # Print any important status messages (if any)
        if isinstance(pkt, messages.Status):
            print(pkt)

        if isinstance(pkt, messages.Magnetometer):
            ts_mag, mag[0], mag[1], mag[2] = pkt 

        if isinstance(pkt, messages.Euler):
            ts_euler, euler[0], euler[1], euler[2] = pkt 
            print("Euler X: %d\nEuler Y: %d\nEuler Z: %d"%(euler[0], euler[1], euler[2]))
            print("Mag X: %d\nMag Y: %d\nMag Z: %d"%(mag[0], mag[1], mag[2]))
            f.write("%6.1f  %6.1f  %6.1f %8.1f  %8.1f  %8.1f     %d\n"%(mag[0], mag[1], mag[2], euler[0], euler[1], euler[2], ts_euler))












