#include "ConveyorBelt.h"
#include "file.h"

ConveyorBelt* makeConveyorBelt(char *filename) {
  return readFile(filename, MAX_CONVEYOR_SIZE, BUFSIZE);
}

int getItem(ConveyorBelt *cv,int index) {
  return (int)(cv[index+1]);
}

int getSize(ConveyorBelt *cv) {
  return (int)(cv[0]);
}

int interpretInput(int currentItem, int nextItem){
	int rule = -1;
	if(currentItem > nextItem && currentItem != 0) rule = ITEM_HEAVIER;
	else if (currentItem == -1) rule = ITEM_FINISH;
	else rule = ITEM_LIGHTER;
	return rule;
}

int ruleMatch(int rule){
	int action;
	switch (rule) {
		case ITEM_LIGHTER:
			action = ADVANCE; break;
		case ITEM_FINISH:
			action = SHUT_DOWN; break;
		case ITEM_HEAVIER:
			action = PICK_UP; break;
		default: break;
	}
//	action = rule;
	return action;
}

int performAction(ConveyorBelt  *cv, int action, int currentPosition){
	printf( "OUTPUT ACTION: %s\n", ACTIONS[action]);
	if (action == ADVANCE) currentPosition++;
	else if (action == PICK_UP) cv[currentPosition+1] = 0;
	return currentPosition;
}

int ReflexAgent(int percept1, int percept2){
	//interpret input
	//rulematch
	//return action;
	int rule, action;
	rule = interpretInput (percept1, percept2);
	action = ruleMatch(rule);
	return action;
}

void testHarness (ConveyorBelt  *cv){
	// while action!= shutdown provide input to reflexagent
	int action = -1;
	int i = 0;

	while(action !=  SHUT_DOWN){		
		printf("INPUT PERCEPTION: %d %d\n", getItem(cv, i), getItem(cv, i+1));
		action = ReflexAgent ( getItem(cv,i) , getItem(cv, i+1) );
		i = performAction(cv, action, i);	
	}
}

int main(int argc, char **argv) {
	ConveyorBelt *cv;
  	
	if (argc==2) {
		cv = (ConveyorBelt *) makeConveyorBelt(argv[1]);
	} else {
		printf("USAGE ERROR:\n./solution inputfile\n");
		return(0);
	}

	testHarness(cv);  

	free(cv);
	return(0);
}
 
