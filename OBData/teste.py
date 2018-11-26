# !/usr/bin/env python
# -'''- coding: utf-8 -'''-

from pydbus import SessionBus
import time
import numpy as np

bus = SessionBus()
obdata = bus.get("local.OBData", "/OBData")

i = 0.0
x = 0.0

while(1):
	i = np.sin(np.pi * x)
	obdata.setRPM(500 *i + 1000)
	obdata.setSpeed(20 *i + 80)
	obdata.setFuelLevel(20 *i + 70)
	obdata.setIntakeTemp(20 *i + 40)
	obdata.setMAF(10*i + 10)
	obdata.setThrottlePos(20 *i + 40)
	obdata.setCoolantTemp(20 *i + 70)
	time.sleep(0.01)
	
	x = x + 0.01
