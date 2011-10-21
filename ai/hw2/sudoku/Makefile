CC = g++
CFLAGS=-c -Wall
SOURCES=main.cpp search.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sudoku

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o sudoku
