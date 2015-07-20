
class ControllerInterface():
	def __init__(self,thread):
		self.thread = thread
		self.is_connected = False
		self.running = False
	def tick(self):
		pass
	def start(self):
		pass
	def stop(self):
		pass
	