#ifndef SPACENAVREADER_H
#define SPACENAVREADER_H
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QtCore/QThread>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
class SpaceNavReader : public QThread
{
	Q_OBJECT

public: 
	

	SpaceNavReader(QObject *parent = 0);

	~SpaceNavReader();

	void run();


private:

	enum SpaceNavCommandType
	{
		SpaceNavLeftButton = 1,
		SpaceNavRightButton=2,
		SpaceNavMotion = 3
	};

	enum SpaceNavMotionType
	{
		SpaceNav_X_Trans = 0,
		SpaceNav_Y_Trans = 1,
		SpaceNav_Z_Trans = 2,
		SpaceNav_X_Rot = 3,
		SpaceNav_Y_Rot = 4,
		SpaceNav_Z_Rot = 5
	};
	enum SpaceNavLeftButtonPress
	{
		SpaceNav_LeftRelease = 0,
		SpaceNav_LeftPress = 1
	};
	
	enum SpaceNavRightButtonPress
	{
		SpaceNav_RightRelease = 0,
		SpaceNav_RightPress = 1
	};
	int fd;
	int i;
    	char *fname ;
	struct input_id ID;
	
	
	char buttonPress;
	char magnitude;
	char commandType;
	char messageClarify;

	void readSpaceNav();
	void handleSpaceNavButtonPress();
	void handleSpaceNavButtonRelease();
	void handleSpaceNavMotion();
};
#endif // SpaceNav.h






