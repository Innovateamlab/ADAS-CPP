#ifndef FLAGS_H_INCLUDED
#define FLAGS_H_INCLUDED

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <cstdio>
#include <wiringPi.h>

//CONSTANTS
#define LIGHT_BLUE 2
#define LIGHT_RED 3
//int FAIL = 4;
#define RUNNING 4

extern char * PIPE_FILENAME;

//FLAGS
// 
typedef struct Data
{
	int flag;
	char message[25];
}Data;

//FUNCTIONS
int setupNamedPipe(int rights);
void runningThread(int pipeDescriptor);

#endif
