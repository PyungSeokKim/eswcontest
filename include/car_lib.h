#ifndef __CAR_LIB_H__
#define __CAR_LIB_H__


#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <pthread.h> 
#include <unistd.h> 
#include <dlfcn.h>

#define ADC_1CH 				0x1000  
#define ADC_2CH 				0x1000  
#define ADC_3CH 				0x1000  
#define ADC_4CH 				0x1000  

#define OFF						0x00
#define ON						0xFF
#define LEFT					1
#define RIGHT					2
#define EMERGENCY				3

#define BAUDRATE B19200
#define MODEDEVICE "/dev/ttyS2"



char cmd[100];
unsigned short rxbuf[4];
int uart_fd;
unsigned char buf[3];
unsigned char read_buf[3];
unsigned char write_buf[3];


int car_connect(void)
{
	char fd_serial[20];
	int fd;
	struct termios oldtio, newtio;

	strcpy(fd_serial, MODEDEVICE); //FFUART

	fd = open(fd_serial, O_RDWR | O_NOCTTY );
	if (fd <0) {
		printf("Serial %s  Device Err\n", fd_serial );
		exit(1);
	}
	printf("robot uart ctrl %s\n", MODEDEVICE);

	tcgetattr(fd,&oldtio); /* save current port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;   /* blocking read until 8 chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	uart_fd = fd;
	
	return fd;
}


//--------------------Basic---------------------

char info_scan(void)
{
	buf[0] = 0x80;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf, 1);
	printf("Car ID is %d\n", read_buf);

	return read_buf;
}

void info_write(char* id)
{
	buf[0] = 0x81;
	buf[1] = "********";
	buf[2] = id;
	write(uart_fd, &buf[0], 3);
}

void line_start(void)
{
	buf[0] = 0x82;
	write(uart_fd, &buf[0], 1);
	printf("Line Start\n");
}

void line_stop(void)
{
	buf[0] = 0x83;
	write(uart_fd, &buf[0], 1);
	printf("Line Stop\n");
}

void control_led(int mode, char* value)
{
	buf[0] = 0x85;
	buf[1] = mode;
	buf[2] = value;
	write(uart_fd, &buf[0], 3);
}

void buzzer_on(void)
{
	buf[0] = 0x86;
	write(uart_fd, &buf[0], 1);
//	printf("Buzzer\n");
}

void buzzer_off(void)
{
	buf[0] = 0x87;
	write(uart_fd, &buf[0], 1);
	printf("Buzzer OFF\n");
}

char lm_speed(char* speed1, char* speed2)
{
	buf[0] = 0xa0;
	buf[1] = speed1; //1-F
	buf[2] = speed2; // 1-3C
	write(uart_fd, &buf[0], 3);
//	printf("Set Speed %x%x\n", speed1, speed2);

	return speed1;
}

char lm_step(char* step1, char* step2)
{
	buf[0] = 0xa3;
	buf[1] = step1; //0-FF
	buf[2] = step2; //0-FF
	write(uart_fd, &buf[0], 3);
	printf("Set Step %x%x\n", step1, step2);

	return step1;
}

char distance(char* dis1, char* dis2)
{
	int dis;
	int a;
	buf[0] = 0xa7;
	buf[1] = dis1; //0-FF
	buf[2] = dis2; //0-FF
	write(uart_fd, &buf[0], 3);

//	printf("Set Distance %x%x\n", dis1, dis2);
	//printf("Set Distance %d%d\n", dis1, dis2);

	return dis1;
}

char accel(char* acc)
{
	buf[0] = 0xa8; 
	buf[1] = acc; // < speed, 1-FA
	write(uart_fd, &buf[0], 2);
//	printf("Set Accel %d\n", acc);
	
	return acc;
}

char reduction(char* reduc)
{
	buf[0] = 0xab;
	buf[1] = reduc; // < acc /2, 1-FA
	write(uart_fd, &buf[0], 2);
	printf("Set Reduction %d\n", reduc);

	return reduc;
}

char sudden_stop(char* sud)
{
	buf[0] = 0xae;
	buf[1] = sud; //1-FA
	write(uart_fd, &buf[0], 2); 
	printf("Set SuddenStop %d\n", sud);

	return sud;
}

char lm_break(char* brea)
{
	buf[0] = 0xb0;
	buf[1] = brea; //1-C8
	write(uart_fd, &buf, 2); // reduc < break
	printf("Set Break %d\n", brea);

	return brea;
}

char buzzer_time(char* time)
{
	buf[0] = 0xb1;
	buf[1] = time; //0-FF
	write(uart_fd, &buf[0], 2);
	printf("Buzzer %d Milli Seconds\n", time);
}

int read_sensor(void)
{
	buf[0] = 0xb2;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 1);
	printf("Sensor Value is %d\n", read_buf[0]);

	return read_buf[0];
}

char read_sensor_while(void)
{
	buf[0] = 0xb3;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf, 2);
	printf("Sensor Value is %x\n", read_buf[0]);

	return read_buf[0];
}
void rear_sensor_stop(void)
{
	buf[0] = 0xb4;
	write(uart_fd, &buf[0], 1);
	printf("Sensor stop\n");
}

char busy_check(void)
{
	buf[0] = 0xb5;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 1);

	return read_buf[0];
}

void forward_step(void)
{
	buf[0] = 0xc0;
	write(uart_fd, &buf[0], 1);
	printf("Go Steps\n");
}

void backward_step(int x)
{
	buf[0] = 0xc1;
	write(uart_fd, &buf[0], 1);
	printf("Back Steps\n");
}

void forward_dis(void)
{
	buf[0] = 0xc8;
	write(uart_fd, &buf[0], 1);
//	printf("Go Distance\n");
}

void backward_dis(int x)
{
	buf[0] = 0xc9;
	write(uart_fd, &buf[0], 1);
//	printf("Back Distance\n");
}

void stop(void)
{
	buf[0] = 0xca;
	write(uart_fd, &buf[0], 1);
	printf("Stop\n");
}

char read_speed(void)
{
	buf[0] = 0xcb;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 2);
	printf("Speed is %x%x\n", read_buf[0], read_buf[1]);
}

char total_distance(void)
{
	buf[0] = 0xce;
	write(uart_fd, &buf[0], 1);
	read(uart_fd, &read_buf[0], 4);
	printf("Tatal Distance is %d%d%d%d\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3]);
}

void distance_reset(void)
{
	buf[0] = 0xd1;
	write(uart_fd, &buf[0], 1);
	printf("Distance Reset\n");
}

void motor_hold(void)
{
	buf[0] = 0xe0;
	write(uart_fd, &buf[0], 1);
	printf("Hold Motor\n");
}

void motor_hold_cancel(void)
{
	buf[0] = 0xe1;
	write(uart_fd, &buf[0], 1);
	printf("Hold Motor Cancel\n");
}

void front_light(char* status)
{
	buf[0] = 0xF0;
	if(status == ON)
	{
		buf[1] = ON;
		printf("Front Light ON\n");
	}
	else
	{
		buf[1] = OFF;
		printf("Front Light OFF\n");
	}
	write(uart_fd, &buf[0], 2);
}

void break_light(char* status)
{
	buf[0] = 0xf1;
	if(status == ON)
	{
		buf[1] = ON;
		printf("Break Light ON\n");
	}
	else
	{
		buf[1] = OFF;
		printf("Bread Light OFF\n");
	}
	write(uart_fd, &buf[0], 2);
}

void fog_light(char* status)
{
	buf[0] = 0xf2;
	if(status == ON)
	{
		buf[1] = ON;
		printf("Fog Light ON\n");
	}
	else
	{
		buf[1] = OFF;
		printf("Fog Light OFF\n");
	}
	write(uart_fd, &buf[0], 2);
}

void winker_light(char* status)
{
	buf[0] = 0xf3;
	if(status == LEFT)
	{
		buf[1] = LEFT;
		printf("Winker Left\n");
	}
	else if(status == RIGHT)
	{
		buf[1] = RIGHT;
		printf("Winker Right\n");
	}
	else if(status == EMERGENCY)
	{
		buf[1] = EMERGENCY;
		printf("Emergency\n");
	}
	else
	{
		buf[1] = OFF;
		printf("Winker OFF\n");
	}
	write(uart_fd, &buf[0], 2);

}

char dm_angle(char* angle1, char* angle2)
{
	buf[0] = 0xf4;
	buf[1] = angle1;
	buf[2] = angle2; 
	write(uart_fd, &buf[0], 3);
	//printf("Set Drive Direction %x%x\n", angle1, angle2);


	return angle1;
}

char dm_speed(char* speed)
{
	buf[0] = 0xf5;
	buf[1] = speed;
	write(uart_fd, &buf[0], 2);
	printf("Set Direction Speed %d\n", speed);
	
	return speed;
}

char cm_angle(char* angle1, char* angle2)
{
	buf[0] = 0xf6;
	buf[1] = angle1;
	buf[2] = angle2;
	write(uart_fd, &buf[0], 3);
	printf("Set Camera Angle %d%d\n", angle1, angle2);
	
	return angle1;
}

char cm_speed(char* speed1)
{
	buf[0] = 0xf7;
	buf[1] = speed1;
	write(uart_fd, &buf[0], 2);
	printf("Set Camera Speed %d\n", speed1);
	
	return speed1;
}
//----------------------Basic------------------------

//---------------------system-------------------------
void uwait(int us)
{
	printf("Delay %d Micro Second\n", us);
	usleep(us);
}

void wait(int se)
{
	printf("Wait %d Second\n", se);
	sleep(se);
}

void camera_on(void)
{
	sprintf(cmd, "/root/test_app/camera &");
	printf("%s\n", cmd);
	system(cmd);
}

void camera_off(void)
{
	sprintf(cmd, "killall -9 camera");
	printf("%s\n", cmd);
	system(cmd);
}

void capture(void)
{
	sprintf(cmd, "/root/test_app/camera_yuv_capture");
	printf("%s\n", cmd);
	system(cmd);
}

void capture_view(char* cap)
{
	sprintf(cmd, "export SDL_NOMOUSE=1");
	printf("%s\n", cmd);
	system(cmd);
	sprintf(cmd, "/root/test_app/yay/yay %s &", cap);
	printf("Capture View\n");
	system(cmd);
}

void lcd_disp_num(int num)
{
	sprintf(cmd, "/root/test_app/fbv_enlarge /root/PIC/%d.jpg", num);	
	printf("%s\n", cmd);	
	system( cmd );
}

void lcd_disp_pic(char* pic)
{
	sprintf(cmd, "/root/test_app/fbv_enlarge /root/PIC/%s", pic);	
	printf("%s\n", cmd);	
	system( cmd );
}

void sound_play(char* sound)
{
	sprintf(cmd, "mpg123 /root/MP3/%s", sound);	
	printf("%s\n", cmd);	
	system( cmd );
}

void disp_inv_up(void)
{
	sprintf(cmd, "echo t > /proc/invert/tb");
	system(cmd);
}

void disp_inv_down(void)
{
	sprintf(cmd, "echo b > /proc/invert/tb");
	system(cmd);
}

void ps_kill(char* kill)
{
	sprintf(cmd, "killall %s", kill);
	system(cmd);
}

char check_distance(int ch)
{
	int fd, i;
	struct timeval start, end;

	if((fd = open("/dev/FOUR_ADC", O_RDONLY )) < 0)
	{         // KEY open
		perror("open faile /dev/FOUR_ADC");
		exit(-1);
	}


	read(fd, rxbuf, sizeof(rxbuf));	// ADC READ
	// printf("ch1 %04d,  ch2 %04d,  ch3 %04d,  ch4 %04d\n", rxbuf[0], rxbuf[1], rxbuf[2] ,rxbuf[3]);

	usleep(50000); //0.1 sec

	return rxbuf[ch-1];
}

char complete(void)
{
	printf("Wait Complete\n");
	while(busy_check()==0xff);

	return busy_check();
}


//-----------------end system function---------------------
#endif


	
