
#include "Flags.hpp"
#include "font.hpp"
const char *device = "/dev/spidev0.0";
uint8_t mode;
uint8_t bits = 8;
uint32_t speed = 500000;
uint16_t delay = 0;

using namespace std;
unsigned char cX = 1;
unsigned char cY = 1;
unsigned char sdelay = 100;
//----------------------------------------------------------------------------------------------
void transfer(int fd, unsigned char tx[])
{
	int ret;
	
	unsigned char rx[24] = {0, };
	
	// SPI transfert
	spi_ioc_transfer tr;
	tr.cs_change = 0;
	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = 24;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.pad = 0;
	tr.tx_nbits = 0;
	tr.rx_nbits = 0;
	
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
	{
		char buffer[256];
		char * errorMessage = strerror_r(errno,buffer,256);
		cout<<"can't send spi message : " << errorMessage <<endl;
	}
}


//----------------------------------------------------------------------------------------------
void drawPixel (unsigned int X, unsigned int Y, unsigned char *videobuf, unsigned char color) {    //draw drawPixel at x,y coordinates to MOD-LED8x8RGB 1,1 is upper left corner
 
	if (Y<=bigY*8 && X<=bigX*8 && X>0 && Y>0) {
      if (Y>8) X=(X+bigX*8)*((Y-1)/8);
      Y=Y%8;
      if (Y==0) Y=8;
      
      int p;
      p=NumberX-((X-1)/8)-1;
		
      videobuf[3*(Y-1)+24*p]&=~(1<<((X-1)%8));                  //turn off chosen drawPixel
      videobuf[3*(Y-1)+1+24*p]&=~(1<<((X-1)%8));
      videobuf[3*(Y-1)+2+24*p]&=~(1<<((X-1)%8));
		
      if (color&1) videobuf[3*(Y-1)+24*p]|=(1<<((X-1)%8));    //set color to the drawPixel
      if (color&2) videobuf[3*(Y-1)+1+24*p]|=(1<<((X-1)%8));
      if (color&4) videobuf[3*(Y-1)+2+24*p]|=(1<<((X-1)%8));
      
    }
   
   
   
}

//----------------------------------------------------------------------------------------------
void vClear(int fd, unsigned char *videobuf) { 
	int i;
	for(i=0;i<24;i++) videobuf[i] = 0;
}

//----------------------------------------------------------------------------------------------
void drawLine (int x1, int y1, int x2, int y2,unsigned char *videobuf, unsigned char color) {      //draw a line from x1,y1 to x2,y2
   int dx, dy, sx, sy, err, e2;

   dx = abs (x2-x1);
   dy = abs (y2-y1);
   if (x1<x2) sx = 1;
      else sx = -1;
   if (y1<y2) sy = 1;
      else sy = -1;
   err = dx-dy;
   do {
      drawPixel (x1, y1, videobuf, color);
      if ((x1 == x2) && (y1 == y2))
         break;
      e2 = 2*err;
      if (e2 > -dy) {
         err = err - dy;
		 x1 = x1+sx;
      }
      if (e2 < dx) {
         err = err + dx;
		 y1 = y1 + sy;
      }
   } while (1);
	return;
}

//----------------------------------------------------------------------------------------------
void drawRectangle (int x1, int y1, int x2, int y2, unsigned char *videobuf, unsigned char color) {  //draw a rectangle from x1,y1 to x2,y2
   drawLine (x1, y1, x1, y2, videobuf, color);
   drawLine (x1, y1, x2, y1, videobuf, color);
   drawLine (x2, y1, x2, y2, videobuf, color);
   drawLine (x1, y2, x2, y2, videobuf, color);
   return;
}

//----------------------------------------------------------------------------------------------
void drawSolidRectangle (int x1, int y1, int x2, int y2, unsigned char *videobuf, unsigned char color) {  //draw a solid rectangle
   if (x2>x1)
      for (int i=x1; i<=x2;i++)
         drawLine (i, y1, i, y2, videobuf, color);
   else for (int i=x2; i<=x1;i++)
         drawLine (i, y1, i, y2, videobuf, color);
   return;
}

//----------------------------------------------------------------------------------------------
void Draw_4_Ellipse_Points (int CX, int CY, int X, int Y, unsigned char *videobuf, unsigned char color) {  //function needed for drawing an ellipse
   drawPixel (CX+X, CY+Y, videobuf, color);
   drawPixel (CX-X, CY+Y, videobuf, color);
   drawPixel (CX-X, CY-Y, videobuf, color);
   drawPixel (CX+X, CY-Y, videobuf, color);
   return;
}

//----------------------------------------------------------------------------------------------
void drawEllipse (int CX, int CY, int XRadius, int YRadius, unsigned char *videobuf, unsigned char color) {   //draw an ellipse & fix radius if negative
   int X, Y, XChange, YChange, EllipseError, TwoASquare, TwoBSquare, StoppingX, StoppingY;
   if (XRadius<0) XRadius=-XRadius;
   if (YRadius<0) YRadius=-YRadius;

   TwoASquare = 2 * XRadius*XRadius;
   TwoBSquare = 2 * YRadius*YRadius;
   X = XRadius;
   Y = 0;
   XChange = YRadius*YRadius * (1-2*XRadius);
   YChange = XRadius*XRadius;
   EllipseError = 0;
   StoppingX = TwoBSquare*XRadius;
   StoppingY = 0;

   while (StoppingX >= StoppingY) {	        // 1st set of points, y'> -1
      Draw_4_Ellipse_Points (CX, CY, X, Y, videobuf, color);
      Y++;
      StoppingY = StoppingY + TwoASquare;
      EllipseError = EllipseError + YChange;
      YChange = YChange + TwoASquare;
      if ((2*EllipseError + XChange) > 0) {
		 X--;
		 StoppingX = StoppingX - TwoBSquare;
		 EllipseError = EllipseError + XChange;
		 XChange = XChange + TwoBSquare;
   }}

   X = 0;
   Y = YRadius;
   XChange = YRadius*YRadius;
   YChange = XRadius*XRadius * (1-2*YRadius);
   EllipseError = 0;
   StoppingX = 0;
   StoppingY = TwoASquare * YRadius;

   while (StoppingX <= StoppingY) {        // 2nd set of points, y'< -1
      Draw_4_Ellipse_Points (CX, CY, X, Y, videobuf, color);
      X++;
      StoppingX = StoppingX + TwoBSquare;
      EllipseError = EllipseError + XChange;
      XChange = XChange + TwoBSquare;
      if ((2*EllipseError + YChange) > 0) {
         Y--;
		 StoppingY = StoppingY - TwoASquare;
		 EllipseError = EllipseError + YChange;
		 YChange = YChange + TwoASquare;
   }}
   return;
}

//----------------------------------------------------------------------------------------------
void drawCircle (int x, int y, int r, unsigned char *videobuf, unsigned char color) {   //draw a circle
   drawEllipse (x, y, r, r, videobuf, color);
   return;
}

//----------------------------------------------------------------------------------------------
void drawTriangle (int x1, int y1, int x2, int y2, int x3, int y3, unsigned char *videobuf, unsigned char color) {   //draw a triangle
   drawLine (x1, y1, x2, y2, videobuf, color);
   drawLine (x2, y2, x3, y3, videobuf, color);
   drawLine (x3, y3, x1, y1, videobuf, color);
   return;
}

//----------------------------------------------------------------------------------------------
void drawChar(unsigned char c, unsigned char *videobuf, unsigned char color) {  //draw static character  if within the Font limit
   unsigned char b,i;
   signed char k;
   if (c<32 || c>125) c=32;

   for(k=0;k<5;k++) {
      b = FontLookup[c-32][k];
      for(i=0;i<8;i++)
         if (b & (1<<(7-i))) drawPixel(k+cX,i+cY, videobuf, color);	// SPP since our 0, 0 coordinates are in the top left corner instead of bottom left, the letters should be mirrored horizontally ==> (1<<7) replaced with (7-i)
   }
}

//----------------------------------------------------------------------------------------------
void drawString( unsigned char c[], unsigned char *videobuf, unsigned char color) {  //draw static string
      for(int i=0; c[i];i++) {
         drawChar(c[i], videobuf, color);
         cX +=6;
      }
}

void drawLargeCircle (unsigned char *videobuf, unsigned char color){
	drawLine(3,1,6,1, videobuf, color);
	drawLine(3,8,6,8, videobuf, color);
	drawLine(1,3,1,6, videobuf, color);
	drawLine(8,3,8,6, videobuf, color);
	drawPixel(2,2, videobuf, color);
	drawPixel(7,2, videobuf, color);
	drawPixel(2,7, videobuf, color);
	drawPixel(7,7, videobuf, color);
	}

void drawInternCircle (unsigned char *videobuf, unsigned char color){
	drawLine(3,2,6,2, videobuf, color);
	drawLine(3,7,6,7, videobuf, color);
	drawLine(2,3,2,6, videobuf, color);
	drawLine(7,3,7,6, videobuf, color);
	drawPixel(3,3, videobuf, color);
	drawPixel(6,3, videobuf, color);
	drawPixel(3,6, videobuf, color);
	drawPixel(6,6, videobuf, color);
	}
	
void drawDoubleWeighCircle(unsigned char *videobuf, unsigned char color) {
	drawLargeCircle(videobuf, color);
	drawInternCircle(videobuf, color);
}

void drawBlinkSmiley (unsigned char *videobuf, unsigned char color){
	/*drawLargeCircle(videobuf, colorContour);
	  drawLine(3,2,6,2,videobuf, 7);
	  drawLine(2,3,2,6,videobuf, 7);
	  drawLine(7,3,7,6,videobuf, 7);
	  drawLine(3,7,6,7,videobuf, 7); 
	  drawPixel(6,6,videobuf, 7);
	  drawLine(4,3,4,4,videobuf, 7);
	  drawLine(1,3,1,6,videobuf, 7);
	  drawLine(6,3,6,4,videobuf, 7);
	  drawLine(1,3,1,6,videobuf, 7);
	  drawLine(3,5,5,5,videobuf, 7);*/
	  
	  drawLine(2,4,3,4,videobuf, color);
	  drawLine(2,6,3,6,videobuf, color);
	  drawPixel(4,2,videobuf, color);
	  drawPixel(4,8,videobuf, color);
	  drawPixel(5,3,videobuf, color);
	  drawPixel(5,7,videobuf, color);
	  drawLine(6,4,6,6,videobuf, color);
	}
	
void blinkingLed (int fd, unsigned char *videobuf, unsigned char color){
		drawPixel(1,1,videobuf, color);
		transfer(fd, videobuf);
		sleep(1);
		drawPixel(1,1,videobuf, 0);
		transfer(fd, videobuf);
		sleep(1);	
}

void drawLargeTriangle (unsigned char *videobuf, unsigned char color){
	drawLine(1,4,1,5,videobuf, color);
	drawLine(2,3,3,3,videobuf, color);
	drawLine(2,6,3,6,videobuf, color);
	drawLine(4,2,5,2,videobuf, color);
	drawLine(4,7,5,7,videobuf, color);
	drawLine(6,1,7,1,videobuf, color);
	drawLine(6,8,7,8,videobuf, color);
	drawLine(8,1,8,8,videobuf, color);
	
	}
	
void drawInternTriangle (unsigned char *videobuf, unsigned char color){
	drawLine(2,4,2,5,videobuf, color);
	drawLine(3,4,3,5,videobuf, color);
	drawLine(4,3,5,3,videobuf, color);
	drawLine(4,6,5,6,videobuf, color);
	drawLine(7,2,7,7,videobuf, color);
	drawPixel(6,2,videobuf, color);
	drawPixel(6,7,videobuf, color);
}
/*
//----------------------------------------------------------------------------------------------
void lScroll(unsigned char *videobuf) {   //scroll everything left
   for (int i=NumberX*24-1;i>=0;i--) {
      videobuf[i]=videobuf[i]>>1;
      if (i>=24) videobuf[i]|=((videobuf[i-24]&1)<<7);
   }
}

//----------------------------------------------------------------------------------------------
void rScroll(unsigned char *videobuf) {   //scroll everything right
   for (int i=0;i<NumberX*24;i++) {
      videobuf[i]=videobuf[i]<<1;
      if (i<(NumberX-1)*24) videobuf[i]|=(videobuf[i+24]>>7);
   }
}

//----------------------------------------------------------------------------------------------
void scrollCharLeft(int fd, unsigned char c, unsigned char *videobuf, unsigned char color) {   //scroll one character left if within the Font limit
   unsigned char b,i,k;
   if (c<32 || c>125) c=32;

   for(k=0;k<5;k++) {
      b = FontLookup[c-32][k];
      for(i=0;i<8;i++)
         if (b & (1<<(7-i))) drawPixel(bigX*8,i+1+(bigY-1)*8,videobuf, color);	// SPP since our 0, 0 coordinates are in the top left corner instead of bottom left, the letters should be mirrored horizontally ==> (1<<7) replaced with (7-i)
      transfer(fd, videobuf);
      lScroll(videobuf);
      wait(sdelay);
   }
   transfer(fd, videobuf);
   wait(sdelay);
}

//----------------------------------------------------------------------------------------------
void scrollCharRight(int fd, unsigned char c, unsigned char *videobuf, unsigned char color) {  //scroll one character right if within the Font limit
   unsigned char b,i;
   char k;	// SPP - k must be able to take negative value so it should be signed char
   if (c<32 || c>125) c=32;

   rScroll(videobuf);
   for(k=4;k>=0;k--) {
      b = FontLookup[c-32][k];
      for(i=0;i<8;i++)
         if (b & (1<<(7-i))) drawPixel(1,i+1);	// SPP since our 0, 0 coordinates are in the top left corner instead of bottom left, the letters should be mirrored horizontally ==> (1<<7) replaced with (7-i)
      transfer(fd, videobuf);
      rScroll(videobuf);
      wait(sdelay);
   }
   transfer(fd, videobuf);
   wait(sdelay);
}

//----------------------------------------------------------------------------------------------
void theEnder(bool directions) {  //move everything left/right until it leaves the screen
   for (int i=0;i<NumberX*8-1;i++) {
      if (directions) rScroll();
         else lScroll();
      Transfer();
      delay(sdelay);
   }
}

//----------------------------------------------------------------------------------------------
void scrollString(unsigned char c[], bool directions) { //draw a scrolling string
   
      int len;
      for(len=0;c[len];len++);
   
      if (directions) {
         for(int i=len-1; i>=0;i--) {
            scrollCharRight(c[i]);
            color++; if (color>7) color = 1;
         }
      } else {
         for(int i=0;c[i];i++) {
            scrollCharLeft(c[i]);
            color++; if (color>7) color = 1;
         }
      }
      theEnder(directions);
}
*/
