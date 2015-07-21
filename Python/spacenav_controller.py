from controller_interface import *
from inertial_controller import *
import time
from ctypes import *
import os
import os_detect
from sys import platform as _platform


FPS = 30.0
timeperframe = 1.0/FPS
# print(timeperframe, FPS)



class SpacenavController(InertialController):
	def __init__(self, thread):
		InertialController.__init__(self,thread)
		print(os.path.dirname(__file__))
		# self.mouselib = CDLL("/Spacenavig-Library/spacenavig_python.so")
		self.mode = INERTIAL_MODE
		if os_detect.isUnix:
			self.mouselib = CDLL(os.path.abspath(os.path.dirname(__file__)+"/../Spacenavig-Library/Linux/spacenavig_python.so"))
		else:
			raise Error("Spacenav has not yet been implemented for" + _platform)

		self.readEvent = self.mouselib.readEvent
		self.initializeMouse = self.mouselib.init
		self.main = self.mouselib.main
		self.getMousePosition = self.mouselib.getMousePosition
		self.values = (c_float * 3)(1, 2, 3)
		self.values_ptr = pointer(self.values)

		self.initializeMouse()
		# print("INIT", thread)
		self.is_connected = True
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

		self.setAcc(l[0],l[1],l[2])

		self.step(timeperframe)
		# self.thread.outputPosition(posOut)

		time.sleep(timeperframe)
	def start(self):
		self.running = True
	def stop(self):
		self.running = False
	








	