# Used compiler:
CC=g++
# Options passed to compiler
CFLAGS=-c -Wall 
# LD flags
LDFLAGS=-lpthread -lrt
#Sources files in project
SOURCES= ../src/main.cpp ../src/highloadgenC.cpp ../src/mathC.cpp ../src/pwmC.cpp ../src/sortC.cpp ../src/timeStampC.cpp ../src/gpioC.cpp
#Compile into objects file
OBJECTS=$(SOURCES:.cpp=.o)
#Output file name
EXECUTABLE=measure

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE)



	
