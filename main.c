/*
	cosc301 project 2
	October, 2013
	Contributers:
		Carrie Burgess
		Zombie Bastard (Mac Lanphier)
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

char** tokenify(char*, const char*);

int parallel=0; /*B: flag for what mode we're running in
	          1: process things in parallel (actually, any nonzero)
	          0: process sequentially */

char* whitespace = " \n\v\t\r\f";	         
	         

//Brett
int is_space_or_semi(char target){
	return isspace(target) || target==';';
}

//Carrie

struct node { //will create a linked list of processes
	char ** arr_for_exec;
	pid_t child_pid;
	int * child_status;
	struct node* next;
};

//Carrie
void check_process_p(struct node * head) {
/*  Do the if statements for if node is first or last, blah blah
cycle through nodes, check if waitpid(childpid, child_status, WNOHANG) is zero
if it's zero, it's done.  Print out that it's done.
Delete child from linked list
*/
	printf("\nchecking the processes\n");
	struct node * temp;
	struct node * curr = head;
	if(head==NULL) {
		printf("There are no processes currently running.\n");
	}
	int check = waitpid(curr->child_pid, (curr->child_status), WNOHANG); //check first node
	printf("\nCheck is %d",check);
	printf("\n");
	if(check == -1) {
		printf("\nError with child");
		free(curr);
	}
	else if(check != 0) {
		temp = head;
		head = head-> next;
		printf("Proccess call '%s' has finished.\n", (temp->arr_for_exec)[0]);
		free(temp);
	}
	while(curr->next != NULL) {
		//printf("\nWaiting for SOMETHING");
		int check = waitpid(curr->next->child_pid, (curr->next->child_status), WNOHANG);
		printf("\nCheck is %d",check);  //This apparently doesnt happen?
		if(curr->next->next != NULL) { //if in middle
			if(check == -1) { //if error
				printf("Error with %s\n", (curr->next->arr_for_exec)[0]);
				temp = curr -> next;
				curr-> next = curr -> next -> next;
				free(temp);
			}
			else if(check != 0) { //if process is done
				temp = curr->next;
				curr->next = curr -> next -> next;
				printf("Proccess call '%s' has finished.\n",(temp->arr_for_exec)[0]);
				free(temp);
			}
		}
		else { //if curr->next->next == NULL: if at end
			if(check == -1) {
				printf("Error with %s\n", (curr->next->arr_for_exec)[0]);
				free(curr->next);
				curr->next = NULL;
			}
			else if(check !=0) {
				printf("Proccess call '%s' has finished.\n",(curr->next->arr_for_exec)[0]);
				free(curr->next);
				curr->next = NULL;
			}
		}
	curr = curr->next;		
	}	
}

//Brett & Carrie
void remove_whitespace(char* str, char* delim){
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



//Carrie: haven't tested yet
void run_command_s(char ** arr) { //sequential:
	int i = 0;
	int ret = 0;
	while(arr[i] != NULL) {
		printf("parsing command %s\n", arr[i]);
		char ** arr_for_exec = tokenify(arr[i],whitespace); //malloced in function
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
				//printf("\nIm a child!");
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

//Carrie, with linked list
void run_command_p(char ** arr, struct node * head) { //parallel:


	//need to have case to initialize linked list, and need to pass in potential head
	printf("\nRunning commands in parallel");
	printf("\n");
	int ret = 0;
	int i =0;
	struct node* newnode = (struct node*)malloc(sizeof(struct node));	
	while(arr[i]!=NULL) {
		char ** arr_for_exec = tokenify(arr[i],whitespace); /* I believe this is malloced
		in the function and includes a remove_whitespace */
		//first entry should be path name.  Following entries will be options.	
		if((strcasecmp(arr_for_exec[0],"exit")!=0) && (strcasecmp(arr_for_exec[0],"mode") != 0)) {
			pid_t child_pid;
			int child_status;
			child_pid = fork();
			printf("\nPID = %d",child_pid);
			if(child_pid == 0) { //child
				printf("\nCHILD SHOULD DIE");
				printf("\n");
				ret = execv(arr_for_exec[0], arr_for_exec);
				//if execv returns, that means there was an error
				int status;
				pid_t result = waitpid(child_pid, &status, WNOHANG);
				if (result == 0) {
  					printf("\nChild still alive");
				} else if (result == -1) {
 					 printf("\nError"); 
				} else {
					 printf("\nChild exited");
				}
				if(ret == -1) {
					printf("Error: Invalid Command\n");
					exit(0);
				}
				newnode->arr_for_exec = arr_for_exec;
				newnode->child_pid = child_pid;
				newnode->child_status = &child_status;
				if(head == NULL) {
					head = newnode;
					head -> next = NULL;
				}
				else{
					newnode -> next = head;
					head = newnode;
				}
			//need to add node to an existing linked list....
			}
		}
		free(arr_for_exec); //*** should I be doing this here?
		i++;
	}
	//printf("\nGoing to check_process_p with PID of %d",child_pid);
	check_process_p(head);
	
	
}



//Brett and Mac and Carrie
void handle_commands(char** arr, struct node * head) {
//	printf("Entered handle_commands\n");
	int exitvar = 0;
	char mode = '\0';
	int i = 0;	


	while(arr[i] != NULL){ /*this will purely go through and see if we need to exit or change
	modes when finished with command line.  That way, don't have to pass anything back
	and forth for parallel or sequential code */
		printf("passing %s into tokenify\n", arr[i]);
		printf("parsing command: %s\n", arr[i]);
		char ** arr_for_exec = tokenify(arr[i],whitespace); /* I believe this is malloced
		in the function and includes a remove_whitespace */
	//	printf("Arrived at location A\n");
		printf("For first arg from parse tokens, got %s\n", arr_for_exec[0]);
		printf("For second arg from parse tokens, got %s\n", arr_for_exec[1]);

		if(strcasecmp(arr_for_exec[0],"exit") == 0){
			exitvar = 1;
		//	printf("Exitvar was set to 1\n");
		}

		else if(strcasecmp(arr_for_exec[0],"mode") == 0){

			//B: Changed this block so that default is displaying mode, 
			//   to fit w/ project description 
		//	printf("Entered mode else if statement.\n");
			printf("\narr_for_exec[0] is %s", arr_for_exec[0]);
			printf("\narr_for_exec[1] is %s", arr_for_exec[1]);

			if(arr_for_exec[1] != NULL) {
				if(strcasecmp(arr_for_exec[1], "p") ==0 || strcasecmp(arr_for_exec[1], "parallel") ==0) {
					mode = 'p';	
				}
				else if(strcasecmp(arr_for_exec[1], "s") == 0 || strcasecmp(arr_for_exec[1], "sequential") ==0) {
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
		printf("\nfreeing");
		print_chararr(arr_for_exec);
		printf("\nsizeof arr_for_exec: %lu", sizeof(arr_for_exec));
		free(arr_for_exec);
		printf("\nsuccessfully freed");
		
	}
	//This will go carry out the commands
	if(parallel == 0) { //if running sequentially
		run_command_s(arr);
	}
	else { //if running parallel
		run_command_p(arr, head);
		//printf("\nWe stop runing run_command_p");
	}
	if(mode == 'p') {
		
		parallel = 1; //global variable indicates operating in parallel
		printf("\narrived in p mode, mode set to %d\n", parallel);
	}
	else if(mode == 's') {
		
		parallel = 0; //global variable indicates operating sequentially
		printf("\narrived in s mode, mode set to %d\n", parallel);
	}
	else if(mode== 'd') {
		printf("\narrived in display, mode set to %d\n", parallel);
		if(parallel == 0) {
			printf("\nThe mode is currently sequential.\n");
		}
		else {
			printf("\nThe mode is currently parallel.\n");
		}
	}
	if(exitvar == 1) {
		exit(0);
	}	
}


//Brett & Carrie
//Brett & Carrie
int num_toks(char* str,const char* delim){  //remove delimiter tokens from front or end
	if(str==NULL || strlen(str)==0)
		return 0;
	
	int toks = 0;
	char* copy = strdup(str);
	char* s = strtok(copy, delim);
	for(;s!=NULL;s = strtok(NULL, delim))
		toks++;
	free(copy);
	return toks;
}

//Carrie
/*
Takes a string str and replaces the first '#' character 
with a null-terminator, efectively chopping off the '#'
and everything after it.  
*/
void nullcomment(char* str) {
	if(str==NULL || strlen(str)==0)
		return;
	int i = 0;
	for(;str[i]!='\0';i++) {
		if(str[i]=='#') {
			str[i]='\0';
			break;
		}
	}
}

//Mac & Brett
char** tokenify(char* str, const char* delim){ 
//takes line of input from command line, breaking up by semicolons
	nullcomment(str);
	remove_whitespace(str,whitespace);

	int tokCount = num_toks(str,delim);
	char** cmds = (char**)malloc(sizeof(char*)*(tokCount+1));

	
//	const char* sep = ";\n";
	char* tmp;
	char* s = strdup(str);
	char* word= strtok_r(s,delim,&tmp);
	int charCount;
	int cmdCount = 0;
	for(; word != NULL; word = strtok_r(NULL,delim,&tmp)){
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
		command[strlen(word)] = '\0';//B: Do we want this to be '\0' instead?
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
	struct node * head = (struct node*)malloc(sizeof(struct node));
	printf(">>>");
	while(fgets(input, 255, stdin)!=NULL){
		char ** cmds = tokenify(input,";");
		tokenify(cmds[0],whitespace);
		handle_commands(cmds,head);
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

