#include <iostream>
#include "Flags.hpp"
#include "font.hpp"
using namespace std;

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

#define MAX_BUFFER 1024

int main(void) 
{	
	int ret = 0;
	int fd;
	unsigned char tx[24] = {0, };
	int i,j,k;
	unsigned char color;
	int pipeDescriptor = setupNamedPipe(O_RDONLY);
	char buffer[MAX_BUFFER];
	Data data;
	int delay_ms;
	
	time_t timer = 0;
	
/*Openning Matrice connection*/		
	fd = open(device, O_RDWR);
	if (fd < 0)
		cout<<"can't open device"<<endl;

	while(1)
	{
		int err = read(pipeDescriptor, &data, sizeof(Data));
		if(err == -1)
		{
			char * message = strerror_r(errno, buffer, MAX_BUFFER);
			cout << "(Read ./Flags.a) " << errno << " : " << message << endl;
			drawPixel(1,1,tx,1);
			transfer(fd, tx);
			return -1;
		}
		else
		{	
			delay_ms = 100;
			
			if(data.flag == RUNNING)
			{
				printf("%s\n",data.message);
				drawPixel (1, 1, tx, 2);
				transfer(fd, tx);
				sleep(1);		
			}
			else if(data.flag == BLUE_SQUARE)
			{
				drawRectangle(2,2,7,7,tx,4);
				drawRectangle(3,3,6,6,tx,7);
				transfer(fd, tx);
				sleep(1);
			}
			else if(data.flag == BLUE_CIRCLE)
			{
				drawLargeCircle(tx, 4);
				drawInternCircle(tx,7);
				transfer(fd, tx);
				sleep(1);
			}
			else if(data.flag == RED_CIRCLE)
			{
				drawLargeCircle(tx, 1);
				drawInternCircle(tx,7);
				transfer(fd, tx);
				sleep(1);
			}
			else if(data.flag == RED_TRIANGLE)
			{
				drawLargeTriangle(tx, 1);
				drawInternTriangle(tx, 7);
				transfer(fd, tx);
				sleep(1);
			}
			
			vClear(fd, tx);
			transfer(fd, tx);
		}
	}	
/*Cercle double épaisseur Rouge*/
	/*drawDoubleWeighCircle(tx,1);
	transfer(fd, tx);
	sleep (5);*/
	
/*Cercle Bleu*/	
	/*drawLargeCircle(tx, 4);
	transfer(fd, tx);
	drawInternCircle(tx,7);
	transfer(fd, tx);
	sleep(5);
	*/
/*Carré Bleu */
	drawRectangle(2,2,7,7,tx,4);
	drawRectangle(3,3,6,6,tx,7);
	transfer(fd, tx);
	sleep(5);
	
/*Cercle Rouge*/	
	/*drawLargeCircle(tx, 1);
	transfer(fd, tx);
	drawInternCircle(tx,7);
	transfer(fd, tx);
	sleep(5);
	*/
	
/*Triangle Rouge*/	
	/*drawLargeTriangle(tx, 1);
	transfer(fd, tx);
	drawInternTriangle(tx, 7);
	transfer(fd, tx);
	sleep(5);*/
		
/*Clear Buffer*/	
	vClear(fd, tx);
	transfer(fd, tx);
	
	sleep(0.5f);
	close(fd);

	return ret;
}

