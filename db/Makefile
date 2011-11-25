# sample makefile using Coin3D library

COIN_DIRECTORY = /usr/local
COIN_INCLUDE = -I/usr/include/postgresql/
COIN_LIBRARIES = -L/usr/lib -lpq

CPP = gcc
#-R$(COIN_DIRECTORY)/lib
INCLUDES = $(COIN_INCLUDE) 

# Uncomment to turn on debugging:
#CPPFLAGS = -g -DDEBUG

LIBRARIES =  $(COIN_LIBRARIES) 

OBJECT_FILES = report1.o
EXEC = rt

.C.o:
	$(CPP) -c $(CPPFLAGS) $(COIN_INCLUDE) $<

lab1: $(OBJECT_FILES)
	$(CPP) -o $(EXEC) $(CPPFLAGS) $(OBJECT_FILES) \
        $(LDFLAGS) $(LIBRARIES)

clean:
	rm *.o $(EXEC)
