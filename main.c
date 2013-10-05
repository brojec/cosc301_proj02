/*
	cosc301 project 2
	October, 2013
	Contributers:
		Carrie Burgess
		Zombie bastard
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

int parallel=0; /*B: I don't think mode() is needed, just use this global int (bool)
	          1: process things in parallel (actually, any nonzero)
	          0: process sequentially */
void mode() {
	//this will change the mode to either parallel or sequential
	//sequential: use waitpd() call
	//parallel: use execv and fork.... should automatically run in parallel?
}

void exit() {
	//exit normally (also, this may not need its own function)
	exit(0);
}

char** handle_commands(const char** arr) {
	//C: we can definitely rename this...
	//B: I did, I called it handle_commands() in main().
	
	/*C:
	//this should take in an array of pointers from tokenify
	//first, comb through to find comments.  Change comment to null character
	//next, go through tokens, break into commands based on semicolons 
		//if 'mode' or 'exit', call own code (although they don't take priority
		//over code that was before it on same line)
		//make sure to check character after 'mode' to see if either a 's', 'p'
		//or says 'parallel' or 'sequential'
	//for rest, check if command/process, or if an option (ex -c)
	use execv (?) to make process work*/
	
	return NULL;
}

int num_toks(const char* str){
	char* str2 = strdup(str); //strtok() fucks w/ input, so copy str
	int toks = 0;
	if(strtok(str2, ";")!=NULL){
		toks++;
		while(strtok(NULL, ";")!=NULL)
			toks++;
	}
	free(str2);
	return toks;
}

char** tokenify(const char* str){
	//break str up into appropriate tokens,
	//and return an array of the results
	
	//string should include no tabs/ spaces //B: why?
	
	//might want to reuse code...
	printf("number of tokens in %s: %d\n", str, num_toks(str));
	
	return NULL;
}

int main(int argc, char **argv) {
	char* input = (char*) malloc(sizeof(char)*255);
	printf(">>>");
	//C: might want another while loop to recursively keep asking user what wants
	//until get EOF/ ctrl+D signal <--that's what this does ~Brett
	while(fgets(input, 255, stdin)!=NULL){
		handle_commands(tokenify(input));
		printf(">>>");
	}
	printf("\n");
	return 0;
}

