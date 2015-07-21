from workspace import *
from math import *
import time
import threading
import serial
import struct
# from leap_listener import *
from spacenav_controller import *
import os_detect
#from tween_controller import *

#
# Processes input from the Leap Motion and uses it to send data to a Delta
# Robot.
#


# Constants for mechanism control.

MAX_POS  = 10000 # Max number of positions to record

numAvgs = 1 #Best 25 # number of times to average average hand pos

sleepTime      = 0.001 # seconds # Best: 0.001

def floatToShort(x):
    """
    Takes a floating point number, muliplies it by 1000 and
    converts it to a 16-bit unsigned short
    """
    x = int(round(x * 1000))
    #Clamp x to -32767 to 32767 (i.e range of short)
    if(x > 32760):
        x = 32760
    if(x < -32760):
        x = -32760
    return x


class ControllerThread(threading.Thread):
    """
    This class provides a thread that, when enabled, gets hand data from the 
    leap motion, processes it, and sends it to the Delta Robot.
    """
    
    def __init__(self, threadName):
        """
        Creates a new ControllerThread named 'threadName'.
        """
        threading.Thread.__init__(self, name = threadName)       
        self.handTrack = True; # If controller should track
        self.record    = True; # If controller should record while tracking
        self.playback  = True; # If controller should playback after stopping
        self.paused = True      # indicates controler should stop
        self.replay = False    # indicates controller is should replay
        self.serConnected = False
        self.stopped = False
        self.MAX_POS = MAX_POS
        
        # Current estimated position of robot. 
        self.currentPos = HOME # Starts by default at home upon powerup.
        
        self.posCount = 0; # Number of positions recorded.
        self.positions = []

       # Serial object.
        self.ser = serial.Serial()
        self.ser.baudrate = 57600
        
        if(os_detect.isLinux):
            RangeMin = 0
            RangeMax = 8

            portPath = lambda x: "/dev/ttyUSB"+str(x)
            portName = portPath
        elif(os_detect.isWindows):
            RangeMin = 4 #less than 4 may lead to false connection... COM3, COM4 etc.
            RangeMax = 20

            portPath = lambda x:x
            portName = lambda x: "COM"+str(x)
        else:
            print "A Serial protocol has not yet been implemented for your operating system. Feel free to do so."
            return

        for i in range(RangeMin,RangeMax+1):
            # print i;
            self.ser.port = portPath(i) # = COM port - 1
            try: 
                self.ser.open()
                time.sleep(0.1) # Wait for serial to open
                self.serConnected = True
            except Exception as e:
                #print "Failed to connect to COM" + str(i+1)
                if(i == RangeMax):
                    print "COULD NOT CONNECT OVER SERIAL: " + str(portName(i)) + "!"
                    return
                else:
                    pass
            else:
                print "Connected to Serial port " +  str(portName(i)) + "..."
                break


            
        self.ser.timeout = .01
        

        # Leap Controller.
        # self.listener = LeapListener(self)
        self.controller = SpacenavController(self)
        self.controller.start()
        # self.leapController = Leap.Controller()
        time.sleep(0.4) # Wait for leap to connect
        if not self.controller.is_connected:
            print "LEAP MOTION IS NOT CONNECTED."
            
    def outputImage(self, img):
        """
        Outputs the image 'img', which consists of a list of tuples of the form
        (x, y, z).
        """
        for p in img:
            self.outputPosition(p)
            if self.paused:
                return
        self.outputPosition(HOME)
        print("'p' to playback - Enter to cancel")
            
    

    def outputPosition(self, p):
        """
        Outputs the position 'p' = (x, y, z) over the serial.
        """
        # First, generate the string to be outputted. 
        (x, y, z) = p

        # output = ("$" +
                  # str(int(round(x * 1000))) +
                  # "," +
                  # str(int(round(y * 1000))) +
                  # "," +                  
                  # str(int(round(z * 1000))) +
                  # "*y")
        output = struct.pack("h", floatToShort(x))
        output += struct.pack("h", floatToShort(y))
        output += struct.pack("h", floatToShort(z))
        # Write over the serial line.
##        print(output)
        self.ser.write(output)

        print "%.3f   %.3f   %.3f" % (p[0], p[1], p[2])
        # myStr = self.ser.read(100)
        # if(myStr != ""):
        #     print(myStr)

        # time.sleep(sleepTime)
    def run(self):
        while(self.stopped == False):
            if(self.controller):
                self.controller.tick()

        
    

    # def play(self):
    #     print("START")
    #     self.paused = False
    #     self.leapController.add_listener(self.listener)
    # def pause(self):
    #     print("PAUSE")
    #     self.paused = True
    #     self.leapController.remove_listener(self.listener)
    

