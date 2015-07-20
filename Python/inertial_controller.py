from controller_interface import *
import time

FPS = 30.0
timeperframe = 1.0/FPS
# print(timeperframe, FPS)

class InertialController(ControllerInterface):
	def __init__(self, thread):
		ControllerInterface.__init__(self,thread)
		self.x = 0.0
		self.y = 0.0
		self.z = 0.0
		self.vx = 0.0
		self.vy = 0.0
		self.vz = 0.0
		self.ax = 0.0
		self.ay = 0.0
		self.az = 0.0
		self.fric_acc = 10.0
		# print("INIT", thread)
		self.is_connected = True
	def tick(self):
		self.step(timeperframe)
		time.sleep(timeperframe)
	def start(self):
		self.running = True
	def stop(self):
		self.running = False
	def step(self,t):
		self.vx += self.ax *t;
		self.vy += self.ay *t;
		self.vz += self.az *t;
		print(self.vx, self.vy, self.vz, timeperframe)
		if(self.ax == 0):
			self.vx = self.applyFric(self.vx,t)
		if(self.ay == 0):	
			self.vy = self.applyFric(self.vy,t)
		if(self.az == 0):	
			self.vz = self.applyFric(self.vy,t)
		self.x +=self.vx *t;
		self.y +=self.vy *t;
		self.z +=self.vz *t;
		

		
	def applyFric(self,v,t):
		if(v > 0.0):
			v -= self.fric_acc*t
			if(v < 0.0):
				v=0.0
		elif(v < 0.0):
			v += self.fric_acc*t
			if(v > 0.0):
				v=0.0
		return v
		








	