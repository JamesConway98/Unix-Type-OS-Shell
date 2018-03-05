#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 

void getInput();
void shell_loop();
int find_command(char []);
void fork_command();

int running = 0;
char *tkarray[512];


void main(){
	shell_loop();

}


void shell_loop(){

    while(running == 0){
    	printf(":) ");
	getInput();
    }   

}


void getInput(){

	char input[512];
	char *token;
	int nt = 0;

	for(int i = 0; i<512; i++){
			tkarray[i] = '\0';
	}

	if(fgets(input, 512, stdin) == NULL) {
		printf("\n");
		running = 1;
		return;
	}

	input[strlen(input)-1] = '\0';

	token = strtok (input, " |><&;\t\n");

	if (token == NULL){
		return;
	}

	while (token != NULL){
		printf("'%s' \n", token);
		tkarray[nt] = token;
			
		token = strtok (NULL, " |><&;\t\n");
		nt++;
	}
	tkarray[nt] = NULL;


	if(strcmp(tkarray[0], "exit") == 0){
		
		if(nt == 1){
			running = 1;
		}else{
			
			printf("Error: exit command doesn't take parameters\n");		

		}
	}

	else if(strcmp(tkarray[0], "cd") == 0){
		
		if(nt == 2){
			chdir(tkarray[1]);
			printf("Changing dir\n");		
		}else if(nt == 1){
			
			chdir(getenv("HOME"));

		}else{
			printf("Error: cd takes 1 argument\n");	
			
		}
	}

	else{
		fork_command();
		}

}

void fork_command(){

pid_t pid;

pid = fork();

if(pid < 0) {
	fprintf(stderr, "Fork Failed");
}

else if(pid == 0){
	execlp(tkarray[0], "ls", NULL);
	//only executed if the above line isn't successful
	perror("Error"); 
	exit(EXIT_FAILURE);
}

else if(pid > 0){
	wait(NULL);
	printf("Child Complete\n");

}

}


