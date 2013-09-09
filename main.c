//#define DEBUG
#define DRIVE_DEBUG
//#define MID_LINE_DEBUG
#define DRIVE
//#define TRACE

#include <stdio.h>
#include <pthread.h>

#include "include/miso_values.h"
#include "include/miso_car_lib.h"
#include "include/miso_camera.h"
#include "include/key_handler.h"
#include "include/sensor_handler.h"
#include "include/distance_check.h"

pthread_t thread[3];	// 0:key handling , 1:sensor handling, 2:distance_check

void init_drive(void);
void drive(void);
void direct_test(void);

int main(void)
{
	cm_handle = init_camera();
	car_connect();
	pthread_create(&thread[0],NULL,key_handler,NULL);
	//pthread_create(&thread[1],NULL,sensor_handler,NULL);
	//pthread_create(&thread[2],NULL,distance_check,NULL);

#ifdef DRIVE
	drive();
#else
	direct_test();
#endif
	pthread_join(thread[0],NULL);
	//pthread_join(thread[1],NULL);
	//pthread_join(thread[2],NULL);
	return 0;
}

void drive(void)
{
	struct image_data* idata;
	int angle,input;
	init_drive();

	while(TRUE)
	{			

		idata = line_check(cm_handle); // get image data 

		switch(g_image_flag)
		{			

			case IF_STOP:
				stop();
				g_drive_flag = DF_STOP;
			break;
			case IF_DRIVE:
#ifdef DRIVE_DEBUG
			printf("img angle %d\n", idata->angle);
#endif
			set_angle(idata->angle);
			break;
			case IF_CL_LEFT:
			case IF_CL_RIGHT:
			case IF_SG_STOP:
			case IF_SG_LEFT:
			case IF_SG_RIGHT:
			break;
		}
#ifdef DRIVE
		distance_set(500);		
		forward_dis();
#endif
	}
}

void init_drive(){
#ifdef DRIVE
	sleep(3);
#endif
	turn_straight();
	usleep(2000);
	camera_straight();
	usleep(2000);
	speed_set(1000);
	usleep(2000);
	accel(0x02f);
	usleep(2000);
	reduction(0x2f);
	usleep(2000);
	distance_set(2000);
	line_stop();
}

void direct_test()
{
	struct image_data* idata;
	int input;

	while(TRUE)
	{
		printf("0.get image, 1. turn left, 2. turn right, 3. set straight, 4. go, 5. back \n");
		scanf("%c",&input);
		switch(input)
		{
			case '0':
				idata = line_check(cm_handle); // get image data 
				printf("angle : %d\n",idata->angle);
				break;	
			case '1':	
				g_angle += 50;
				turn_set(g_angle);
				break;
			case '2':
				g_angle -= 50;
				turn_set(g_angle);
				break;
			case '3':
				turn_straight();
				break;
			case '4':
				distance_set(50);
				forward_dis();
				break;
			case '5':
				distance_set(50);
				backward_dis(0);
				break;
			defualt: 
				break;
		}
	}
}
