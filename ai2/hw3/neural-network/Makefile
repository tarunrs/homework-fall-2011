CC = g++
CFLAGS=-c -Wall
SOURCES=neural-network.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=neural-network

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o neural-network
