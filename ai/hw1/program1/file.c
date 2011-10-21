#include "file.h"
#include <stdlib.h>
int* readFile(char* filename, int maxArraySize, int bufferSize){
  int* cv=malloc(sizeof(int)*maxArraySize);
  char buf[bufferSize];
  int counter=0;
  FILE *myFile;

  if (filename==NULL) {
    myFile=stdin;
  } else {
    myFile=fopen(filename,"r");
    if (myFile==NULL) {
      fprintf(stderr,"Error opening file %s, exiting\n",filename);
      exit(1);
    }
  }

  /* beware!  no error checking here! */
  while(fgets(buf,bufferSize-1,myFile)) {
    counter++;
    cv[counter]=atoi(buf);
    cv[0]=counter;
  }
  if (myFile != stdin)
    fclose(myFile);
  return cv;
}
