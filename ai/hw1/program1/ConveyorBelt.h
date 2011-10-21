#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int ConveyorBelt;
#define BUFSIZE 1024
#define MAX_CONVEYOR_SIZE 100
#define ADVANCE 1
#define PICK_UP 2
#define SHUT_DOWN 3

#define ITEM_LIGHTER 1
#define ITEM_HEAVIER 2
#define ITEM_FINISH 3
char ACTIONS [4][10] ={"INVALID OP", "ADVANCE", "PICK UP", "SHUTDOWN"};
