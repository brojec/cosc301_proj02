/*
	cosc301 project 2
	October, 2013
	Contributers:
		Carrie Burgess
		Zombie bastard
		Brett Rojec
		
NB:
	We're trying to do string modifications in-place, to avoid pointer confusion
	and memory leaks & annoying stuff.  Plus I think it's generally cleaner & 
	no more difficult
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

//Brett
int is_space_or_semi(char target){
	return isspace(target) || target==';';
}

//Brett & Carrie
void remove_whitespace(char* str){ //something doesn't work yet
	//removes leading & trailing whitespaces and ';'s in str in-place. 
	char* strcpy = strdup(str);
	int first_char = 0; //index of first non semicolon or whitespace character
	int last_char = 0; //index of the last non semicolon or whitespace character
	int i=0;
	for(;str[i]!='\0';i++){
		if(!is_space_or_semi(str[i])){
			if(!first_char)
				first_char = i;
			last_char = i;
		}
	}
	if(first_char==last_char)
		str[first_char] = '\0';
		return;
	int z = 0; //short for jerk Zombie
	for(i=first_char;i<=last_char;i++){
		str[z] = strcpy[i];
		z++;
	}
	str[z] = '\0';
	free(strcpy);
}

//Brett & Carrie
int num_toks(char* str){
	remove_whitespace(str);
	if(str==NULL || strlen(str)==0 || str[0]=='#')
		return 0;
	int toks = 1;
	char c;
	int i = 0;
	for(c=str[i];c!='\0';c=str[++i]){
		if(c==';'){
			if(str[i+1]!='\0' && str[i+1]!=';')
				toks++;
		}
		if(c=='#')
			break;
	}
	return toks;
}

//Carrie
void nullcomment(char* str) {
	int i = 0;
	for(;str[i]!='\0';i++) {
		if(str[i]=='#') {
			str[i]='\0';
			break;
		}
	}
}

char** tokenify(char* str){
	nullcomment(str);
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
	free(input);
	return 0;
}


/*
Journal entry 1:
	It's day 1 of the zombie attacks.  The infestation has started on campus, and I'm afraid one of our coding teammates was... was infected.  We don't know what to do.  Also, all my boyfriend does is think about the code, instead of surviving the goddamn apocalypse.  
	Anyway....we have food, for survival.  Like indian curry.  (Shows we are real legit survivors).  We have weapons for protection; our brightly colored nerf guns will surly spark fear in both the zombie's brains and the brains they are feasting on (those turning zombie).  We've decided the only alternate form of attack is to add side remarks in our code commenting.  Other then that... well, let's just hope we find the cure, and hope that the Zombies' computer viruses aren't as virulent as their biological ones.  
	We must go and gather our wits.  We have a lot of work to do, in a very short period of time.  And an apocalypse to think about on the side.
	
	-Survivors out.
*/
