delta-robot

This is the git repository for the Delta Robot built by Justin Koch and Edward Fouad. The project was built for ME 115, Kinematics and Robotics taught by Professor Joel Burdick.

*July 1, 2015:
	Forked by Danny Weitekamp, Charlotte Suchet, Romain Teulier
	-For further development including optimization, smoothing and integration with additional devices
	
Explanation of files:
DeltaMSVS/ -> contains Microsoft visual studio code
	deltaKinematics->code for inverse kinematics eq
	PolyFit->Code for something I never finished. i.e Control path.
	SpaceNav->Probably nothing
	Tests->Crud
Delta Driver/ -> Atmel studios code
	Delta Driver/ ->code for the microcontroller
		(shares delta_kinematics.c with DeltaMSVS/deltaKinematics
Spacenavig-Library\
	Windows/->Code for compiling Windows .dll for SpaceNav controller
	Linux/->Code for compiling Linux .so for SpaceNav controller
Python/->Python code

*Important changes:
-Now works with Spacenav 3D mouse (Linux and Windows)
-Class structure is more modular (controllers simply need to extend controller_interface)
-Microcontroller now takes 6 bytes (2 per x,y,z) instead of string and computes full inverse kinematics equation
-Leap controller now runs on Event system (30 FPS)
-Optimized inv Kinematics equation, and verified that I did not change initial behavior
-Got rid of Leap averaging (was not constructive since it was taking the average of the same number repeatedly)

*TODO
-Fix discontinuities in motion near edges (currently reproducible at certain points with spaceNav). I have not yet ruled out issues in invKinematics or motors.
-See if motors can be made more accurate
	-Via adjusting settings/physical gears
	-In the long term, create a control feedback loop
-Implement tween-controller (would be useful for automating repetitive movements)





		


