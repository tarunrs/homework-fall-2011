CC = g++
CFLAGS=-c
SOURCES=main.c stem.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mine

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *o mine
	rm -rf results/*
