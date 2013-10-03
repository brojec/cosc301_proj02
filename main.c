/*
	cosc301 project 2
	October, 2013
	Contributers:
		Carrie Burgess
		Mac Lanphier
		Brett Rojec
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>

void mode() {
	//this will change the mode to either parallel or sequential
	//sequential: use waitpd() call
	//parallel: use execv and fork.... should automatically run in parallel?
}

void exit() {
	//this exits everything
	exit;
}

char** deligate(const char** arr) {
	//we can definitely rename this...
	
	//this should take in an array of pointers from tokenify
	//first, comb through to find comments.  Change comment to null character
	//next, go through tokens, break into commands based on semicolons 
		//if 'mode' or 'exit', call own code (although they don't take priority
		//over code that was before it on same line)
		//make sure to check character after 'mode' to see if either a 's', 'p'
		//or says 'parallel' or 'sequential'
	//for rest, check if command/process, or if an option (ex -c)
	//use execv (?) to make process work
	
	return NULL;
}

char** tokenify(const char* str){
	//break str up into appropriate tokens,
	//and return an array of the results
	
	//string should include no tabs/ spaces
	
	//might want to reuse code...
	return NULL;
}

int main(int argc, char **argv) {
	char* input = (char*) malloc(sizeof(char)*255);
	printf(">>>");
	//might want another while loop to recursively keep asking user what wants
	//until get EOF/ ctrl+D signal
	while(fgets(input, 255, stdin)!=NULL){
		printf("You entered: %s (length %d) \n", input, (int)strlen(input));
		printf(">>>");
	}
	printf("\n");
	return 0;
}

