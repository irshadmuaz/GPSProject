#!/usr/bin/env python3

import vmu931_driver as vmu
import dato

d = vmu.vmu931("/dev/ttyACM0")
d.setup()
d.isConnected()
d.StreamingAccelerometers()
d.StreamingGyroscopes()
d.StreamingHeading()
d.StreamingMagnetometers()
# Now calibrate
d.calibrate()
d.close()
print("\nCalibration finished\n")
