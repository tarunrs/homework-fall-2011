CC = g++
CFLAGS=-c -Wall
SOURCES=main.cpp search.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=solution

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o solution
