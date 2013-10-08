/*
	cosc301 project 2
	October, 2013
	Contributers:
		Carrie Burgess
		Zombie bastard (Mac Lanphier)
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

int parallel=0; /*B: flag for what mode we're running in
	          1: process things in parallel (actually, any nonzero)
	          0: process sequentially */

//Brett
int is_space_or_semi(char target){
	return isspace(target) || target==';';
}


//Brett & Carrie
void remove_whitespace(char* str){
	//removes leading & trailing whitespaces and ';'s in str in-place. 
	int first_char = 0; //index of first non semicolon or whitespace character
	int last_char = 0; //index of the last non semicolon or whitespace character
	int seen_char = 0;
	int i=0;
	for(;str[i]!='\0';i++){
		if(!is_space_or_semi(str[i])){
			if(!seen_char){
				first_char = i;
				seen_char = 1;
			}
			last_char = i;
		}
	}
	
	if(first_char==last_char && !seen_char){
		str[first_char] = '\0';
		return;
	}
	
	int z = 0; 
	for(i=first_char;i<=last_char;i++){
		str[z] = str[i];
		z++;
	}
	str[z] = '\0';
}

//Brett
/*
Takes a char* token from tokenify() and cleans it up so that it can be easily used by execv() and handle_commands().  Returns an array of chars containing the program path & all options.  
*/
char** parse_tokens(char* token){
	remove_whitespace(token);
	

}

//Carrie: haven't tested yet
void run_command_s(char ** arr_for exec) { //sequential:
	//this will have been malloced in parse_tokens
	//first entry should be path name.  Following entries will be options.
	pid_t child_pid;
	int child_status;
	child_pid = fork();
	if(child_pid == 0) { //if child:
		execv(arr_for_exec[0], arr_for_exec);
		//if execv returns, that means there was an error
		printf("Error: Invalid Command\n");
		exit(0);
	}
	else {
		pid_t tpid = waitpid(&child_status); //wait until child is done
		if(tpid !=child_pid) { //if not same, soemthing went wrong
			printf("Error with \s\n", arr_for_exec[0]);
		}
	}
}

//Carrie: haven't tested yet

//Question: I want to access the address of child_Status.  Should I pass back the int, or a pointer?
int child_status run_command_p(char ** arr_for_exec) { //parallel:
	//this will have been malloced in parse_tokens
	//first entry should be path name.  Following entries will be options.
	pid_t child_pid;
	int child_status;
	child_pid = fork();
	if(child_pid == 0) {
		execv(arr_for_Exec[0], arr_for_exec);
		//if execv returns, that means there was an error
		printf("Error: Invalid Command\n");
		exit(0);
	}
	else {
		pid_t tpid = waitpid(&child_status, WNOHANG); //returns immediately
		if(tpid == -1) {
			printf("Error with \s\n", arr_for_exec[0]);
		}
	}
}

//Brett and Mac and Carrie
void handle_commands(char** arr) {
		//if 'mode' or 'exit', call own code (although they don't take priority
		//over code that was before it on same line)
		//make sure to check character after 'mode' to see if either a 's', 'p'
		//or says 'parallel' or 'sequential'
	//for rest, check if command/process, or if an option (ex -c)
	//use execv (?) to make process work*/
	int exit = 0;
	char mode = '\0';
	int i = 0;
	int tofree = 0;
	if(parallel !=0) { //if running in parallel: doing in if statement so don't needlessly
	//malloc any space.  Variable tofree is freed at bottom if we enter this if statement
		int j = 0;
		while(arr[j] != NULL) { //C: I just need a count of how many commands there are
			j++;
		}
		(int *) checkarr = (int *) malloc(sizeof(int)*j); /*this will be used to keep
 		track of which processes are done for parallel */
 		int tofree = 1;
 		int k = 0;
 	}	
	while(arr[i] != NULL){
		remove_whitespace(arr[i]);
		if(strcmp(arr[i],"exit") == 0){
			exit = 1;
		}
		else if(strncasecmp(arr[i],"mode",4) == 0){
			//B: Changed this block so that default is displaying mode, 
			//   to fit w/ project description 
			if(strcasecmp(arr[i][5], "p") ==0) {
			//if(arr[i][5] == 'p'){
				mode = 'p';
				
			}
			else if(strcasecmp(arr[i][5], "s") == 0) {
			//else if(arr[i][5] == 's'){
				mode = 's'; 
			}
			else{
				mode = 'd';
			}
		}
		else{
			char** arr_for_exec = parse_tokens(arr[i]);
			if(parallel == 0) { //if running sequentially
				run_command_s(arr_for_exec);
			}
			else { //if running parallel
				//created an array of ints as long as num of entries
				//have waitpid for each entry.  Once all of them are 0, can continue
				child_status = run_command_p(arr_for_exec);
				checkarr[i] = child_status; //this fills checkarr with child_statuses
			}
			free(arr_for_exec);
		}	
	}
	int count = 0;
	while(checkarr[count] != NULL) {
		pid_t tpid = wait(&checkarr[count]); //this SHOULD check that they are all done
		if(tpid == -1) { //if there was an error
			printf("Error occured. \n");
		}
		count++;			
	}
	if(mode == 'p') {
		parallel = 1; //global variable indicates operating in parallel
	}
	else if(mode == 's') {
		parallel = 0; //global variable indicates operating sequentially
	}
	else if(mode== 'd') {
		if(parallel == 0) {
			printf("The mode is currently sequential.\n");
		}
		else {
			printf("The mode is currently parallel.\n");
		}
	}
	if(tofree == 1) {
		free(checkarr);	
	}	
}


//Brett & Carrie
int num_toks(char* str){
	if(str==NULL || strlen(str)==0)
		return 0;
	int toks = 1;
	char c;
	int i = 0;
	for(c=str[i];c!='\0';c=str[++i]){
		if(c==';'){
			if(str[i+1]!='\0' && str[i+1]!=';')
				toks++;
		}
	}
	return toks;
}

//Carrie
/*
Takes a string str and replaces the first '#' character 
with a null-terminator, efectively chopping off the '#'
and everything after it.  
*/
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
	remove_whitespace(str);

	int tokCount = num_toks(str);
	char** cmds = (char**)malloc(sizeof(char*)*(tokCount+1));

	
	printf("number of tokens in %s: %d\n", str, tokCount);
	
	
	
	const char* sep = ";\n";
	char* tmp;
	char* s = strdup(str);
	char* word= strtok_r(s,sep,&tmp);
	int charCount;
	int cmdCount = 0;
	for(; word != NULL; word = strtok_r(NULL,sep,&tmp)){
		charCount = 0;
		int i;		
		for(i = 0; i < strlen(word); i++){ 
			charCount++;
		}
		char* command = (char*)malloc(charCount*sizeof(char));
		for(i = 0; i < strlen(word); i++){ 
			command[i] = word[i];
		}
		command[strlen(word)] = '\n';
		cmds[cmdCount] = command;
		cmdCount++;

	}
	free(s);
	cmds[cmdCount] = NULL;
	return cmds;

}

//Brett
int main(int argc, char **argv) {
	printf("argc: %d\n", argc);
	int i=0;
	for(;i<argc;i++){
		printf("argument %d: %s\n",i,argv[i]);
	}
	char* input = (char*) malloc(sizeof(char)*255);
	printf(">>>");
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


/*
Braaaaaaaaaiiiiiiiiiiiiiiins

	-Zombies out
*/

