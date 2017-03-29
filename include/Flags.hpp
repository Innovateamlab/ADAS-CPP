#ifndef FLAGS_H_INCLUDED
#define FLAGS_H_INCLUDED

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <cstdio>

//CONSTANTS
int LIGHT_BLUE = 2;
int LIGHT_RED = 3;
//int FAIL = 4;
int RUNNING = 4;

char * PIPE_FILENAME = "./fifo_adas";


//FLAGS

// 
typedef struct Data
{
	int flag;
	char message[25];
}Data;

//FUNCTIONS
int setupNamedPipe(int rights)
{
	int fd;
	
	if(rights == O_WRONLY)
		mkfifo(PIPE_FILENAME, 0666);
	
	//O_WRONLY - O_RDONLY
	fd = open(PIPE_FILENAME, rights);
	if(fd == -1)
	{
		char buffer[256];
		char * message = strerror_r(errno, buffer, 256);
		std::cout << "(Open) " << errno << " : " << message << std::endl;
	}
	
	return fd;
}

void runningThread(int pipeDescriptor)
{
	pid_t pid = fork();
	
	if(pid == 0)
	{
		//Child
		while(1)
		{
			Data data;
			data.flag = RUNNING;
			std::sprintf (data.message, "Running Thread");
			int err = write(pipeDescriptor,&data, sizeof(Data));
			if(err == -1)
			{
				char buffer[256];
				char * message = strerror_r(errno, buffer, 256);
				std::cout << "(Running Thread) " << errno << " : " << message << std::endl;
				break;
			}
			
			sleep(2);
		}
	}
}






#endif
