#include "SpaceNavReader.h"

SpaceNavReader::SpaceNavReader(QObject *parent):
	QThread(parent)

{
	 fname = (char *)malloc(1000 * sizeof(char));
	 i = 0;
	while (i < 32)
	    {
		sprintf(fname, "/dev/input/event%d", i++);
		fd = open(fname, O_RDWR | O_NONBLOCK);
		if (fd > 0)
		{
		    ioctl(fd, EVIOCGID, &ID);

		    if (ID.vendor == 0x046d && 
		            (ID.product == 0xc626 || 
		             ID.product == 0xc623 ||
		             ID.product == 0xc603))
		    {
		        fprintf(stderr,"Using device: %s\n", fname);
		        break;
		    }
		}
	    }
	  fd = open(fname, O_RDWR | O_NONBLOCK);
	  ioctl(fd, EVIOCGID, &ID);
};

SpaceNavReader::~SpaceNavReader()
{
	close(fd);
};

void SpaceNavReader::run()
{
	readSpaceNav();
};

void SpaceNavReader::readSpaceNav()
{	
	struct input_event ev;
	while(1)
	{   int n = read(fd, &ev, sizeof(struct input_event));
            if(n >= sizeof(struct input_event))
		{switch (ev.type)
                {
		    
                    case EV_KEY:
			if (ev.time.tv_usec > 0.0)
                        {printf("Key %d pressed %d %ld.%06ld .\n", ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);}
                      
                        break;

                    
                    
                    case EV_REL:
                       // printf("REL %d %d %ld.%06ld .\n", ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
			switch (ev.code)
				{
					case 0:	
						 printf("X Translation %d %d %ld.%06ld .\n",ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
						 break;
					case 1:	
						 printf("Y Translation %d %d %ld.%06ld .\n",ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
						 break;
					case 2:	
						 printf("Z Translation %d %d %ld.%06ld .\n",ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
						break;
					case 3:	
						printf("X Rotation %d %d %ld.%06ld .\n",ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
						break;
					case 4:	
						printf("Y Rotation %d %d %ld.%06ld .\n",ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
						break;	
					case 5:	
						 printf("Z Rotation %d %d %ld.%06ld .\n",ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
						break;	
				}

                       
			//printf("%d\n", n);
                        break;

                    default:
                        break;
                }}
	   fflush(stdout);
		
	}
};

void SpaceNavReader::handleSpaceNavButtonPress()
{ 
	//Nothing yet
};
	void handleSpaceNavButtonRelease()
{ 
	//Nothing yet
};
	void handleSpaceNavMotion()
{ 
	//Nothing yet
};
