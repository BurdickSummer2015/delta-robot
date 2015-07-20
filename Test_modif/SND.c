#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

/* this macro is used to tell if "bit" is set in "array"
 * it selects a byte from the array, and does a boolean AND
 * operation with a byte that only has the relevant bit set.
 * eg. to check for the 12th bit, we do (array[1] & 1<<4)
 */
#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

int main(int argc, char **argv)
{
    int i = 0;
    char *fname = NULL;
    struct input_id ID;

    int fd;

    // find the SpaceNavigator or similar device
    fname = (char *)malloc(1000 * sizeof(char));
   /* while (i < 32)
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
                printf("Using device: %s\n", fname);
                break;
            }
        }
    }*/
    sprintf(fname,"/dev/input/spacenavigator");
    fd = open(fname, O_RDWR | O_NONBLOCK);
     ioctl(fd, EVIOCGID, &ID);
    printf("Using device: %s\n", fname);
    // detect supported features
    if (fd > 0)
    {
        int axes[6] = {0,0,0,0,0,0};
	int prevaxes[6]  = {0,0,0,0,0,0};
	
        int buttons[2] = {0, 0};
	int prevbuttons[2] = {0,0};

        struct input_event ev;
        uint8_t evtype_bitmask[EV_MAX/8 + 1];
        int ev_type;

        ioctl(fd, EVIOCGBIT(0, sizeof(evtype_bitmask)), evtype_bitmask);

        printf("Supported event types:\n");
        for (ev_type = 0; ev_type < EV_MAX; ev_type++)
        {
            if (test_bit(ev_type, evtype_bitmask))
            {
                /* this means that the bit is set in the event types list */
                printf("  Event type 0x%02x ", ev_type);
                switch ( ev_type)
                {
                    case EV_SYN:
                        printf(" (Sync)\n");
                        break;
                    case EV_REL:
                        printf(" (Relative Axes)\n");
                        break;
                    case EV_KEY :
                        printf(" (Keys or Buttons)\n");
                        break;
                    case EV_ABS :
                        printf(" (Absolute Axes)\n");
                        break;
                    case EV_LED :
                        printf(" (LEDs)\n");
                        break;
                    case EV_REP :
                        printf(" (Repeat)\n");
                        break;
                    case EV_MSC :
                        printf(" (Misc)\n");
                        break;
                    default:
                        printf(" (Unknown event type: 0x%04hx)\n", ev_type);
                }
            }
        }

        // print out all received events
        for (;;)
        {
            int n = read(fd, &ev, sizeof(struct input_event));
            
            if(n >= sizeof(struct input_event))
            {
		
                switch (ev.type)
                {
		    
                    case EV_KEY:
                        printf("Key %d pressed %d %ld.%06ld .\n", ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
                        buttons[ev.code] = ev.value;
			printf("%d\n", n);
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

                        axes[ev.code] = ev.value;
			//printf("%d\n", n);
                        break;

                    case EV_ABS:
                        printf("ABS %d %d %ld.%06ld .\n", ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
                        axes[ev.code] = ev.value;
			//printf("%d\n", n);
                        break;
                        
                    default:
                        break;
                }
            }
	   
          
	
            
            fflush(stdout);
        }
    }
    else
    {
        fputs("Unable to open sensor!\n", stderr);
        return(-1);
    }

    close(fd);
}


