#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/* this macro is used to tell if "bit" is set in "array"
 * it selects a byte from the array, and does a boolean AND
 * operation with a byte that only has the relevant bit set.
 * eg. to check for the 12th bit, we do (array[1] & 1<<4)
 */
#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))


float trans[] = {0.0, 0.0 ,0.0};
int fd;
double prevTime;
double zeroTime = .01;
extern void getMousePosition(float *mylist){
    mylist[0] = trans[0];
    mylist[1] = trans[1];
    mylist[2] = trans[2];
}

extern void readEvent(double inputTime) {
    struct input_event ev;
    int i = 0;
    int coeff = 1;
    float x_trans, y_trans, z_trans;
    char *fname = NULL;
    struct input_id ID;
    while(1){
	
        int n = read(fd, &ev, sizeof(struct input_event));
        //if(n <= 0){
          //  printf("%i", (int)(sizeof(struct input_event)));
            //return;
        //}
                
        if(n >= (int)sizeof(struct input_event)){
            
            switch (ev.type){
                case EV_KEY:
                    // printf("Key %d pressed %d %ld.%06ld .\n", ev.code,ev.value,ev.time.tv_sec,ev.time.tv_usec);
                    //buttons[ev.code] = ev.value;
                    // printf("%d\n", n);
                    break;

                /* 
                    older kernels than and including 2.6.31 send EV_REL events for SpaceNavigator movement
                    newer - 2.6.35 and upwards send the more logical EV_ABS instead. 
                    
                    The meaning of the numbers is the same. Spotted by Thomax, thomax23@googlemail.com*/
                
                case EV_REL:
                    if(inputTime != 0){
                        prevTime = inputTime;
                    }else{
                        prevTime = (double)clock()/(double)CLOCKS_PER_SEC;
                    }
                    switch (ev.code){
                        case 0: 
                             x_trans = ev.value*coeff;
                             trans[0] = x_trans;
                             //y_trans = 0;
                             //z_trans = 0;
                             // printf("X Translation %f\n",x_trans);
                             break;

                        case 1:  
                             //x_trans = 0;
                             y_trans = -ev.value*coeff;   //Change axis way
                             trans[1] = y_trans;
                             //z_trans = 0;
                             // printf("Y Translation %f\n",y_trans);
                             break;

                        case 2: 
                             //x_trans = 0;
                             //y_trans = 0;
                             z_trans = ev.value*coeff;
                             trans[2] = z_trans;
                             // printf("Z Translation %f\n",z_trans);
                            
                             break;
                    }  

           default:
                break;
            }
            // fflush(stdout);
        }else{            
            break;
        }
    }
    //printf("%f\n", (clock() - prevTime + zeroTime*CLOCKS_PER_SEC));
    double c;
    if(inputTime != 0){
        c = inputTime;
    }else{
        c = (double)clock();
    }

    if(c > (double)(prevTime + zeroTime)){
        trans[0] = 0;
        trans[1] = 0;
        trans[2] = 0;
    }
}

extern int init(){
    int i = 0;
    int coeff = 100;
    float x_trans, y_trans, z_trans;
    char *fname = NULL;
    struct input_id ID;

    

    // find the SpaceNavigator or similar device
    fname = (char *)malloc(1000 * sizeof(char));
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
                printf("Using device: %s\n", fname);
                break;
            }
        }
    }
    
    
    // detect supported features
    if (fd > 0)
    {
        int axes[6] = {0,0,0,0,0,0};
       int prevaxes[6]  = {0,0,0,0,0,0};
    
        //int buttons[2] = {0, 0};
       //int prevbuttons[2] = {0,0};

        struct input_event ev;
        uint8_t evtype_bitmask[EV_MAX/8 + 1];
        int ev_type;

        ioctl(fd, EVIOCGBIT(0, sizeof(evtype_bitmask)), evtype_bitmask);

        printf("Supported event types:\n");
        for (ev_type = 0; ev_type < EV_MAX; ev_type++)
        {
            if (test_bit(ev_type, evtype_bitmask))
            {
                 // this means that the bit is set in the event types list 
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
        return 1;
    }
    else
    {
        fputs("Unable to open sensor!\n", stderr);
        return 0;
    }
}


extern int main()  //int argc, char **argv
{
    if(init()){
        // print out all received events
        clock_t t = clock();
        while(1){
            if(clock() > t + CLOCKS_PER_SEC/10){
                readEvent(0);
                t = clock();
            }
        }
    }

    close(fd);
}


