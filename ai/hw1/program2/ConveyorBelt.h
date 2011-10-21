#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int ConveyorBelt;
#define BUFSIZE 1024
#define MAX_CONVEYOR_SIZE 100

//ACTIONS
#define ADVANCE_A 1
#define ADVANCE_B 2
#define PICK_UP_A 3
#define PICK_UP_B 4
#define DROP_ITEM_1 5
#define DROP_ITEM_2 6
#define FILL_ITEM_1 7
#define FILL_ITEM_2 8
#define SHUT_DOWN 9

// RULES
#define ITEM_NEITHER 0
#define ITEM_A_EMPTY 1
#define ITEM_B_EMPTY 2
#define ITEM_A_CURRENT_HEAVIER 3
#define ITEM_B_CURRENT_HEAVIER 4
#define ITEM_A_NEXT_HEAVIER 5
#define ITEM_B_NEXT_HEAVIER 6
#define ITEM_A_FINISH 9
#define ITEM_B_FINISH 10
#define ITEM_PICKED 13
#define POWER_EMPTY 11
#define POWER_LOW 14
#define BELTS_EMPTY 12
#define BELTA_EMPTY 15
#define BELTB_EMPTY 16

// constans
#define BELT_A 1
#define BELT_B 2
#define ITEM_CURRENT 1
#define ITEM_NEXT 2
#define DROPPED 0
#define EMPTY -1
#define YES 1
#define NO 0
#define POWER_TOTAL 20
char ACTIONS [10][20] ={"NO_OP", "ADVANCE_A", "ADVANCE_B", "PICK UP_A", "PICK_UP_B", "DROP_ITEM_1", "DROP_ITEM_2", "FILL_ITEM_1", "FILL_ITEM_2", "SHUTDOWN"};

//States
int Item1 = 0;
int Item2 = 0;

int previousAction = 0;
int pickedUpState = 0;
int pickedUpItem = -1;
int minOfItemFilled = 0; 
