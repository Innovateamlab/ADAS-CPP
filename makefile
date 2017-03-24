SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:.cpp=.o)

#OBJ = main.o Mask.o Reshape.o ShapeDetector.o Utils.o

DEPS = include/
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lrt -lwiringPi -lraspicam -lraspicam_cv -lopencv_imgcodecs
CFLAGS = -Wall

CC = g++

all: ShapeColorDetector.a clean

#define a rule that applies to all files ending in the .o suffix, which says that the .o file depends upon the .c version of the file and all the .h files included in the DEPS macro.  Compile each object file
%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) -I $(DEPS) 

#Combine them into the output file
#Set your desired exe output file name here
ShapeColorDetector.a: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -Wl,-rpath,libs/ $(LIBS)

#Cleanup
.PHONY: clean

clean:
	rm -f src/*.o *~ core *~
