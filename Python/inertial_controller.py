from controller_interface import *
import time

# FPS = 30.0
# timeperframe = 1.0/FPS
INERTIAL_MODE = 1
TRANSLATIONAL_MODE = 2
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
		self.fric_acc = 40
		self.maxAcc = 5.0
		self.maxVel = 7.0
		# print("INIT", thread)
		self.is_connected = True
	def tick(self):
		self.step(self.timeperframe)
		time.sleep(self.timeperframe)
	def start(self):
		self.running = True
	def stop(self):
		self.running = False
	def step(self,t):
		self.ax = bound(self.ax, self.maxAcc)
		self.ay = bound(self.ay, self.maxAcc)
		self.az = bound(self.az, self.maxAcc)
		self.vx += self.ax *t;
		self.vy += self.ay *t;
		self.vz += self.az *t;
		self.vx = bound(self.vx, self.maxVel)
		self.vy = bound(self.vy, self.maxVel)
		self.vz = bound(self.vz, self.maxVel)

		print("ACC: ",self.ax, self.ay, self.az)
		print("VEL: ",self.vx, self.vy, self.vz)
		print("POS: ",self.x, self.y, self.z)
		if(self.ax == 0):
			self.vx = self.applyFric(self.vx,t)
		if(self.ay == 0):	
			self.vy = self.applyFric(self.vy,t)
		if(self.az == 0):	
			self.vz = self.applyFric(self.vz,t)

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

def bound(x, absMax):
	if(x > absMax):
		x = absMax
	if(x < -absMax):
		x = -absMax
	return x

def bound2(x, _min, _max):
	if(x > _max):
		x = _max
	if(x < _min):
		x = _min
	return x
		








	