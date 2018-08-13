#!/usr/bin/env python3


from pyvmu.vmu931 import VMU931Parser
from pyvmu import messages
import math
import numpy as np


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




theta = [-178.761, 87.207, -29.288]

acc = [-10.019, -0.21, -0.49]

mag = [0, 0, 0]

has_ran_euler = 0
has_ran_mag = 0

vel = [0, 0, 0]

f = open("speed_imu.txt", "w")
f.write("Speed (m/s) | Heading (deg) | Time (sec)\n")

prevSec = 0

# Variables to calculate dynamic moving average for error
lstError = []
lstIndex = 0
lstSize = 300

with VMU931Parser(euler=True, accelerometer=True, magnetometer=True, heading=True) as vp:

    while True:
        
        pkt = vp.parse()

        # Print any important status messages (if any)
        if isinstance(pkt, messages.Status):
            print(pkt)

        if isinstance(pkt, messages.Accelerometer):
            ts_acc, acc[0], acc[1], acc[2] = pkt

        if isinstance(pkt, messages.Magnetometer):
            ts_mag, mag[0], mag[1], mag[2] = pkt
            has_ran_mag = 1

        if isinstance(pkt, messages.Heading):
            ts_h, h = pkt

        


        if isinstance(pkt, messages.Euler):
            if has_ran_euler == 1 and has_ran_mag:
                ts_old = ts_euler
            ts_euler, theta[0], theta[1], theta[2] = pkt
            if has_ran_euler == 1 and has_ran_mag:
                ts_diff = (ts_euler - ts_old) / 1000

                theta[0] *= 0.0174533
                theta[1] *= 0.0174533
                theta[2] *= 0.0174533

                acc[0] *= 9.81
                acc[1] *= 9.81
                acc[2] *= 9.81

                acc_i = np.dot(eulerAnglesToRotationMatrix(theta), acc)
                acc_i[2] -= 9.81

                vel[0] += ts_diff * acc_i[0]
                vel[1] += ts_diff * acc_i[1]
                vel[2] += ts_diff * acc_i[2]
                
                # Heading calculations
                # Calculate magnetometer direction and its error from heading
                m = math.atan2(mag[0], mag[1]) * 180 / math.pi
                error = h - m

                # Keep the error within 0-360 deg
                if error < 0:
                    error += 360
                elif error > 360:
                    error -= 360

                # Add to the moving average variables
                lstError.insert(0, error)
                if lstIndex < lstSize:
                    lstIndex += 1
                else:
                    lstError.pop(lstIndex)

                # Calculate real heading by subtracting the moving average of error
                dyn_h = h - sum(lstError) / float(len(lstError))

                if dyn_h > 360:
                    dyn_h -= 360
                elif dyn_h < 0:
                    dyn_h += 360
                #print("Dynamic-Corrected Direction: %d"%(dyn_h))
                print("Direction: %d"%(m))
                
                #print("%10f  %10f  %10f %10f"%(acc_i[0], acc_i[1], acc_i[2], ts_euler))
                # print("%10f  %10f  %10f"%(vel[0], vel[1], vel[2]))
                

                if ts_euler - prevSec >= 1000:
                    prevSec = ts_euler
                    f.write("%12f %14f %11d\n"%(np.linalg.norm(vel), dyn_h, ts_euler/1000))
            has_ran_euler = 1












