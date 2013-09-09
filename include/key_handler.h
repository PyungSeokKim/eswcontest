

#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include "miso_camera.h"
#include "miso_car_lib.h"

#define KEY1 1
#define KEY2 2
#define KEY3 3

static char keyDev[] ="/dev/KEYPAD";
static int keyFD = -1;
static char keyState[3]={OFF,OFF,OFF};
int point;

void* key_handler(void* data)
{
	// connect car 
	unsigned char buf,read_key;
	int key;

	// open key interface. 
	if((keyFD = open(keyDev,O_RDONLY))<0)
	{
		perror("Cannot open /dev/key!");
		buzzer_on();
	}

	// buzy wait to pushing key. 
	while(1){
		read_key= read(keyFD, &buf,sizeof(buf));
		key = read_key;

		switch(key)
		{
			case KEY1:
				keyState[0]=~keyState[0];
				printf("keyState0 : %d\n",keyState[0]);
				speed_up(1000);
				buzzer_on();
				break;
			case KEY2:
				keyState[1]=~keyState[1];
				printf("keyState1 : %d\n",keyState[1]);
				speed_down(1000);
				buzzer_on();
				usleep(500000);
				buzzer_on();
				break;
			case KEY3:
				buzzer_on();
				usleep(500000);
				buzzer_on();
				usleep(500000);
				buzzer_on();
				usleep(500000);
				stop();
				exit_camera(cm_handle);
				exit(0);
				return 0;
			default:
				break;
		}
	}
}

#endif
