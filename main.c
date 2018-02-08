#include <stdio.h>
#include <string.h>

void getInput();
void shell_loop();
int find_command(char []);

int running = 0;


void main(){
	shell_loop();

}


void shell_loop(){

    while(running == 0){
    	printf(":) ");
	    getInput();
	    find_command();
    }   

}


char getInput(){

char input[512];
char * token;

fgets(input, 512, stdin);
input[strlen(input)-1] = '\0';

printf("splitting string %s into tokens: \n", input);
token = strtok (input, " ,.");

	while (token != NULL){
		printf("'%s' \n", token);
		token = strtok (NULL, " ,.");
}


}

int find_command(char c[120]){

	
if(strncmp("exit", c, 4) == 0){
	
		running = 1;
		return 0;		

	}
else if(strncmp("cd", c, 2) == 0){
	
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
