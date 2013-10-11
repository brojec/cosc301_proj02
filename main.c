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
void print_chararr(char** arg){
	printf("printing array of char*s:\n");
	if(arg==NULL){
		printf("Null array");
		return;
	}
	int i = 0;
	while(arg[i]!=NULL){
		printf("%s\n", arg[i]);
		i++;
	}
	
	printf("%s\n",arg[i]);
}

//Brett
/*
Takes a char* token from tokenify() and cleans it up so that it can be easily used by execv() and handle_commands().  Returns an array of chars containing the program path & all options.  
*/
char** parse_tokens(char* token){
	remove_whitespace(token);
	if(strlen(token)==0){
		return NULL;
	}
	int i=0;
	char c = token[i];
	int counting_space=0;
	int argc = 1;
	while(c!='\0'){
		if(isspace(i) && !counting_space){
			counting_space=1;
		}else if(!isspace(i) && counting_space){
			argc++;
			counting_space = 0;	
		}
		i++;
		c = token[i];
	}
	char* delim = " \t\n\f\v\r"; //the chars checked by isspace()
	char** argv = (char**)malloc((argc+1)*sizeof(char*));
	char* str = strtok(token, delim);
	i=0;
	while(str!=NULL){
		argv[i] = str;
		i++;
		str = strtok(NULL, delim);
	}
	argv[i] = NULL;
	return argv;	

}


//Carrie: haven't tested yet
void run_command_s(char ** arr) { //sequential:
	int i = 0;
	int ret = 0;
	while(arr[i] != NULL) {
		printf("parsing command %s\n", arr[i]);
		char ** arr_for_exec = parse_tokens(arr[i]); //malloced in function
		//first entry should be path name.  Following entries will be options.
		printf("in sequential, in loop\n");
		
		/*if(strcasecmp(arr_for_exec[0],"exit") == 0){ //want to pass over exit commands here
			printf("got an exit\n");
			continue;
		}
		else if(strcasecmp(arr_for_exec[0],"mode") == 0) {
			continue;
		}*/
		if((strcasecmp(arr_for_exec[0],"exit")!=0) && (strcasecmp(arr_for_exec[0],"mode") != 0)) {
		//else {
			pid_t child_pid;
			int child_status;
			child_pid = fork();
			if(child_pid == 0) { //if child:
				ret = execv(arr_for_exec[0], arr_for_exec);
				//if execv returns, that means there was an error
				
				//****is it okay to set a NULL return?
				if(ret== -1) {
					printf("Error: Invalid Command\n");
					exit(0);
				}
			}
			else {
				pid_t tpid = waitpid(child_pid, &child_status, 0); /*wait until child is done */
				if(tpid !=child_pid) { //if not same, soemthing went wrong
					printf("Error with %s\n", arr_for_exec[0]);
				}
			}
		}
		free(arr_for_exec);
		i++;
	}
}

//Carrie: haven't tested yet

void run_command_p(char ** arr) { //parallel:
	int j = 0;
	int ret = 0;
	printf("In a parallel universe...\n");
	while(arr[j] != NULL) { //C: I just need a count of how many commands there are
		j++;
	}
	int ** checkarr = (int **) malloc(sizeof(int *)*j); /*this will be used to keep
	track of the statuses of the child processes as they run in parallel */	
	int i =0;	
	while(arr[i]!=NULL) {
		char ** arr_for_exec = parse_tokens(arr[i]); /* I believe this is malloced
		in the function and includes a remove_whitespace */
		//first entry should be path name.  Following entries will be options.	
		
		/*if(strcmp(arr_for_exec[0],"exit") == 0){ //want to pass over exit commands here
			continue;
		}
		else if(strncasecmp(arr_for_exec[0],"mode",4) == 0) {
			continue;
		}*/
		if((strcasecmp(arr_for_exec[0],"exit")!=0) && (strcasecmp(arr_for_exec[0],"mode") != 0)) {
			pid_t child_pid;
			int child_status;
			child_pid = fork();
			if(child_pid == 0) { //child
				ret = execv(arr_for_exec[0], arr_for_exec);
				//if execv returns, that means there was an error
				if(ret == -1) {
					printf("Error: Invalid Command\n");
					exit(0);
				}
			}
			else { //parent
				pid_t tpid = waitpid(child_pid, &child_status, WNOHANG); //returns 	immediately
				if(tpid == -1) { //if it returns -1, there was an error
					printf("Error with %s\n", arr_for_exec[0]);
				}
				checkarr[i] = &child_status; //this fills checkarr with 	child_statuses
				//not sure if this part will work properly
			}
		}
		free(arr_for_exec); //*** should I be doing this here?
		i++;
	}	
	int count = 0;
	while(checkarr[count] != NULL) {
		pid_t tpid = wait(checkarr[count]); //this SHOULD check that they are all done
		if(tpid == -1) { //if there was an error
			printf("Error occured. \n");
		}
		count++;			
	}
}

//Brett and Mac and Carrie
void handle_commands(char** arr) {
//	printf("Entered handle_commands\n");
	int exitvar = 0;
	char mode = '\0';
	int i = 0;	
	while(arr[i] != NULL){ /*this will purely go through and see if we need to exit or change
	modes when finished with command line.  That way, don't have to pass anything back
	and forth for parallel or sequential code */
		printf("parsing command: %s\n", arr[i]);
		char ** arr_for_exec = parse_tokens(arr[i]); /* I believe this is malloced
		in the function and includes a remove_whitespace */
	//	printf("Arrived at location A\n");
		if(strcasecmp(arr_for_exec[0],"exit") == 0){
			exitvar = 1;
		//	printf("Exitvar was set to 1\n");
		}
		else if(strcasecmp(arr_for_exec[0],"mode") == 0){
			//B: Changed this block so that default is displaying mode, 
			//   to fit w/ project description 
	//		printf("Entered mode else if statement.\n");
	//		printf("exec[0] is %s\n", arr_for_exec[0]);
			if(arr_for_exec[1] != NULL) {
				if(strcasecmp(arr_for_exec[1], "p") ==0) {
					mode = 'p';	
				}
				else if(strcasecmp(arr_for_exec[1], "s") == 0) {
					mode = 's'; 
				}else{
					mode = 'd';
				}
			}
			else {
				mode = 'd';
			}
		}
		i++;
		free(arr_for_exec);
	}
	//This will go carry out the commands
	if(parallel == 0) { //if running sequentially
		run_command_s(arr);
	}
	else { //if running parallel
		run_command_p(arr);
	}
	if(mode == 'p') {
		printf("arrived in p mode, mode set to %d\n", parallel);
		parallel = 1; //global variable indicates operating in parallel
	}
	else if(mode == 's') {
		printf("arrived in s mode, mode set to %d\n", parallel);
		parallel = 0; //global variable indicates operating sequentially
	}
	else if(mode== 'd') {
		printf("arrived in display, mode set to %d\n", parallel);
		if(parallel == 0) {
			printf("The mode is currently sequential.\n");
		}
		else {
			printf("The mode is currently parallel.\n");
		}
	}
	if(exitvar == 1) {
		exit(0);
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

//Mac
char** tokenify(char* str){ //takes line of input from command line, breaking up by semicolons
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
		/*
		charCount = 0;
		int i;		
		for(i = 0; i < strlen(word); i++){ 
			charCount++;
		}
		*/
		charCount = strlen(str);
		char* command = (char*)malloc(charCount*sizeof(char));
		int i;
		for(i = 0; i < strlen(word); i++){ 
			command[i] = word[i];
		}
		//B: ^^can we strdup() this?
		command[strlen(word)] = '\n';//B: Do we want this to be '\0' instead?
		cmds[cmdCount] = command;
		cmdCount++;

	}
	free(s);
	cmds[cmdCount] = NULL;
	return cmds;

}

//Brett
int main(int argc, char **argv) {
	char* input = (char*) malloc(sizeof(char)*255);
	printf(">>>");
	while(fgets(input, 255, stdin)!=NULL){
		char ** cmds = tokenify(input);
		handle_commands(cmds);
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

