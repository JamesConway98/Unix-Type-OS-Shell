#include <stdio.h>
#include <string.h>

void getInput();
void shell_loop();


void main(){
	shell_loop();

}


void shell_loop(){

int exitProg = 0;


/*do{*/
	printf(":) ");
	getInput();


/*}while(exitProg == FALSE);*/

}


void getInput(){

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
