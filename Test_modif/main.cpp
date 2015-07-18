#include "SpaceNavReader.h"
#include <QtCore>
int main(void)
{
SpaceNavReader *myReader = new SpaceNavReader(0);

/*QObject::connect(myReader,
		 SIGNAL(setNewSpaceNav(const spacenav&)),
		 this,
		 SlOT(setSpaceNav(const spacenav&))  );*/

myReader-> start();
myReader->wait();
return 0;
}
