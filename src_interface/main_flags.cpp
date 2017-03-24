#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

#define MAX_BUFFER 1024

int setupNamedPipe();

int main(void) 
{
	int pipeDescriptor = setupNamedPipe();
	char buffer[MAX_BUFFER];
	
	
	while(1)
	{
	
		read(pipeDescriptor, buffer, MAX_BUFFER);
		cout << "Received " << buffer << endl;
	
	}
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
	char * fifo_adas = "/tmp/fifo_adas";
	
	fd = open(fifo_adas, O_RDONLY);
	
	return fd;
}
