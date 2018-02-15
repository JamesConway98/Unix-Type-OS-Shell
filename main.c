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
char tkarray[12][120];


void main(){
	shell_loop();

}


void shell_loop(){

    while(running == 0){
    	printf(":) ");
	getInput();
	find_command(tkarray[0]);
    }   

}


void getInput(){

char input[512];
char * token;
int nt = 0;

for(int i = 0; i<12; i++){
	for(int j = 0; j < 120; j++)
		{
		tkarray[i][j] = '\0';
		}
	}

		if(fgets(input, 512, stdin) == NULL){
		running = 1;
}
	input[strlen(input)-1] = '\0';

	printf("splitting string %s into tokens: \n", input);
	token = strtok (input, " ,.");

	while (token != NULL){
		printf("'%s' \n", token);
		strcpy(tkarray[nt], token);
		token = strtok (NULL, " ,.");
		nt++;
}

}

int find_command(char c[120]){

	
if(strncmp("exit", c, 5) == 0){
		running = 1;
		return 0;		

	}

else if(strncmp("fork", c, 2) == 0){
		return 1;		

	}
else if(strncmp("getpath", c, 7) == 0){
	
		return 2;	

	}

else if(strncmp("setpath", c, 7) == 0){
	
		return 3;		

	}
	
	return 0;



}

void fork_command(){

pid_t pid;

pid = fork();

if(pid < 0) {
	fprintf(stderr, "Fork Failed");
}

else if (pid == 0)
	execlp("", , NULL); //this line is incomplete. the first 2 paramaters should be user inputted commands


else if(pid > 0){
	wait(NULL);
	printf("Child Complete\n");

}

}
