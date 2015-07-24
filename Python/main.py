import time
from getChar import _Getch
getch = _Getch()
from controller_thread import *
import sys



#
# Main function for the computer side of the delta robot. 
#
# getch = getch._Getch()

def loop():
    """
    Keeps looping until user enters something besides a playback command.
    """

    while(1):
        try:
            message = getch()
            pass
        except KeyboardInterrupt:
            print("KeyboardInterrupt")
            endProgram()
        # print("User Input:",message)
        if message == 'q':
            endProgram()
        elif message == 'p':
            print ("PPPPPPPPPPPPPPPP")
            ct.replay = True
        # elif(ct.paused == False):
        #     ct.controller.stop()
        # else:
        #     ct.play()
def endProgram():
    ct.stopped = True
    sys.exit(0);

if __name__ == "__main__":                  
    ct = ControllerThread("ct")
    time.sleep(0.5)
    # if ct.controller.is_connected:
    if ct.serConnected or ct.ignoreSerial and ct.controller.is_connected:
        ct.start()
        # ct.stop = True
        if ct.playback:
            loop()
        
