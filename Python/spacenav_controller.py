from controller_interface import *
from inertial_controller import *
import time
from ctypes import *
import os
import os_detect
from sys import platform as _platform
from workspace import *


# FPS = 30.0
# timeperframe = 1.0/FPS
# print(timeperframe, FPS)

CONTROLLER_MAX_TRANSLATION = 350.0

class SpacenavController(InertialController):
	def __init__(self, thread):
		InertialController.__init__(self,thread)
		print(os.path.dirname(__file__))
		# self.mouselib = CDLL("/Spacenavig-Library/spacenavig_python.so")
		self.mode = TRANSLATIONAL_MODE
		if os_detect.isUnix:
			self.mouselib = CDLL(os.path.abspath(os.path.dirname(__file__)+"/../Spacenavig-Library/Linux/spacenavig_python.so"))
		else:
			raise Error("Spacenav has not yet been implemented for" + _platform)

		self.fric_acc = 40 
		self.maxAcc = 5.0
		self.maxVel = 10.0

		self.readEvent = self.mouselib.readEvent
		self.initializeMouse = self.mouselib.init
		self.main = self.mouselib.main
		self.getMousePosition = self.mouselib.getMousePosition
		self.values = (c_float * 3)(1, 2, 3)
		self.values_ptr = pointer(self.values)

		self.initializeMouse()
		# print("INIT", thread)
		self.is_connected = True
		self.x = HOME[0]
		self.y = HOME[1]
		self.z = HOME[2]
	def setAcc(self,x,y,z):
		if(self.mode == INERTIAL_MODE):
			self.ax = x
			self.ay = y
			self.az = z
		elif(self.mode == TRANSLATIONAL_MODE):
			self.vx = x
			self.vy = y
			self.vz = z

	def tick(self):
		self.readEvent(c_double(time.time()))
		self.getMousePosition(self.values_ptr)
		l = list(self.values)
		# print(l)
		if(self.mode == INERTIAL_MODE):
			c = self.maxAcc/CONTROLLER_MAX_TRANSLATION
		elif(self.mode == TRANSLATIONAL_MODE):
			c = self.maxVel/CONTROLLER_MAX_TRANSLATION

		z = -l[2]*c
		threshold = 3
		if(z > -threshold and z < 0):
			z = 0
		elif(z < 0):
			z += threshold
		if(z < threshold and z > 0):
			z = 0
		elif(z > 0):
			z -= threshold
		self.setAcc(l[0]*c,l[1]*c,z*2)

		self.step(self.timeperframe)

		# bound2(z, Z_MIN, Z_MAX)
		# bound2(z, Z_MIN, Z_MAX)
		# bound2(z, Z_MIN, Z_MAX)

		desiredPos = (self.x, self.y, self.z)

		boundedPos = boundDestination(self.thread.currentPos, desiredPos)
####            print "BOUNDED:    " + str(boundedPos)
		restrainedPos = restrainMove(self.thread.currentPos, boundedPos)
			# Make move distance no more than 1" for mechanism safety.
####            print "RESTRAINED: " + str(restrainedPos
####            print "\n"
		posOut = restrainedPos

		self.x = posOut[0]
		self.y = posOut[1]
		self.z = posOut[2]

		# SEND 'posOut' TO THE ROBOT, if serial connected.
		if self.thread.serConnected:
			self.thread.outputPosition(posOut)

		self.thread.currentPos = posOut # Save position just sent to robot.
		# self.thread.outputPosition(posOut)

		time.sleep(self.timeperframe)
	def start(self):
		self.running = True
	def stop(self):
		self.running = False
	








	