import serial
from ctypes import *
import time


def go():

    kinlib = cdll.LoadLibrary("../DeltaDriver/DeltaDriver/DeltaMSVS/x64/Release/deltakinematics.dll")
    getAngles = kinlib.getAngles
    pointValid = kinlib.pointValid
    lookupAngles = kinlib.lookupAngles
    INVERSE_TABLE = kinlib.INVERSE_TABLE
    # sd = list(kinlib.INVERSE_TABLE())
    # print(sd)
    class POINT(Structure):
        _fields_ = [("x", c_double),("y", c_double),("z", c_double)]
    point = POINT(1,2,-8)
    AngleSet = c_double*3
    angles = AngleSet(0.0,0.0,0.0)
    angles_ptr = pointer(angles)
    start = time.clock()
    for i in range(0, 1000000):
        if(pointValid(point)):
            # lookupAngles(INVERSE_TABLE,angles_ptr, point)
            getAngles(angles_ptr, point)
            # print list(angles)
        # else:
            # print("INVALID")
    print("DURATION FULL:", (1000.0/1000000.0)*(time.clock()-start))

    for i in range(0, 1000000):
        if(pointValid(point)):
            lookupAngles(INVERSE_TABLE,angles_ptr, point)
            # getAngles(angles_ptr, point)
            # print list(angles)
        # else:
            # print("INVALID")
    print("DURATION LOOKUP:", (1000.0/1000000.0)*(time.clock()-start))
    

    
    while (1):
        pass

    ser = serial.Serial()
    ser.baudrate = 57600
    
    RangeMin = 4 #less than 4 may lead to false connection... COM3, COM4 etc.
    RangeMax = 8

    for i in range(RangeMin,RangeMax+1):
        print i;
        ser.port =i # = COM port - 1
        try: 
            ser.open()
            time.sleep(0.1) # Wait for serial to open
            sserConnected = True
        except Exception as e:
            #print "Failed to connect to COM" + str(i+1)
            if(i == RangeMax):
                print "COULD NOT CONNECT OVER SERIAL: COM" + str(i+1) + "!"
                return
            else:
                pass
        else:
            print "Connected to Serial port COM" + str(i+1) + "..."
            break
    while(1):
        print("HI")


go()