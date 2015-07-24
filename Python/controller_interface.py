DEFAULT_FPS = 30

class ControllerInterface():
	def __init__(self,thread):
		self.setFPS(DEFAULT_FPS);
		self.thread = thread
		self.is_connected = False
		self.running = False
	def tick(self):
		pass
	def start(self):
		pass
	def stop(self):
		pass
	def setFPS(self, FPS):
		self.FPS = FPS
		self.timeperframe = 1.0/FPS
	