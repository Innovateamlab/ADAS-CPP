#ifndef FLAGS_H_INCLUDED
#define FLAGS_H_INCLUDED

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <cstdio>
//#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>


//CONSTANTS
#define BLUE_CIRCLE 0
#define BLUE_SQUARE 1
#define RED_CIRCLE 2
#define RED_TRIANGLE 3 
//int FAIL = 4;
#define RUNNING 4

#define LIGHT_BLUE 10
#define LIGHT_RED 11 

#define bigX 1                  // Number of MOD-LED8x8RGB in columns
#define bigY 1                  // Number of MOD-LED8x8RGB in rows
#define NumberX bigX*bigY       // Total number of MOD-LED8x8RGBs connected together

extern char * PIPE_FILENAME;


//FLAGS
// 
typedef struct Data
{
	int flag;
	char message[25];
}Data;


/*Flags.cpp*/
int setupNamedPipe(int rights);
void runningThread(int pipeDescriptor);

/*Utils_Flags.cpp*/
void transfer(int fd, unsigned char tx[]);
void drawPixel (unsigned int X, unsigned int Y, unsigned char *videobuf,unsigned char color);
void vClear(int fd, unsigned char *videobuf);
void drawLine (int x1, int y1, int x2, int y2,unsigned char *videobuf, unsigned char color);
void drawRectangle (int x1, int y1, int x2, int y2, unsigned char *videobuf, unsigned char color);
void drawSolidRectangle (int x1, int y1, int x2, int y2, unsigned char *videobuf, unsigned char color);
void Draw_4_Ellipse_Points (int CX, int CY, int X, int Y, unsigned char *videobuf, unsigned char color);
void drawEllipse (int CX, int CY, int XRadius, int YRadius, unsigned char *videobuf, unsigned char color);
void drawCircle (int x, int y, int r, unsigned char *videobuf, unsigned char color);
void drawTriangle (int x1, int y1, int x2, int y2, int x3, int y3, unsigned char *videobuf, unsigned char color);
void drawChar(unsigned char c, unsigned char *videobuf, unsigned char color);
void drawString( unsigned char c[], unsigned char *videobuf, unsigned char color);
void drawLargeCircle (unsigned char *videobuf, unsigned char color);
void drawInternCircle (unsigned char *videobuf, unsigned char color);
void drawDoubleWeighCircle(unsigned char *videobuf, unsigned char color);
void drawBlinkSmiley (unsigned char *videobuf, unsigned char color);
void blinkingLed (int fd, unsigned char *videobuf, unsigned char color);
void drawLargeTriangle (unsigned char *videobuf, unsigned char color);
void drawInternTriangle (unsigned char *videobuf, unsigned char color);
/*void lScroll(unsigned char *videobuf);
void rScroll(unsigned char *videobuf);
void scrollCharLeft(int fd, unsigned char c, unsigned char *videobuf, unsigned char color);
void scrollCharRight(int fd, unsigned char c,unsigned char *videobuf, unsigned char color); 
void theEnder(bool directions);
void scrollString(unsigned char c[], bool directions);
*/
#endif
