from controller_interface import *
import time
import numpy.linalg

# FPS = 30
# timeperframe = 1/FPS

class TweenController(ControllerInterface):
	def __init__(self, thread):
		ControllerInterface.__init__(self,thread)
		# print("INIT", thread)
		self.prevtime = 0
		self.is_connected = True
		print(solvePolynomial(2, [1,2,2,2]))
	def tick(self):
		if(time.clock() > self.prevtime + self.timeperframe):
			posOut = (self.prevtime, self.prevtime*2, self.prevtime*4)
			# self.thread.outputPosition(posOut)
			#print "%.3f   %.3f   %.3f" % (posOut[0], posOut[1], posOut[2])
			self.prevtime = time.clock()
	def start(self):
		self.running = True
	def stop(self):
		self.running = False
class Tween():
	def __init__(self):
		self.eq = "?"
		self.len = "?"
		self.speed = "?"
	def solve(self, t):
		pass



def solvePolynomial(x_in, coeff):
	F_x = coeff[0]
	x = x_in
	for i in range(1,len(coeff)):
		print(i)
		F_x += coeff[i] * x
		x *= x_in
	return F_x


twoPI = Math.PI*2

def getCoeffSetFromPoints(points)
			X = []
			if (len(points) == 2):
				p1 = points[0]
				p2 = points[1]
				# var p1X:Number = (p1.x + transformX0) * transformX1 
				# var p2X:Number = (p2.x + transformX0) * transformX1 
				# var p1Y:Number = (p1.y + p1.phase * (_upperBound - _lowerBound)+ transformY0) * transformY1
				# var p2Y:Number = (p2.y + p2.phase * (_upperBound - _lowerBound)+ transformY0) * transformY1
				cB = (p1Y - p2Y) / (p1X- p2X)
				cA = cB*(-p1X) + p1Y
				X[0] = cA
				X[1] = cB
				
				return X
			}else {
				L = len(points)
				A = []
				B = []
				AL = 0
				for i in range(0,L):
					p = points[i]
					#ar _x:Number = (p.x + transformX0) * transformX1
					#//var _y:Number = p.y
					_x0 = p.x
					A[AL++] = 1
					for j in range(1,L):
						A[AL++] = _x
						_x *= _x0
					}
					B[i] = (p.y)
				}
				
				X = linalg.solve(A, B)
				return X
			}
			
			vec[0] = 1
			vec[1] = 1
			return X
			
		}


	