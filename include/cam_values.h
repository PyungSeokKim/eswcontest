#ifndef  __CAM_VALUES_H__
#define __CAM_VALUES_H__

// VIDEOBUF_COUNT must be larger than STILLBUF_COUNT
#define PI 3.14159265358979323846
#define VIDEOBUF_COUNT  3
#define STILLBUF_COUNT  2
#define CAM_STATUS_INIT     0
#define CAM_STATUS_READY    1
#define CAM_STATUS_BUSY     2

// -------- MISO Defined Value ----------

#define CUTLINE 80
#define GAP 10

// threshold
#define THRESHOLD 130
#define THRESHOLD_CB 120

#define THRESHOLD_RED_MIN_CB 100
#define THRESHOLD_RED_CB 130
#define THRESHOLD_RED_CR 165
#define THRESHOLD_YELLOW_CB 120
#define THRESHOLD_YELLOW_CR 140
#define THRESHOLD_GREEN_CB 120
#define THRESHOLD_GREEN_CR 110

#define THRESHOLD_STOP_CB 120
#define THRESHOLD_STOP_CR 170

#define BOT 0
#define MID 1
#define END 2
#define PT_SIZE 3

// find  line flag 
#define FL_NONE 0
#define FL_FIND 1
#define FL_PASS 2

// mid line flag
#define MID_DRIVE 1
#define MID_STOP 2 
#define MID_CL_LEFT 3
#define MID_CL_RIGHT 4

// screen size 
#define MAXHEIGHT 240
#define MAXWIDTH 320
#define MIDWIDTH 160

// color flag
#define COL_UNKNOWN 0
#define COL_PASS 1
#define COL_YELLOW 2
#define COL_WHITE 3
#define COL_RED 4

// macro function 
#define Y(A,B) vidbuf->ycbcr.y[(B)*MAXWIDTH+(A)]
//#define Y(C,D) img_buf_y[(D)][(C)]
#define CB(E,F) vidbuf->ycbcr.cb[(F/2)*MAXWIDTH+(E/2)]
#define CR(G,H) vidbuf->ycbcr.cr[(H/2)*MAXWIDTH+(G/2)]

#define IS_YELLOW(I,J) ( Y(I,J) >= THRESHOLD) && ( CB(I,J) < THRESHOLD_YELLOW_CB)
#define IS_WHITE(K,L) (Y(K,L) >= THRESHOLD+70) && ( CB(K,L) >= THRESHOLD_CB)
#define IS_BLACK(X,Z) !(IS_YELLOW(X,Z) || IS_WHITE(X,Z))
#define IS_RED(X,Z) Y(X,Z) >= THRESHOLD  && CB(X,Z) < THRESHOLD_CB && CR(X,Z) >= THRESHOLD_RED_CR

#define IS_TRAFFIC_RED(X,Z) Y(X,Z) >= THRESHOLD && CB(X,Z) < THRESHOLD_CB && CR(X,Z) >= THRESHOLD_RED_CR
#define IS_TRAFFIC_YELLOW(X,Z) Y(X,Z) >= THRESHOLD  && CB(X,Z) < THRESHOLD_CB && CR(X,Z) >= THRESHOLD_YELLOW_CR && CR(X,Z) < THRESHOLD_RED_CR
#define IS_TRAFFIC_GREEN(X,Z) Y(X,Z) >= THRESHOLD  && CB(X,Z) < THRESHOLD_CB && CR(X,Z) < THRESHOLD_GREEN_CR

// struct 
struct p_point
{
	int x;		// x좌표 (0~319)
	int y;		// y좌표 (0~239)
};


struct p_point pt[PT_SIZE]; // 잡힌 점 

static int cm_handle;
static int fd;

int find_left = FL_NONE, find_right = FL_NONE;
int img_buf_y[MAXHEIGHT][MAXWIDTH];
int width_scan_point = MIDWIDTH;


#endif
