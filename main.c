#include <stdio.h>
#include <string.h>

void getInput();
void shell_loop();
int find_command(char []);

int running = 0;
char *tkarray[512];


void main(){
	shell_loop();

}


void shell_loop(){

    while(running == 0){
    	printf(":) ");
	getInput();
	/*find_command(*tkarray);*/
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
		
		printf("P error\n");		

	}
} 
}


