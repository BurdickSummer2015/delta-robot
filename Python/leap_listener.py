import Leap
from workspace import *
from controller_interface import *
import time

x = 0
y = 1
z = 2

class LeapController(ControllerInterface, Leap.Controller):
    def __init__(self, thread):
        ControllerInterface.__init__(self,thread)
        Leap.Controller.__init__(self)
        self.listener = LeapListener(thread, self)
        self.running = False
    def tick(self):
        time.sleep(0.5)
    def start(self):
        self.add_listener(self.listener)
        self.running = True
    def stop(self):
        self.remove_listener(self.listener)
        self.running = False



class LeapListener(Leap.Listener):
    def __init__(self, thread, controller):
        super(self.__class__, self).__init__()
        self.thread = thread
        self.controller = controller
        print("INIT", thread)

    def on_connect(self, controller):
        self.is_connected = True
        print "Connected"
        # self.thread.run();
    def on_frame(self, controller):
        # print "Frame available"
        if self.thread.handTrack:
            listenToHand(self.thread)
            if self.thread.record:
                if self.thread.posCount < self.thread.MAX_POS:
                    self.thread.positions.append(self.thread.currentPos)
                    self.thread.posCount += 1
    def on_disconnect(self, controller):
        # Note: not dispatched when running in a debugger.
        self.is_connected = False
        print "Disconnected"

def getFingerPos(leapController):
    """
    OBSOLTE -- Use getHandPos instead.
    Uses the Leap motion to get a raw value of the finger position in mm in the
    coordinate system of the device (x, y, z). If no finger is detected,
    returns (0, 0, 0).
    """
    if not leapController.is_connected:
        print "Connection lost."
        return (0, 0, 0)
    frame        = leapController.frame()
    fingers      = frame.fingers
    indexFingers = fingers.finger_type(Leap.Finger.TYPE_INDEX)
    indexFinger  = indexFingers[0]
    distalBone   = indexFinger.bone(Leap.Bone.TYPE_DISTAL)
    distalPos    = distalBone.next_joint
    
    return (distalPos.x, distalPos.y, distalPos.z)
    
def getHandPos(leapController):
    """
    Uses the Leap motion to get a raw value of the center of the palm in mm 
    in the coordinate system of the device (x, y, z). If no finger is detected,
    returns (0, 0, 0).
    """
    if not leapController.is_connected:
        print "Connection lost."
        return (0, 0, 0)
    frame        = leapController.frame() # Frame
    hands        = frame.hands            # HandList
    hand         = hands.frontmost        # Hand
    handPos      = hand.palm_position     # Vector
    return (handPos.x, handPos.y, handPos.z)

def listenToHand(thread):
    """
    Listens to hand position information and outputs it to the Delta.
    """
    posOut = thread.currentPos # Position to be outputted to move to.
                             # By default, output current position
                             # (don't move).
####        print "START:      " + str(self.currentPos)
    pos = getFingerPos(thread.controller)
####        print "FINGER:     " + str(fingerPos)
        
    if pos != (0.0, 0.0, 0.0): # Data from LeapMotion is good            
        desiredPos = transformPoint(pos) # Desired move position
####            print "DESIRED:    " + str(desiredPos)
        boundedPos = boundDestination(thread.currentPos, desiredPos)
####            print "BOUNDED:    " + str(boundedPos)
        restrainedPos = restrainMove(thread.currentPos, boundedPos)
            # Make move distance no more than 1" for mechanism safety.
####            print "RESTRAINED: " + str(restrainedPos
####            print "\n"
        posOut = restrainedPos

    # SEND 'posOut' TO THE ROBOT, if serial connected.
    if thread.serConnected:
        thread.outputPosition(posOut)

    thread.currentPos = posOut # Save position just sent to robot.