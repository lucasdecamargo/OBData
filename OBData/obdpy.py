# !/usr/bin/env python
# -'''- coding: utf-8 -'''-

from pydbus import SessionBus
import obd
import time


bus = SessionBus()
obdata = bus.get("local.OBData", "/OBData")

connection = obd.Async()

obdata.setConnectionStatus(connection.status())
obdata.setProtocolName(connection.protocol_name())
obdata.setPortName(connection.port_name())


def new_rpm(v):
    obdata.setRPM(v.value.magnitude)

def new_speed(v):
    obdata.setSpeed(v.value.magnitude)

def new_fuel_level(v):
	obdata.setFuelLevel(v.value.magnitude)

def new_intake_temp(v):
	obdata.setIntakeTemp(v.value.magnitude)
	
def new_maf(v):
	obdata.setMAF(v.value.magnitude)
	
def new_throttle_pos(v):
	obdata.setThrottlePos(v.value.magnitude)
	
def new_coolant_temp(v):
	obdata.setCoolantTemp(v.value.magnitude)

connection.watch(obd.commands.RPM, callback=new_rpm)
connection.watch(obd.commands.SPEED, callback=new_speed)
connection.watch(obd.commands.FUEL_LEVEL, callback=new_fuel_level)
connection.watch(obd.commands.INTAKE_TEMP, callback=new_intake_temp)
connection.watch(obd.commands.MAF, callback=new_maf)
connection.watch(obd.commands.THROTTLE_POS, callback=new_throttle_pos)
connection.watch(obd.commands.COOLANT_TEMP, callback=new_coolant_temp)

connection.start()

while(True):
	time.sleep(1)
