import time
import getch
from controller_thread import *

#
# Main function for the computer side of the delta robot. 
#
getch = getch._Getch()

def loop():
    """
    Keeps looping until user enters something besides a playback command.
    """

    while(1):
        message = getch()
        print(message)
        if message == 'q':
            exit()
        elif message == 'p':
            print ("PPPPPPPPPPPPPPPP")
            ct.replay = True
        elif(ct.paused == False):
            ct.pause()
        else:
            ct.play()

if __name__ == "__main__":                  
    ct = ControllerThread("ct")
    time.sleep(0.5)
    # if ct.leapController.is_connected:
    if ct.serConnected and ct.leapController.is_connected:
        ct.start()
        # ct.stop = True
        if ct.playback:
            loop()
        
