#include "ConveyorBelt.h"
#include "file.h"

ConveyorBelt* makeConveyorBelt(char *filename) {
  return readFile(filename, MAX_CONVEYOR_SIZE, BUFSIZE);
}

int getItem(ConveyorBelt *cv,int index) {
  return (int)(cv[index+1]);
}

void setItem(ConveyorBelt *cv,int index, int value) {
  cv[index+1] = value;
  return;
}
int getSize(ConveyorBelt *cv) {
  return (int)(cv[0]);
}
int min( int a, int b){
	//printf(" min (%d, %d )\n", a, b);
	return (a < b ? a: b);
}
int interpretInput (int belta_current, int belta_next, int beltb_current, int beltb_next, int power) {	
	int rule = ITEM_NEITHER;
	int currentMax =0;
	int belt = 0;
	int item = 0;
	
	if (pickedUpItem != EMPTY) rule = ITEM_PICKED;
	else if (power == 0) rule = POWER_EMPTY;
/*
	After an item is picked up, the current position turns empty or 0.
	To have an optimal solution, we need to advance that that belt so that our future decisions are based 
	on maximum number of valid cadidates to pick from. The below two conditions hence raise a rule that 
	the current item is empty, but the end of the belt has not yet been reached.
*/
	else if (belta_current == 0 && belta_next != -1) rule = ITEM_A_EMPTY;
	else if (beltb_current == 0 && beltb_next != -1) rule = ITEM_B_EMPTY;
/* 
	If neither of the above conditions are satisfied, then it means that we have a state where we have 
	4 valid candidates to compare with the currently holding items.
*/
	else {
		if(belta_current > currentMax && (belta_current > Item1 || belta_current > Item2)) { belt = BELT_A; item = ITEM_CURRENT; currentMax = belta_current; } 
		if(beltb_current > currentMax && (beltb_current > Item1 || beltb_current > Item2)) { belt = BELT_B; item = ITEM_CURRENT; currentMax = beltb_current; } 
		if(belta_next > currentMax && (belta_next > Item1 || belta_next > Item2)) { belt = BELT_A; item = ITEM_NEXT; currentMax = belta_next; }
		if(beltb_next > currentMax && (beltb_next > Item1 || beltb_next > Item2)) { belt = BELT_B; item = ITEM_NEXT; currentMax = beltb_next; }

		if(currentMax != 0){
			if(belt == BELT_A){
				if (item == ITEM_NEXT) { 
/*
							 this is a check for a special case, when one belt is
							 repeatedly being advanced because successive elements 
							are greater than the previous ones. we need to stop and 
							maximize the items holding before the power becomes 0
							Hence, instead of marking the next item as heavier, 
							we mark the current item as heavier if power == 2
*/
							 if (power > 2)
							    rule = ITEM_A_NEXT_HEAVIER; 
							 else if(previousAction == ADVANCE_A && power == 2)
							    rule = ITEM_A_CURRENT_HEAVIER; 
							 else
							    rule = ITEM_A_CURRENT_HEAVIER;
							}
				else 			rule = ITEM_A_CURRENT_HEAVIER; 
			} else {
				if (item == ITEM_NEXT) {
//							printf("====> %d, %d ,%d \n", previousAction, item, belt);
							 if (power > 2)
							    rule = ITEM_B_NEXT_HEAVIER; 
							 else if(previousAction == ADVANCE_B && power == 2)
							    rule = ITEM_B_CURRENT_HEAVIER; 
							 else
							    rule = ITEM_B_CURRENT_HEAVIER;
							}
				else 			rule = ITEM_B_CURRENT_HEAVIER;
			}
		}
	}
	if(rule == ITEM_NEITHER){
/*
		ITEM_NEITHER is raised when none of the above conditions pass. This could either mean, 
		either of the belts are empty, or both of the belts are empty
*/		
		if (belta_current == -1 && beltb_current == -1) rule = BELTS_EMPTY;
		else if (belta_next == -1 && beltb_next == -1) rule = BELTS_EMPTY;
		else if (belta_current == -1 || belta_next == -1) rule = BELTA_EMPTY;
		else if (beltb_current == -1 || beltb_next == -1) rule = BELTB_EMPTY;
	}
//	if (currentMax != 0 && power < 2)i
//		rule = POWER_LOW;
	return rule;
}

int ruleMatch(int rule){
	int action;
//	printf("Rule = %d\n", rule);
	switch (rule) {
		case ITEM_PICKED:
			action = Item1 == DROPPED ? FILL_ITEM_1 : FILL_ITEM_2;
			break;
		case ITEM_A_EMPTY:
			action = ADVANCE_A; break;
		case ITEM_B_EMPTY:
			action = ADVANCE_B; break;
		case BELTA_EMPTY:
			action = ADVANCE_B; break;
		case BELTB_EMPTY:
			action = ADVANCE_A; break;
		case ITEM_NEITHER:
//			printf("rand\n");
/*
		This case is when the agent can't decide on the next action. It happens when 
		states the of the belts are such that none of the items on the  belt are heavier 
		than either of the Items being held currently. Hence we advance a belt randomly.
*/
			action = (ADVANCE_A + (rand() % 2)); break;

		case ITEM_A_CURRENT_HEAVIER:
			if(Item2 != DROPPED && Item1 != DROPPED)
				action = (Item1 > Item2) ? DROP_ITEM_2: DROP_ITEM_1; 
			else if ( pickedUpItem == EMPTY)
				action = PICK_UP_A;
			break;
		case ITEM_B_CURRENT_HEAVIER:
			if(Item2 != DROPPED && Item1 != DROPPED)
				action = (Item1 > Item2) ? DROP_ITEM_2: DROP_ITEM_1;
			else if ( pickedUpItem == EMPTY)
				action = PICK_UP_B;
			break;
		case ITEM_A_NEXT_HEAVIER:
			action = ADVANCE_A; break;
		case ITEM_B_NEXT_HEAVIER:
			action = ADVANCE_B; break;
		case POWER_LOW:
			action = SHUT_DOWN; break;
		case POWER_EMPTY:
			action = SHUT_DOWN; break;
		case BELTS_EMPTY:
			action = SHUT_DOWN; break;
		default: break;
	}
	return action;
}

int performAction(ConveyorBelt  *cb1, ConveyorBelt *cb2, int action, int *currentPositionOfA, int *currentPositionOfB){
	int powerChange = 0;
	printf( "OUTPUT ACTION: %s\n", ACTIONS[action]);
	switch(action){
		case SHUT_DOWN:
			powerChange = 0;
			break;
		case FILL_ITEM_1:
			Item1 = pickedUpItem; pickedUpItem = EMPTY;
			minOfItemFilled = min (Item1, Item2);
			pickedUpState = NO;
			powerChange = 0;
			break;
		case FILL_ITEM_2:
			Item2 = pickedUpItem; pickedUpItem = EMPTY;
			minOfItemFilled = min (Item1, Item2);
			pickedUpState = NO;
			powerChange = 0;
			break;
		case ADVANCE_A:
			(*currentPositionOfA)++;
			powerChange = -1;
			break;
		case ADVANCE_B:
			(*currentPositionOfB)++;
			powerChange = -1;
			break;
		case PICK_UP_A:
			pickedUpItem = getItem(cb1, *currentPositionOfA);
			setItem(cb1, *currentPositionOfA, 0);
			powerChange = -1;
			pickedUpState = YES;
			break;
		case PICK_UP_B:
			pickedUpItem = getItem(cb2, *currentPositionOfB);
			setItem(cb2, *currentPositionOfB, 0);
			powerChange = -1;
			pickedUpState = YES;
			break;
		case DROP_ITEM_1:
			Item1 = 0;
			powerChange = -1;
			break;
		case DROP_ITEM_2:
			Item2 = 0;
			powerChange = -1;
			break;
		default:
			break;
			
	}
	previousAction = action;
	printf ("Items currently held: %d %d\n\n", Item1, Item2);
	return powerChange;
	
}
int ReflexAgentWithState(int belta_current, int belta_next, int beltb_current, int beltb_next, int power){
	//interpret input
	//rulematchi
	//return action;
	int rule, action;
	printf("Agent State : Min. of Items Filled -> %d, Is an Item Picked -> %s, Power Remaining -> %d, Previous Action: %s\nPercepts : A(Current) -> %d, A(Next) -> %d, B(Current) -> %d, B(Next) -> %d\n", minOfItemFilled,( pickedUpState ? "Yes" : "No"), power, ACTIONS[previousAction], belta_current, belta_next, beltb_current, beltb_next);
	rule = interpretInput (belta_current, belta_next, beltb_current, beltb_next, power);;
	action = ruleMatch(rule);
	return action;
}

void testHarness (ConveyorBelt  *cb1, ConveyorBelt *cb2){
	int action = -1;
	int i = 0;
	int j = 0;
	int power = POWER_TOTAL;
	int belta_current = 0 ;
	int belta_next = 0; 
	int beltb_current = 0;
	int beltb_next = 0;
	int powerChange = 0;
//	char t;
	// while action!= shutdown provide input to reflexagent
	while(action !=  SHUT_DOWN){		
		belta_current = getItem(cb1, i);
		beltb_current = getItem(cb2, j);
		belta_next = getItem(cb1, i+1);
		beltb_next = getItem(cb2, j+1);
		printf("INPUT PERCEPTION: %d %d %d %d %d\n", belta_current, belta_next, beltb_current, beltb_next, power);
		action = ReflexAgentWithState(belta_current, belta_next, beltb_current, beltb_next, power); 
		powerChange = performAction(cb1, cb2, action, &i, &j);	
		power += powerChange;
		//t = getchar();
	}
}

int main(int argc, char **argv) {
	ConveyorBelt *cb1;
	ConveyorBelt *cb2;
  	
	if (argc==3) {
		cb1 = (ConveyorBelt *) makeConveyorBelt(argv[1]);
		cb2 = (ConveyorBelt *) makeConveyorBelt(argv[2]);
	} else {
		printf("USAGE ERROR:\n Should be of the form ./solution inputfile1 inputfile2\n");
		return(0);
	}


	testHarness(cb1, cb2);  

	free(cb1);
	free(cb2);
	return(0);
}
 
