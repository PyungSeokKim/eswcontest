#ifndef __MISO_CAR_LIB_H__
#define __MISO_CAR_LIB_H__

#include "car_lib.h"
#include "miso_values.h"
//#include "cam_values.h"


#define DM_INTERVAL 100

// MACRO FUNCTION. 
#define HIGH(X) X/256
#define LOW(X) X%256

// ANGLE RNAGES. 
#define RANGE_STRAIGHT(X)	((X) == 1000)
#define RANGE_NO_CHANGE(X) 	((X) == 0 || (X) == 90 || (X) == 180)
#define RANGE_RIGHT(X)		(0<(X) && (X)<90)
#define RANGE_LEFT(X)		(90<(X) && (X)<180)

static int g_sp = 0;
static int g_angle = DM_STRAIGHT;
static char g_sleep = FALSE;

// car speed 
void speed_set(int sp)
{
	g_sp = sp;
	lm_speed(HIGH(g_sp),LOW(g_sp));
}

void speed_up(int v)
{
	if( LSPEED_MAX < g_sp+v)
		g_sp = LSPEED_MAX;
	else 
		g_sp+=v;

	lm_speed(HIGH(g_sp),LOW(g_sp));
}

void speed_down(int v)
{
	if( LSPEED_MIN > g_sp - v)
		g_sp = LSPEED_MIN;
	else 
		g_sp-=v;

	lm_speed(HIGH(g_sp),LOW(g_sp));
}

// direction
void turn_straight()
{
	if ( g_angle == DM_STRAIGHT)
	{
		dm_angle(HIGH(g_angle),LOW(g_angle));
	}else
	{
		g_angle = (g_angle+DM_STRAIGHT)/2;
		dm_angle(HIGH(g_angle),LOW(g_angle));
	}
}

void turn_set(int v)
{
	if( v > DM_ANGLE_MAX )
		g_angle = DM_ANGLE_MAX;
	else if( v < DM_ANGLE_MIN)
		g_angle = DM_ANGLE_MIN;
	else
		g_angle = v;

	printf("#set angle %d \n",g_angle);
	dm_angle(HIGH(g_angle),LOW(g_angle));
}

void turn_left(int v)
{
	int angle;
//	angle =  (DM_STRAIGHT+((v-135)*(14))) / DM_INTERVAL + 2;
	turn_set(a2dm_left[((v-1)/2)-45]);
}

void turn_right(int v)
{
	int angle;
//	angle =  (DM_STRAIGHT-((45-v)*(16))) / DM_INTERVAL - 2;
	turn_set(a2dm_right[44 - (v-1)/2]);
}

// cm_angle 
void camera_straight()
{
	cm_angle(HIGH(CM_STRAIGHT),LOW(CM_STRAIGHT));
}

void camera_turn_right()
{
	cm_angle(HIGH(CM_STRAIGHT+100),LOW(CM_STRAIGHT+100));
}

void camera_turn_left()
{
	cm_angle(HIGH(CM_STRAIGHT-100),LOW(CM_STRAIGHT-100));
}

void distance_set(int dis)
{
	distance(HIGH(dis),LOW(dis));
}

void change_line(int v)
{
	if(g_image_flag == IF_CL_LEFT){
	}
	else if( g_image_flag == IF_CL_RIGHT){
	}
}


void set_angle(int angle)
{
	if(RANGE_STRAIGHT(angle)){
		g_drive_flag = DF_STR;
	//	speed_set(1500);
		turn_straight();
	} 
	else if( RANGE_RIGHT(angle)){
		g_drive_flag = DF_CUR;
	//	speed_down(10);		
		turn_right(angle);
	} 
	else if(RANGE_LEFT(angle)){
		g_drive_flag = DF_CUR;		
	//	speed_down(10);
		turn_left(angle);
	}else if(RANGE_NO_CHANGE(angle))
	{
		g_drive_flag = DF_CUR;
	}
}

#endif

