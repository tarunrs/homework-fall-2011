CC = g++
CFLAGS=-c -Wall
SOURCES=decision-tree.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=decision-tree

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o decision-tree
