#include "Flags.hpp"

char * PIPE_FILENAME = "./fifo_adas";

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
