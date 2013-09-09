
#ifndef __MISO_VALUES_H__
#define __MISO_VALUES_H__

#define NONE -1

#define TRUE 0xff
#define FALSE 0x00

// required define-values. 
#define LSPEED_MAX 2000
#define LSPEED_MIN 500
#define DISTANCE_MAX 65535
#define DISTANCE_MIN 0	
#define DM_ANGLE_MAX 2200
#define DM_ANGLE_MIN 800 
#define CM_SPEED_MAX 2200
#define CM_SPEED_MIN 800

// drive flag
#define DF_STOP 0
#define DF_STR 1
#define DF_CUR 2
#define DF_VPARK 3
#define DF_PPARK 4
#define DF_END 9

// image flag 
#define IF_NO_PROCESS -1

#define IF_STOP 0
#define IF_DRIVE 1
#define IF_CL_LEFT 2
#define IF_CL_RIGHT 3

#define IF_SG_STOP 10
#define IF_SG_LEFT 12
#define IF_SG_RIGHT 13

#define DM_STRAIGHT 1533
#define CM_STRAIGHT 1533

struct image_data
{
	int angle;	
	int dist;
};

// flags
static int g_drive_flag = DF_STR;
static int g_image_flag = IF_DRIVE;


// data arrays
static int a2dm_right[45]={
1100,1100,1100,1100,1100, 	// 90 ~ 81
1200,1200,1200,1200,1200, 	// 80 ~ 71
1300,1300,1300,1300,1300, 	// 70 ~ 60
1400,1400,1400,1400,1400, 	// 60 ~ 51
1533,1533,1400,1400,1400,	// 50 ~ 41
1300,1300,1300,1200,1200,	// 40 ~ 31
1200,1200,1100,1100,1100,	// 30 ~ 21
1100,1100,1100,1100,1000,	// 20 ~ 11
1000,900,900,800,800	// 10 ~ 1
};	// img angle에 대한 drive moter 값 설정 

static int a2dm_left[45]={
1800,1800,1800,1800,1800, 	// 91 ~ 100
1700,1700,1700,1700,1700, 	// 101 ~ 110
1650,1650,1650,1650,1650,	// 111 ~ 120
1600,1600,1600,1600,1600,	// 121 ~ 130
1533,1533,1650,1650,1650,	// 131 ~ 140
1750,1750,1750,1750,1750,	// 141 ~ 150
1850,1850,1850,1850,1850,	// 151 ~ 160
1900,1900,1900,2000,2000,	// 161 ~ 170
2100,2100,2200,2200,2200	// 171 ~ 180
};	// img angle에 대한 drive moter 값 설정 


#endif
