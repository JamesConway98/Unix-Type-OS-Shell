##include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 

void getInput();
void shell_loop();
int find_command(char []);
void fork_command();
void setpath(char []);
void getpath();

int running = 0;
char *tkarray[512];


void main(){
	char *path = getenv("PATH"); /*saves original path*/
	chdir(getenv("HOME"));
	shell_loop();
	setenv("PATH", path, 1); /*restores orignal path upon exit*/
	printf("%s \n", path);
}


void shell_loop(){

    while(running == 0){
    	printf(":) ");
	getInput();
    }   

}

void setpath(char *dir) {
	char *oldpath = getenv("PATH");
	setenv("PATH", dir, 1);
	char *path = getenv("PATH");
	if (strcmp(dir, path) == 0) {
		printf("%s \n", path);
	}
	else {
		printf("Re-pathing has failed, please try again");
		setenv("PATH", oldpath, 1);
	}
}

void getpath() {
	char *path = getenv("PATH");
	printf("%s \n", path);
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
			if (chdir(tkarray[1]) == 0) {
				printf("Changing dir\n");
			}else{
				perror("Error");
			}
		}else if(nt == 1){

			chdir(getenv("HOME"));

		}else{
			printf("Error: cd takes 1 argument\n");	
			
		}
		
	}

	else if (strcmp(tkarray[0], "getpath") == 0){
		if (nt == 1) {
			getpath();
		}
		else {
			printf("Getpath does not require any parameters, please try again \n");
		}
	}

	else if (strcmp(tkarray[0], "setpath") == 0){
		if (nt ==2) {
			setpath(tkarray[1]);
		}
		else if (nt != 2) {
			printf("Setpath requires one parameter, please try again. \n");
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
	execvp(tkarray[0], tkarray);
	perror("Error: ");
	exit(EXIT_FAILURE);
}

else if(pid > 0){
	wait(NULL);

}

}