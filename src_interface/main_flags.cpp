#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

using namespace std;

#define MAX_BUFFER 1024

int setupNamedPipe();

int main(void) 
{
	int pipeDescriptor = setupNamedPipe();
	char buffer[MAX_BUFFER];
	
	
	while(1)
	{
	
		int err = read(pipeDescriptor, buffer, MAX_BUFFER);
		if(err == -1)
		{
			char buffer[256];
			char * message = strerror_r(errno, buffer, 256);
			cout << "(Read ./Flags.a) " << errno << " : " << message << endl;
		}
		else
		{
			cout << "Received " << buffer << endl;
		}
	}
	
	
	/*
	while(1)
	{
		read(pipeDescriptor, buffer, MAX_BUFFER);
		cout << "Received " << buffer << endl;
	}*/
	
	
	
	
	
	/*int color_flag = buffer[0];
	int fail_flag = buffer[4];
	
	wiringPiSetup () ;
	pinMode (LIGHT_BLUE, OUTPUT);
	pinMode (LIGHT_RED, OUTPUT);
	pinMode (FAIL, OUTPUT);
	
	if (color_flag == LIGHT_BLUE)
	{
		digitalWrite (LIGHT_BLUE, HIGH) ; delay (200) ;
		digitalWrite (LIGHT_BLUE,  LOW) ; delay (500) ;
	}
	if (color_flag == LIGHT_RED)
	{
		digitalWrite (LIGHT_RED, HIGH) ; delay (200) ;
		digitalWrite (LIGHT_RED,  LOW) ; delay (500) ;
	}
	if (fail_flag == FAIL) // 3 time green blink when error 
	{
		while(1){
			digitalWrite (FAIL, HIGH) ; delay (200) ;
			digitalWrite (FAIL,  LOW) ; delay (500) ;
			digitalWrite (FAIL, HIGH) ; delay (200) ;
			digitalWrite (FAIL,  LOW) ; delay (500) ;
			digitalWrite (FAIL, HIGH) ; delay (200) ;
			digitalWrite (FAIL,  LOW) ; delay (500) ;
		}
	}*/
}

int setupNamedPipe()
{
	int fd;
	char * fifo_adas = "./fifo_adas";
	
	fd = open(fifo_adas, O_RDONLY);
	if(fd == -1)
	{
		char buffer[256];
		char * message = strerror_r(errno, buffer, 256);
		cout << "(Open ./Flags.a) " << errno << " : " << message << endl;
	}
	
	return fd;
}
