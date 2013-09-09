#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <pxa_lib.h>
#include <pxa_camera_zl.h>

#include "miso_car_lib.h"
#include "miso_values.h"
#include "cam_values.h"

struct sigaction act;
struct pxa_video_buf* vidbuf;
struct pxacam_setting camset;

struct pxa_camera
{
	int handle;
	int status;
	int mode;
	int sensor;
	int ref_count;

	// Video Buffer
	int width;
	int height;
	enum    pxavid_format format;
};

void sighandler(int signo);
void init_values(int handle);
void init_point();

void print_screen_org();
void print_screen_y();
void print_screen_cb();
void print_screen_cr();
void print_screen_color();

int get_road_angle(struct p_point a, struct p_point b);
int left_line_check(int h);
int right_line_check(int h);
int find_inline(int rl_info, int i, int offset);
int find_outline(int i, int offset);
int check_mid_line(int mid_line, int under_line);
int check_speed_bump_inline(int w,int y);
int find_in_point(int rl_info, int i, int offset);
int find_out_end_point( int i, int offset);
int set_end_point(int rl_info, struct p_point* pt_tmp, int flag);

int check_traffic_light();

int direct = NONE;
int dir_count = 0;

struct image_data* line_check(int handle)
{
	int i,j,rl_info;
	int ret, temp, weight, midangle, topangle;
	struct image_data* img_data = (struct image_data*)malloc(sizeof(struct image_data));
	// init values
	init_values(cm_handle);

#ifdef DEBUG
	print_screen_y();
	//print_screen_org();
	//print_screen_cb();
	//print_screen_cr();
	//print_screen_color();
	//check_traffic_light();
	//exit(0);
#endif

	switch(g_drive_flag)
	{
		int tmp;

		case DF_CUR:
#ifdef TRACE
		printf(" DF_CUR");
#endif
		for(i = 1; i< CUTLINE ; i++)
		{
			if( (find_left == FL_NONE) && left_line_check(i))
			{
				rl_info = LEFT;
			}
			else if( (find_right == FL_NONE) && right_line_check(i))
			{
				rl_info = RIGHT;
			}

			if( find_right == FL_FIND || find_left == FL_FIND)
			{
				g_image_flag = IF_DRIVE;
				break;
			}
		}
		break;

		case DF_STR:
#ifdef TRACE
		printf("DF_STR\n");
#endif
		// 도로 중간 값을 검사한다. 
		tmp = check_mid_line(140,100);
		if( tmp == MID_DRIVE)
		{
			for(i = 1; i< CUTLINE ; i++)
			{
				if((find_left == FL_NONE) && left_line_check(i))
				{
					rl_info = LEFT;
				}
				else if((find_right == FL_NONE) && right_line_check(i))
				{
					rl_info = RIGHT;
				}

				// 두 선중에 하나라도 제대로 된 선을 찾았으면 멈춘다. 
				if( find_right == FL_FIND || find_left == FL_FIND)
				{
					g_image_flag = IF_DRIVE;
					break;
				}
			}
		}
		else if( tmp == MID_STOP)
		{
			g_image_flag = IF_STOP;
			return img_data;
		}
		else if( tmp == MID_CL_LEFT){
			g_image_flag = IF_CL_LEFT;
			printf("----------------------change line left\n");
			return img_data;
		}

		else if( tmp == MID_CL_RIGHT)
		{
			g_image_flag = IF_CL_RIGHT;
			printf("----------------------change line right\n");
			return img_data;
		}
		break;

		case DF_STOP:
		tmp = check_traffic_light();
		// mid red??

		if(tmp == NONE){
			g_image_flag = IF_SG_STOP;
			return img_data;
		}
		else if(tmp == LEFT)
		{
			g_image_flag = IF_SG_LEFT;
			return img_data;
		}
		else if(tmp == RIGHT)
		{
			g_image_flag = IF_SG_RIGHT;
			return img_data;
		}
		// 신호등, 정지선  처리
		break;

		default:
		g_image_flag = IF_NO_PROCESS;
		return img_data;
	}

	if(find_left == FL_NONE && find_right == FL_NONE)
	{
		printf("-------- no line -------\n");
		img_data->angle = 1000;
		return img_data;
	}

	midangle = get_road_angle(pt[BOT], pt[END]);

#ifdef DRIVE_DEBUG
	printf("pt /(%d,%d)->(%d,%d) ", pt[BOT].x, pt[BOT].y,pt[END].x, pt[END].y);
#endif

	img_data->angle = midangle;
	img_data->dist = pt[BOT].y;

	return img_data;
}

// 라인을 찾은 것인지를 TRUE FALSE로 리턴 
int left_line_check(int i)
{
#ifdef TRACE
	printf("left line check\n");
#endif
	int w;

	if(!IS_BLACK(width_scan_point,i))	// 중간 값이 1일때, scan point가 선에 겹친경우. 아웃라인을 찾아야 한다.
	{
		return find_outline(i,width_scan_point);
	}

	for( w = width_scan_point+1 ; w < MAXWIDTH -1; w++)		// 중간값이 1이 아닌 경우 인라인을 찾아야 한다. 
	{
		if(!IS_BLACK(w,i)){
			return find_inline(LEFT,i,w);
		}
	}
	return FALSE;
}

int right_line_check(int i)
{

#ifdef TRACE
	printf("left line check\n");
#endif

	int w;

	if( !IS_BLACK(width_scan_point,i))	// 중간 값이 1일때, scan point가 선에 겹친경우. 아웃라인을 찾아야 한다.
	{
		return find_outline(i,width_scan_point);
	}
	for( w = width_scan_point-1; w >= 0; w--)
	{
		if(!IS_BLACK(w,i)){
			return find_inline(RIGHT,i,w);
		}
	}
	return FALSE;
}

int find_inline(int rl_info, int y, int w)
{
#ifdef TRACE
	printf("find inline\n");
#endif
	int x;
	if(rl_info == LEFT)					// 왼쪽 선이라면 
	{
		// 위로 올라가면서 방지턱을 확인한다
		if(check_speed_bump_inline(w,y))
		{
#ifdef DRIVE_DEBUG
			printf("------------------ speed_bump\n");
#endif
			find_left = FL_PASS;
			return TRUE;
		}

		pt[BOT].y = y;				
		pt[BOT].x = w;
#ifdef DRIVE_DEBUG
		printf("set bot point <%d><%d>\n",w,y);
#endif

		if(find_in_point(LEFT, y, pt[BOT].x)) // sub_point를 찾는다. 
		{
			find_left = FL_FIND;
			return TRUE;
		}else{
		
			return FALSE;
		}
	}
	else if(rl_info == RIGHT)
	{
		// 위로 올라가면서 방지턱을 확인한다. 
		if(check_speed_bump_inline(w,y))
		{
			find_left = FL_PASS;
			return TRUE;
		}
		pt[BOT].y = y;		
		pt[BOT].x = w;

#ifdef DRIVE_DEBUG
		printf("set bot point <%d><%d>\n",w,y);
#endif

		if(find_in_point(RIGHT, y, pt[BOT].x))
		{
			find_right =FL_FIND;
			return TRUE;
		}else{ 
			
			return FALSE;
		}
	}
}

int find_outline(int y, int w)
{

#ifdef TRACE
	printf("find out line\n");
#endif
	int x;
	for(x = w; x < MAXWIDTH-1; x++)
	{
		if( IS_BLACK(x+1,y) && !IS_BLACK(x,y))
		{
			pt[BOT].y = y;
			pt[BOT].x = x;
#ifdef DRIVE_DEBUG
			printf("set bot point <%d><%d>\n",x,y);
#endif
			if(find_out_end_point(y, pt[BOT].x)) // sub_point를 찾는다. 
			{
				find_left =  FL_FIND;
				return TRUE;
			}else{
				init_point();
				return FALSE;
			}
		}
	}
		return FALSE;
}

int check_mid_line(int mid_line,int under_line)
{
#ifdef TRACE
	printf("check mid line\n");
#endif
	// return 되는 가지의 수 
	// 빨간 표지판, 차선 변경, 직진, 곧 곡선구간 

	int i,j,k;
	int left_color = COL_UNKNOWN, right_color = COL_UNKNOWN;

	// mid line 까지 올라가면서 검사한다. 
	for( i = under_line ; i < mid_line; i++)
	{
		if(!IS_BLACK(MIDWIDTH,i))
			break;
		// 왼쪽 탐색 
		for( j = MIDWIDTH ; j < MAXWIDTH-2; j++)
		{
			if( !IS_BLACK(j,i) )
			{
				if(IS_RED(j,i))
				{
					if( i == under_line )
						return MID_STOP;
					return MID_DRIVE;
				}
				// 아웃 라인이 노랑일 때 
				else if(IS_YELLOW(j,i) && IS_BLACK(j+1,i))
				{
#ifdef	DEBUG
					printf("left mid line yellow (%d,%d)\n",j,i);
#endif
					left_color = COL_YELLOW;
					break;
				}// 아웃 라인이 흰색일 때 
				else if( IS_WHITE(j,i) && IS_BLACK(j+1,i))
				{
#ifdef	DEBUG
					printf("left mid line white (%d,%d)\n",j,i);
#endif

					left_color = COL_WHITE;
					break;
				}
			}else 
				break;
		} 
		// 오른 쪽 탐색 
		for( j = MIDWIDTH -1; j > 1 ; j--)
		{
			if( !IS_BLACK(j,i) )
			{
				if(IS_RED(j,i))
				{
					if( i == under_line)
						return MID_STOP;
					return MID_DRIVE;
				}
				else if(IS_YELLOW(j,i) && IS_BLACK(j-1,i))
				{
					right_color = COL_YELLOW;
#ifdef	MID_LINE_DEBUG
					printf("right mid line yellow (%d,%d)\n",j,i);
#endif

					break;
				}
				else if(IS_WHITE(j,i) && IS_BLACK(j-1,i))
				{
					right_color = COL_WHITE;
#ifdef	MID_LINE_DEBUG
					printf("left mid line yellow (%d,%d)\n",j,i);
#endif

					break;
				}
			}else 
				break;
		} 
	} // end vertical scan 

#ifdef MID_LINE_DEBUG

	printf(" left color %d, right color  %d \n", left_color,right_color);
	printf("Y value\n");
	for(j = mid_line; j>under_line ; j--)
	{
		printf("%3d:",j);

		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			if(Y(i,j) >= THRESHOLD)
			{
				if(IS_YELLOW(i,j))
					printf("1");
				else if(IS_WHITE(i,j))
					printf("2");
				else
					printf("0");
			}
			else 
				printf("0");
		}
		printf("\n");
	}
	for( j = 0 ; j < MAXWIDTH; j++)
	{
		if( j == width_scan_point)
			printf("^");
		else 
			printf(" " );
	}
	printf("\n");

#endif
	if(left_color == COL_YELLOW && right_color == COL_WHITE)
		return MID_CL_RIGHT;

	else if(left_color == COL_WHITE && right_color == COL_YELLOW)
		return MID_CL_LEFT;

	else
		return MID_DRIVE;			
}

int check_speed_bump_inline(int w, int y)
{
#ifdef TRACE
	printf("speed bump\n");
#endif
	int count = 0;
	int current_color = ( IS_YELLOW(w,y)? COL_YELLOW : COL_WHITE );
	int h;

	for(h = y; h<CUTLINE; h++)
	{
		if((current_color == COL_YELLOW) && IS_WHITE(w,y))
		{
			current_color = COL_WHITE;	
			count+=1;

		}
		else if((current_color == COL_WHITE) && IS_YELLOW(w,y))
		{
			current_color = COL_YELLOW;
			count+=1;
		}

		if(count > 1)
		{
			printf("current color %d / is_white %d / is_yellow %d \n",current_color,IS_WHITE(w,y), IS_YELLOW(w,y));
			return TRUE;
		}
	}
	return FALSE;
}

int find_in_point(int rl_info, int i, int offset)
{
#ifdef TRACE
	printf("find in point\n");
#endif

	int y, x,flag;
	int direct = NONE;
	struct p_point pt_tmp;

	if(rl_info == LEFT)
	{
		for( y = i+1; y <= CUTLINE ; y++)
		{
			// 위 점이 1인 경우 오른쪽으로 진행중. 
			// 오른쪽으로 돌면서 ->(1,0)을 찾는다. 
			if(!IS_BLACK(offset,y))
			{
				flag = 1;

				for( x = offset; x>0; x--)
				{
					// (1,0)을 찾은 경우 점을 저장하고 offset을 갱신한다.  
					if( !IS_BLACK(x,y) && IS_BLACK(x-1,y) )
					{
						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;

						if(set_end_point(rl_info, &pt_tmp,flag))
							return TRUE;
						break;
					}
				}
				// 오른쪽에서 1,0 을 못 찾은 경우
				if(x == 0)
				{
					if(pt[END].y < 0 ){
						init_point();
						return FALSE;
					}
					return TRUE;
				}
			}
			// 위 점이 0인 경우 왼쪽으로 순회하면서 (1,0)<- 찾는다. 
			else
			{
				flag = 0;

				for( x = offset; x<MAXWIDTH-1; x++)
				{
					if( IS_BLACK(x,y)  && !IS_BLACK(x+1,y))
					{
						pt_tmp.y = y;
						pt_tmp.x = x+1;
						offset = x+1;

						if(set_end_point(rl_info,&pt_tmp,flag))
							return TRUE;
					}
				}
				// 왼쪽에서 (10)을 못 찾은 경우
				if(x == MAXWIDTH-1 && pt[END].y < 0){
						init_point();
						return FALSE;
				}
			}
		}// end scan for top
	} // end rl_info == LEFT
	else{

		for( y = i+1; y <= CUTLINE ; y++)
		{
			if( IS_BLACK(offset,y))
			{
				flag = 0;

				for( x = offset ; x >0 ; x--)
				{
					if(!IS_BLACK(x,y) && IS_BLACK(x-1,y))
					{
						pt_tmp.y = y;
						pt_tmp.x = x-1;
						offset = x-1;

						if(set_end_point(rl_info,&pt_tmp,flag))
							return TRUE;
						break;
					}
				}

				// 오른쪽에서 (0,1) 을 못 찾은 경우
				if(x == 0 && pt[END].y < 0){
					init_point();
					return FALSE;
				}

			} // 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
			else
			{
				flag = 1;
				for( x = offset; x<MAXWIDTH-1; x++)
				{
					if( !IS_BLACK(x,y) && IS_BLACK(x+1,y))
					{
						pt_tmp.y = y;
						pt_tmp.x = x;
						offset = x;

						if(set_end_point(rl_info,&pt_tmp,flag))
							return TRUE;

						break;
					}
				}
				// 왼쪽에서 (0,1)을 못 찾은 경우
				if(x == MAXWIDTH-1 && pt[END].y < 0){
					init_point();
					return FALSE;
				}
			}
		} // end scan for top 
	} // end rl_info == RIGHT
	return TRUE;
}

int find_out_end_point(int i, int offset)
{

#ifdef TRACE
	printf("find out point\n");
#endif
	int y, x;
	
	for( y = i+1; y <= CUTLINE ; y++)
	{
		// 위 점이 1인 경우 왼쪽으로 순회하면서 0,1 찾기
		if( !IS_BLACK(offset,y) )
		{
			for( x = offset; x < MAXWIDTH-1; x++)
			{
				if( IS_BLACK(x+1, y) && !IS_BLACK(x,y))
				{
					pt[END].y = y;
					pt[END].x = x;
					offset = x;
					break;
				}
			}
			if(pt[END].y == -1 && x == MAXWIDTH-1)
				return FALSE;
		}else	// 위 점이 0인 경우 오른으로 순회하면서 0,1 찾기
		{
			for( x = offset; x > 0; x--)
			{
				if( !IS_BLACK(x,y) && IS_BLACK(x-1,y) )
				{
					pt[END].y = y;
					pt[END].x = x;
					offset = x;
					break;
				}
			}
			if(pt[END].y == -1 && x == 0 )
				return FALSE;
		}
	}
	return TRUE;
}

int set_end_point(int rl_info, struct p_point* pt_tmp, int flag)
{
#ifdef TRACE
	printf("set_end_point\n");
#endif

	if( direct == NONE ) // 방향이 설정되어 있지 않을 때,
		{
			if( pt_tmp->y > GAP)
			{
				if( pt[MID].y == -1){
					pt[MID].x = pt_tmp->x;
					pt[MID].y = pt_tmp->y;
				}
				else if( pt[MID].y + GAP < pt_tmp->y )
				{
					if( pt[MID].x > pt_tmp->x )
						direct = RIGHT;
					else if( pt[MID].x < pt_tmp->x)
						direct = LEFT;
				}
			}		
		}else 
		{
			if(
					(direct == LEFT && rl_info == LEFT && flag) ||
					(direct == LEFT && rl_info == RIGHT && !flag) ||
					(direct == RIGHT && rl_info == LEFT && !flag) ||
					(direct == RIGHT && rl_info == RIGHT && flag)
			  ){
				return TRUE;
			}else if( pt[MID].x == pt_tmp->x)
			{
				dir_count+=1;
				if( dir_count >3)
				{
					dir_count = 0;
					return TRUE;
				}
			}

		}

		pt[END].x = pt_tmp->x;
		pt[END].y = pt_tmp->y;

		return FALSE;
}

int get_road_angle( struct p_point a, struct p_point b)
{
	int ret = (int)(atan2((double)(b.y-a.y), (double)(a.x-b.x)) * 180 / PI);
	int angle  = 0 ;
	int weight;

	if( 0<=a.y && a.y<= 30){
		weight = ret>90? (35-a.y) : (35-a.y);
		angle = ( ret>45 || ret < 135) ? ret + (weight/2) : ret + weight; 

		if(angle <0)
			angle =1;

		else if(angle >180)
			angle =179;
	}
	else if( 60<a.y && a.y <=80)
		angle =  (ret>90? ret-(a.y-CUTLINE/2)/3 : ret +(a.y-CUTLINE/2)/3); 
	else 		
		angle  = ret;

#ifdef DRIVE_DEBUG
	printf(" org angle : %d / result angle : %d \n",ret,angle);
#endif
	return angle;
}

int check_traffic_light()
{
	// 정지선에 의한 차량 정지 시 신호등 확인 부분 수행
	// return으로 정지(NONE), 좌회전(LEFT), 우회전(RIGHT)

	int i, j;

	// cutline 위 부분을 확인
	for(i = MAXWIDTH -1 ; i >= 0; i--)
	{
		for(j = MAXHEIGHT-1 ; j> 170; j--)
		{
			// Y, CB, CR값 테스트로 Red, Green, Yellow 구분 (+ 좌회전 화살표?)
			if(IS_TRAFFIC_RED(i,j))
			{
				// 정지
				printf("COLOR : RED!\n");
				return NONE;
			}
			else if(IS_TRAFFIC_GREEN(i,j))	// 초록불인 경우
			{
				printf("COLOR : GREEN!\n");
				// 그냥 초록불인 케이스와 좌회전 신호인 경우 확인
				if(is_left_turn(i,j))		// 좌회전
				{
					// 좌회전 -> 좌회전 수행
					// -> 좌회전을 어느정도 수행하면 영상처리로도 자동으로 좌회전을 수행하게 될듯
					printf("LEFT TURN!\n");
					return LEFT;
				}
				else						// 우회전
				{
					// 그냥 녹색불 -> 우회전 수행
					// -> 우회전도 마찬가지
					printf("RIGHT TURN!\n");
					return RIGHT;
				}
			}
			else if(IS_TRAFFIC_YELLOW(i,j))
			{
				printf("COLOR : YELLOW!\n");
				return NONE;
			}
			else							// 그 외
			{
			}
		}
	}
}

int is_left_turn(int x, int y)
{
	int k, l, count = 0;
	for(k = x; k > x-35 ;k--)
	{
		for(l = y; l > 200; l--)
		{
			if(IS_TRAFFIC_GREEN(k,l))	// 초록불인 경우
			{
				count++;
			}
		}
	}
	if(count < 250)
		return TRUE;
	else
		return FALSE;
}


void init_point()
{
#ifdef DRIVE_DEBUG 
	printf("init point\n");
#endif
	int i;
	for( i = 0; i< 3; i++){
		pt[i].y	= -1;
		pt[i].x = -1;
	}

	direct = NONE;
}

int get_width_scan_point()
{
	if(g_angle > DM_STRAIGHT + 500) // 심한 좌회전인경우
		return MAXWIDTH-1;

	else if(g_angle < DM_STRAIGHT - 500)
		return 0;

	else
		return  MIDWIDTH-1;
}

void init_values(int handle)
{
#ifdef TRACE
	printf("init values\n");
#endif
	int i=0;

	init_point();
	width_scan_point = MIDWIDTH;
	find_left = FL_NONE;
	find_right = FL_NONE;

	// 버퍼 초기화 
	camera_release_frame(cm_handle,vidbuf);
	vidbuf = camera_get_frame(cm_handle);
	camera_release_frame(cm_handle,vidbuf);
	vidbuf = camera_get_frame(cm_handle);
}

void sighandler(int signo)
{
	close(fd);
	exit(0);
}

void print_screen_org()
{	
	int i,j;
	printf("Y value\n");
	for(j = CUTLINE; j>0 ; j--)
	{

		printf("%3d:",j);
		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			printf("%3d ",Y(i,j));
		}
		printf("\n");

	}
	for( j = 0 ; j < MAXWIDTH; j++)
	{
		if( j == width_scan_point)
			printf("^");
		else 
			printf(" " );
	}

}

void print_screen_y()
{
	int i,j;
	printf("Y value\n");
	for(j = CUTLINE; j>0 ; j--)
	{
		printf("%3d:",j);

		for(i = MAXWIDTH-1; i>=0; i--)
		{	
			if(Y(i,j) >= THRESHOLD)
			{
				if(IS_YELLOW(i,j))
					printf("1");
				else if(IS_WHITE(i,j))
					printf("2");
				else
					printf("0");
			}
			else 
				printf("0");
		}
		printf("\n");
	}
	for( j = 0 ; j < MAXWIDTH; j++)
	{
		if( j == width_scan_point)
			printf("^");
		else 
			printf(" " );
	}
	printf("\n");
}

void print_screen_cb()
{
	int i,j;
	printf("CB value\n");
	/*
	   for(i = CUTLINE; i>0; i--)
	   {
	   printf("%3d ", i);
	   }
	 */
	for(j = 100; j>0 ; j--)
	{
		printf("%3d:",j);
		for(i = MAXWIDTH-1; i>=0; i--)
		{
			printf("%3d ",CB(i,j));
		}
		printf("\n");
	}
	printf("\n");

}

void print_screen_cr()
{
	int i,j;
	printf("CR value\n");
	/*
	   for(i = 200; i>=0; i--)
	   {
	   printf("%3d ", i);
	   }
	 */
	for(j = 200; j>100 ; j--)
	{
		printf("%3d:",j);
		for(i = MAXWIDTH -1; i>=0; i--)
		{
			printf("%3d ",CR(i, j));
		}
		printf("\n");
	}
	printf("\n");

}

void print_screen_color()
{
	int i,j;
	printf("    ");
	for(i = MAXWIDTH -1; i>=0; i--)
	{
		printf("%d", i/100);
	}
	printf("\n");
	for(i = 200; i>0 ; i--)
	{
		printf("%3d:",i);
		for(j = MAXWIDTH-1; j>=0; j--)
		{	
			if(IS_RED(j,i))
				printf("R");
			else if(IS_YELLOW(j,i))
				printf("Y");
			else if(IS_WHITE(j,i))
				printf("W");
			else if(IS_BLACK(j,i))
				printf("B");
			else
				printf(" ");
			/*
			   if(vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] >= THRESHOLD_RED_MIN_CB &&
			   vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] < THRESHOLD_RED_CB &&		// 빨간불인 경우
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] >= THRESHOLD_RED_CR)
			   printf("R");
			   else if(vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] < THRESHOLD_GREEN_CB &&
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] < THRESHOLD_GREEN_CR)	// 초록불인 경우
			   printf("G");
			   else if(vidbuf->ycbcr.cb[i/2 * MAXWIDTH + j/2] < THRESHOLD_YELLOW_CB &&		// 노란불인 경우
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] >= THRESHOLD_YELLOW_CR &&
			   vidbuf->ycbcr.cr[i/2 * MAXWIDTH + j/2] < THRESHOLD_RED_CR)
			   printf("Y");
			   else
			   printf(" ");
			 */
		}
		printf("\n");
	}
	/*
	   for( j = 0 ; j < MAXWIDTH; j++)
	   {
	   if( j == width_scan_point)
	   printf("^");
	   else 
	   printf(" " );
	   }
	 */
	printf("\n");
}

int init_camera(){
	int len;

	cm_handle = camera_open(NULL, 0);
	if(cm_handle < 0)
		return -1;

	system("echo b > /proc/invert/tb"); //LCD DriverIC top-bottom invert ctrl

	memset(&camset,0,sizeof(camset));
	camset.mode = CAM_MODE_VIDEO;
	camset.format = pxavid_ycbcr422;
	camset.width = 320;
	camset.height = 240;

	camera_config(cm_handle,&camset);
	camera_start(cm_handle);

	return cm_handle;
}


int exit_camera(int handle){
	camera_stop(handle);
	camera_close(handle);
	return 0;
}

#endif


