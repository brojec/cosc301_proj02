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

char** tokenify(const char* str){
	//break str up into appropriate tokens,
	//and return an array of the results
}

int main(int argc, char **argv) {
	char* input;
	while(fgets(input, 255, stdin)!=NULL){
		printf("You entered: %s\n", input);
	}
	return 0;
}

