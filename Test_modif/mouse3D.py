from workspace import *
from math import *
import time
import threading
import serial
import struct
from ctypes import *

#Import the dynamic library spacenavig_python.so to get the motion from the 3D spacenavigator
mouse = CDLL("/home/me/Documents/Delta Robot/Test_modif/spacenavig_python.so")

#-------------------------------------------------------------------------------------------------------

#Declaration of the foncions from spacenavig_python.so
readEvent = mouse.readEvent
init = mouse.init
main = mouse.main
getMousePosition = mouse.getMousePosition
print(getMousePosition)

#--------------------------------------------------------------------------------------------------------

#Test of the good importation of the fonction from spacenavig_python.so
#print(readEvent)
#print(init)
#print(main)

#---------------------------------------------------------------------------------------------------------


#Test de sortie de readevent
values = (c_float * 3)(1, 2, 3)
p = pointer(values)

init() 
while(1):
	readEvent()
	getMousePosition(p)
	print(list(values))

print(init)


#----------------------------------------------------------------------------------------------------------


