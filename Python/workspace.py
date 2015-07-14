from math import *

#
# Represents the workspace boundaries of a Delta Mechanism.
#

# 
# The workspace boundary for the endpoint of the mechanism is a cylinder of 
# radius R_MAX from Z_MIN to Z_MAX. However, the pen (which extends a distance 
# PEN_DIST below the endpoint of the mechanism) cannot travel below Z_PAPER. 
# When the pen is between Z_PAPER and Z_CLAMP, it is limited to the xy rectangle
# X : [X_RCT_MIN, X_RCT_MAX], Y : [Y_RCT_MIN, Y_RCT_MAX].
#


# Everything is 0.01 less than the values on the motor controller to avoid 
# rounding error issues. 
R_MAX      =   6.00          # Max radius of cylinder in inches.
Z_MIN      = -13.00         # Lowest height of cylinder in inches.
Z_MAX      =  -8.00          # Maximum height of cylinder in inches.
X_RCT_MIN  =  -4.50          # x-min of rectangular prism for the paper.
X_RCT_MAX  =   4.50          # x-max of rectangular prism for the paper.
Y_RCT_MIN  =  -3.75          # y-min of rectangular prism for the paper.
Y_RCT_MAX  =   3.75          # y-max of rectangular prism for the paper.
Z_PAPER    = -14.50          # Z location of the paper.
Z_CLAMP    = -13.50          # Z location of the clamps.
PEN_DIST   =   2.00          # Portrusion of pen below bottom face of bottom
                             # triangle in inches. Must be >= 1.50, or Z_MIN
                             # limit will prevent pen from touching paper. Must
                             # be <= 5.50, or Z_MAX limit will prevent pen from
                             # rising above paper.
HOME       = (0, 0, -8.5064) # Home position of mechanism.

restrainThresh = 0.15 # inches  # Best: 0.15
restrainDist   = 0.15 # inches  # Best: 0.15


xOffset = 0.0
yOffset = 0.0
zOffset = -20.0

                # Last best:
xScale = -1.0   # 1.0   
yScale = -1.0   # 1.0  
zScale =  1.0   # 1.0  

x = 0
y = 1
z = 2

def inCircle(p):
    """
    Returns True if the point 'p' = (x, y, z) when projected on the xy-plane 
    falls in a cylinder of radius R_MAX about the origin.
    """
    (x, y, z) = p
    return x ** 2 + y ** 2 <= R_MAX ** 2

def inRectangle(p):
    """
    Returns True if the point 'p' = (x, y, z) when project on the xy-plane falls
    in a rectangle  X : [X_RCT_MIN, X_RCT_MAX], Y : [Y_RCT_MIN, Y_RCT_MAX].
    """
    (x, y, z) = p
    return (x >= X_RCT_MIN and x <= X_RCT_MAX and
            y >= Y_RCT_MIN and y <= Y_RCT_MAX)

def pointValid(p):
    """
    Returns True if the point 'p' = (x, y, z) is in the cylinder of the
    workspace. 
    """
    (x, y, z) = p
    return (inCircle(p) and z >= Z_MIN and z <= Z_MAX)

def penPointValid(p):
    """
    Returns True if the point 'p' is valid AND the pen location is valid.
    """
    (x, y, z) = p
    zPen = z - PEN_DIST
    return (pointValid(p) and (zPen >= Z_CLAMP or penInPrism(p)))


def penInCylinder(p):
    """
    Returns True if the point 'p' = (x, y, z) causes the pen to be within the 
    cylinder of the workspace.
    """
    (x, y, z) = p
    zPen = z - PEN_DIST
    return (pointValid(p) and zPen >= Z_CLAMP)

def penInPrism(p):
    """
    Returns True if the point 'p' = (x, y, z) causes the pen to be within the
    rectangular prism around the paper.
    """
    (x, y, z) = p
    zPen = z - PEN_DIST
    return (pointValid(p) and inRectangle(p) and zPen >= Z_PAPER 
            and zPen <= Z_CLAMP)

def boundByCircle(p):
    """
    Returns the (x, y, z) value closest to the specified point 'p' = (x, y, z)
    that, when projected on the xy-plane, lies within the circle in the circle 
    of radius R_MAX about the origin.
    """
    (x, y, z) = p
    if inCircle(p):
        return p
    else: # If outside circle:
        theta = atan2(y, x)
        return (R_MAX * cos(theta), R_MAX * sin(theta), z)
    
def boundByRectangle(p):
    """
    Returns the (x, y, z) value closest to the specified point 'p' = (x, y, z) 
    that, when projected onto the xy-plane, lies within the rectangle 
    X : [X_RCT_MIN, X_RCT_MAX], Y : [Y_RCT_MIN, Y_RCT_MAX].
    """
    (x, y, z) = p
    if inRectangle(p):
        return p
    else: # if outside rectangle
        xOut = x
        yOut = y
        zOut = z
        if xOut < X_RCT_MIN:
            xOut = X_RCT_MIN
        elif xOut > X_RCT_MAX:
            xOut = X_RCT_MAX
        if yOut < Y_RCT_MIN:
            yOut = Y_RCT_MIN
        elif yOut > Y_RCT_MAX:
            yOut = Y_RCT_MAX        
        return (xOut, yOut, zOut)

def boundDestination(pStart, pEnd):
    """
    Returns a point (x, y, z) that limits the point 'pEnd' = (x, y, z) to the
    boundary of the workspace. 
    Safe operations:
      START                                END
      In cylinder                 ->       In cylinder
      In cylinder                 ->       Circular wall of cylinder
      In cylinder                 ->       Top of cylinder
      In cylinder outside rect    ->       Bottom of cylinder 
      In cylinder inside rect     ->       Bottom of prism inside rect
      In cylinder inside rect     ->       Bottom of cylinder outside rect
      In prism                    ->       In prism
      In prism                    ->       Retangular wall of prism
      In prism                    ->       Bottom of prism
      In prism                    ->       Top of cylinder inside rect 
    """
    (xStart, yStart, zStart) = pStart
    zPenStart = zStart - PEN_DIST
    
    (xEnd, yEnd, zEnd) = pEnd
    zPenEnd = zEnd - PEN_DIST
    
    (xOut, yOut, zOut) = (0, 0, 0)
    
    if penInCylinder(pStart):
        if penInCylinder(pEnd):
            (xOut, yOut, zOut) = pEnd
        else: 
            # Project xy onto outer circle:
            (xOut, yOut, zOut) = boundByCircle(pEnd)
            # Deal with z issues:
            if zEnd > Z_MAX: # Deal with z
                zOut = Z_MAX
            if not inRectangle(pStart):
                if zPenEnd < Z_CLAMP:
                    zOut = Z_CLAMP + PEN_DIST
            else: # if pStart is in rectangle
                if zPenEnd < Z_CLAMP:
                    if inRectangle(pEnd):
                        if zPenEnd < Z_PAPER:
                            zOut = Z_PAPER + PEN_DIST
                    else: # if pEnd is not in rectangle
                        zOut = Z_CLAMP + PEN_DIST
    elif penInPrism(pStart):
        if penInPrism(pEnd):
            (xOut, yOut, zOut) = pEnd
        else:
            # Project xy onto rectangle:
            (xOut, yOut, zOut) = boundByRectangle(pEnd)
            # Deal with z issues:
            if zPenEnd < Z_PAPER:
                zOut = Z_PAPER + PEN_DIST
            if zEnd > Z_MAX:
                zOut = Z_MAX
    else:
    # This should never be entered, because pStart should always be valid. If
    # it isn't, send the point to home.
        (xOut, yOut, zOut) = HOME
        
    return (xOut, yOut, zOut)

def transformPoint(p):
    """
    Takes a position 'p' = (x, y, z) in millimeters in the coordinate system 
    of the leapmotion, and outputs a point (x, y, z) in inches in the coordinate
    system of the of the Delta Robot by doing an appropriate transformation.
    """
    (xIn, yIn, zIn) = p
    # First, change the axes.
    (xOut, yOut, zOut) = (xIn, -zIn, yIn)
    # Next, convert to inches. 
    mmPerInch = 25.4 
    (xOut, yOut, zOut) = (xOut / mmPerInch, yOut / mmPerInch, zOut / mmPerInch)
    # Next, translate the origin.
    (xOut, yOut, zOut) = (xOut + xOffset, yOut + yOffset, zOut + zOffset)
    # Finally, apply a scale factor.
    (xOut, yOut, zOut) = (xScale * xOut, yScale * yOut, zScale * zOut)
    # TODO EDIT SCALE, OFFSET VALUES.
    return (xOut, yOut, zOut)

def restrainMove(pStart, pEnd):
    """
    'pStart' and 'pEnd' are points of the form (x, y, z), in inches. If pEnd
    is <= restrainThresh inches away from pStart, returns pEnd. Otherwise, returns the point
    restrainThresh inches from 'pStart' in the direction of 'pEnd'.
    """
    if dist(pStart, pEnd) <= restrainThresh:
        return pEnd
    else:
        print("RESTRAIN")
        dirVector = dirTo(pStart, pEnd)     # Vector from 'pStart' to 'pEnd'
        normDirVector = normalize(dirVector)    # Norm of 'dirVector'
        transVector = sclProd(restrainDist, normDirVector) 
                # Actual vector to translate; equal to 1" * 'dirVector'.
        return (pStart[x] + transVector[x],
                pStart[y] + transVector[y],
                pStart[z] + transVector[z])

# 
# VECTOR FUNCTIONS.
# 
def dirTo(p1, p2):
    """
    Returns the vector v = [vx, vy, vz] that points from point 
    'p1' = (x1, y1, z1) to point 'p2' = (x2, y2, z2).
    """
    (x1, y1, z1) = p1
    (x2, y2, z2) = p2
    return [x2 - x1, y2 - y1, z2 - z1]

def norm(v):
    """
    Returns the norm of vector 'v' = [vx, vy, vz].
    """
    return sqrt(v[x] ** 2 + v[y] ** 2 + v[z] ** 2)

def normalize(v):
    """
    Returns the normalized version of vector 'v' = [vx, vy, vz].
    """
    n = norm(v)
    return [v[x] / n, v[y] / n, v[z] / n]

def dist(p1, p2):
    """
    Returns the distance from 'p1' = (x1, y1, z1) to 'p2' = (x2, y2, z2).
    """
    return norm(dirTo(p1, p2))

def add(p, v):
    """
    Adds to point 'p' the vector 'v'.
    """
    return (p[x] + v[x], p[y] + v[y], p[z] + v[z])

def sclProd(s, v):
    """
    Returns a vector consisting of scalar 's' multiplied by vector 'v'.
    """
    output = len(v) * [0]
    for i in range(len(v)):
        output[i] = s * v[i]
    return output

def lineImg(p1, p2, step):
    """
    Returns a list of tuples representing the points from 'p1' to 'p2',
    incrementing by size 'step'.
    """
    img = []
    currentPos = p1
    direction = dirTo(p1, p2)
    while dist(currentPos, p2) >= step * 1.00: # Add 0% for rounding error
        img.append((currentPos[x], currentPos[y], currentPos[z]))
        currentPos = add(currentPos, sclProd(step, direction))
    img.append(p2)
    return img

                           
                           
if __name__ == "__main__":
    pStart = (0, 0, -12)
    pEnd = (0, 0, -4.25196)
    print "Start:       " + str(pStart)
    print "Desired End: " + str(pEnd)
    print "Actual End:  " + str(boundDestination(pStart, pEnd))