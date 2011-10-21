CC = gcc
CFLAGS=-c -Wall
SOURCES=ConveyorBelt.c file.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=solution

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o solution
