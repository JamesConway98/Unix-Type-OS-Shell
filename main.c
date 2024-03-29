#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 
#include <stdbool.h>

void shell_loop();
void setpath(char []);
void getpath();
void getInput();
int tokeniser();
void checkCommands();
void newCmd(char*);
void listHistory();
void clearHistory();
void fork_command();
void setAlias(int noTokens);
void printAliases();
int checkAlias();
void unalias(int noTokens);
void writeToFile(char*, char*);
void readFile(char*);

int running = 0; 					//keep program running 
char *tkarray[512]; 					//array of tokens
int numAliases; 					//number of aliases
char input[512];					//input given by user
int nt = 0; 						//number of tokens

#define historySize 20					// The size of the history array
int cmdcount = 0;					// number of commands
char history[historySize][512];				// array of history commands


typedef struct alias{
	char name[15]; 					//name of alias
	char command[15][15];				//alias command
	int parameters;					//number of parameters for alias
}alias;

struct alias aliases[10];

void main(){
    
    
	char *path = getenv("PATH"); 			//saves original path
	chdir(getenv("HOME"));
	readFile(".hist_list.txt");			// loads history from file
	shell_loop();
	setenv("PATH", path, 1); 			//restores orignal path upon exit//
	printf("%s \n", path);
}

/*
allows for the shell to continuously loop until it has been told to exit
*/
void shell_loop(){

	numAliases = 0;						

	//continuous loop for running program
    	while(running == 0){
    		printf(":) ");
		getInput();
	}   

}

/*
 changes path to one given by user
*/
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

/*
prints current path to screen
*/
void getpath() {
	char *path = getenv("PATH");
	printf("%s \n", path);
}

/*
gets input from user
*/
void getInput(){
	
	//loop that exits if CTRL D is pressed
	if(fgets(input, 512, stdin) == NULL) { 
		printf("\n");
		running = 1;
		return;
	}

	input[strlen(input)-1] = '\0'; 			//adds null to end of input string
	tokeniser();
}	

/*

*/
int tokeniser(){

	char inputcpy[512];
	char *token;
    	int aliasFound = 0;
	char num[2];
	int number = 0;
	nt = 0;

	for(int i = 0; i<512; i++){
			tkarray[i] = '\0';
	}

	strcpy(inputcpy, input); 			//makes a copy of input
	token = strtok (input, " |><&;\t\n"); 		//creates the first token
	
	//creates loop for when enter key is pressed on its own, returns to start to go through loop
	if (token == NULL){ 	
		return 0;
	}
	
	tkarray[nt] = token;
	aliasFound = checkAlias();

	if(aliasFound > 0){
		nt = nt + aliasFound;
	}

	while (token != NULL){	//tokenises rest of input
		tkarray[nt] = token;
		token = strtok (NULL, " |><&;\t\n");
		nt++;
	}
	
	//makes sure alias name isn't part of the tkarray (writes over it)
	if(aliasFound > 0){
		for(int i = aliasFound; i < nt; i++){
			tkarray[i] = tkarray[i+1];
		}	
		nt--;
	}
	
	tkarray[nt] = NULL;				//sets the last token to null


	if(strncmp(tkarray[0], "!", 1)==0){

		if(strncmp(tkarray[0], "!!", 2)==0){
			strcpy(input, history[cmdcount%historySize-1]);
			tokeniser();
			return 0;
		} else if(strncmp(tkarray[0], "!-", 2)==0){
			strncpy(num, tkarray[0]+2, 2);
			number = atoi(num);
			strcpy(input, history[cmdcount%historySize-1-number]);
			tokeniser();
			return 0;
		} else{
			number = 0;
			strncpy(num, tkarray[0]+1, 2);
			number = atoi(num);
			strcpy(input, history[cmdcount%historySize-21+number]);
			tokeniser();
			return 0;
		}
	}else{
		newCmd(inputcpy);
	}

	
	checkCommands(); //checks the input for list of commands
	return 0;
}

/*
    sees which comand the user is trying to execute (e.g an internal command or a fork)
*/
void checkCommands(){

	//closes the shell when exit is entered
	if(strcmp(tkarray[0], "exit") == 0){ 
		if(nt == 1){
			running = 1;
		} else {
			printf("Error: exit command doesn't take parameters\n");		
		}
	} else if(strcmp(tkarray[0], "cd") == 0){	//changes directory
		if(nt == 2){
			if (chdir(tkarray[1]) == 0) {
				printf("Changing dir\n");
			} else {
				perror(tkarray[nt-1]);
			}
		} else if(nt == 1){
			chdir(getenv("HOME"));
		} else {
			printf("Error: cd takes 1 argument\n");		
		}
	} else if (strcmp(tkarray[0], "getpath") == 0){//returns current path
		if (nt == 1) {
			getpath();
		} else {
			printf("Error: Getpath does not require any parameters. \n");
		}
	} else if (strcmp(tkarray[0], "setpath") == 0){//changes current path
		if (nt ==2) {
			setpath(tkarray[1]);
		} else if (nt != 2) {
			printf("Error: incorrect number of parameters. Setpath requires one argument. \n");
		}
	} else if(strcmp(tkarray[0], "history") == 0){ //Command displays history list
		if(nt == 1){
			listHistory();
		} else {
			printf("Error: History command doesn't take parameters\n");		
		}
	} else if(strcmp(tkarray[0], "alias") == 0){
		if(nt >= 3){//sets new alias
			if(numAliases <= 10){
		 		setAlias(nt);
				numAliases++;
			} else {
				printf("Error: Maximum number of aliases has been reached. unable to add alias.\n");
			}
		} else if(nt == 1){//prints list of aliases
				printAliases();
			} else { 
				printf("Error: incorrect number of parameters for alias command.\n");
			}
	} else if(strcmp(tkarray[0], "unalias") == 0){//removes alias
			if(nt == 2){
				unalias(nt);
				numAliases--;
			} else {
				printf("Error: incorrect number of parameters for the unalias command.\n");
			}
	} else if((strcmp(tkarray[0], "clear") ==0) && nt == 2){
		if(strcmp(tkarray[1], "history") == 0)
		clearHistory();
	}else {
			fork_command();
		}	
}

/*
	Add the new command to the history array
*/
void newCmd(char* inputcmd) {

		strcpy(history[cmdcount%historySize], inputcmd);
		writeToFile(".hist_list.txt", strcpy(history[cmdcount%historySize], inputcmd));
		cmdcount++;
 }

/*
	calls and displays the history 
*/
void listHistory(){ 

	if(cmdcount<20){ 
		for(int i = 0, j = 1; i < cmdcount%(historySize) ; i++, j++){
       			printf("%d.%s\n", j, history[i]);
		}
	} else { 
		int number = 1;
		int i=cmdcount%historySize;

		do{
			if (history[i]) {
				printf("%d: %s\n", (number), history[i]);
				number++;
			}
			i = (i+1)%historySize;		
		}while (i!=cmdcount%historySize);
	}
}


/*
	clears the history 
*/
void clearHistory(){ 



	char* path = getenv("PATH");
	chdir(getenv("HOME")); //changes directory to home whenever history is cleared to ensure right file


	
  	FILE *fptr;
   	fptr = fopen(".hist_list.txt","w"); //writing nothing to file will clear it

   	//checks if file exists then closes
   	if(fptr){
		fclose(fptr);
   	}
	
	chdir(path); //changes directory back again

	
	
	for(int i = 0; i<20; i++){
		history[i] == NULL;
	}
	
	cmdcount = 0; //reset history command count
	
}

/*
 runs any commands then have not been internally programmed
*/
void fork_command(){

	pid_t pid;
	pid = fork();

	if(pid < 0) {
		fprintf(stderr, "Fork Failed");
	} else if(pid == 0){
		execvp(tkarray[0], tkarray);
		perror(tkarray[nt-1]);
		exit(EXIT_FAILURE);
	} else if(pid > 0){
		wait(NULL);
	}
}

/*
  adds an alias to list, eg alias a b means in the future when you type a, b happens
*/
void setAlias(int noTokens){

	int amount = numAliases;

	//checks if there are any aliases already using the name given
	for(int i = 0; i < amount; i++){
		if(strcmp(aliases[i].name, tkarray[1]) == 0){
			amount = i;
			numAliases--;	
			printf("alias %s -> ", aliases[i].name);
			for(int i = 0; i <  aliases[amount].parameters; i++){ 
				printf("%s ", aliases[amount].command[i]);
			}
			printf(" will be overrided.\n");
		}
	}

	//ensures that only 10 aliases can be stored at most
	if(numAliases < 10){
		strcpy(aliases[amount].name, tkarray[1]);
		aliases[amount].parameters = noTokens-2;
		printf("new alias: %s -> ", aliases[amount].name);
		for(int i = 0; i <  aliases[amount].parameters; i++){ 
			strcpy(aliases[amount].command[i], tkarray[i+2]);
			printf("%s ", aliases[amount].command[i]);
		}
		printf("\n");
	} else { 	
		printf("Maximum number of aliases has been reached. unable to add alias.\n");
		numAliases--;
	}

}

/*
outputs list of all aliases to screen
*/
void printAliases(){

	int amount = numAliases;

	if(amount == 0){
		printf("There are currently no saved aliases\n");
	} else { 
		printf("The current saved aliases are: \n");
	}
	for(int i = 0; i < amount; i++){
		printf("alias %d: ", i+1);
		printf("%s -> ", aliases[i].name);
		for(int j = 0; j < aliases[i].parameters; j++){
			printf("%s ", aliases[i].command[j]);
		}
		printf("\n");
	}
}

/*
checks to saee if the first token is an alias for something else
*/
int checkAlias(){

	int addedTokens = 0;

	for(int i = 0; i < numAliases; i++){
		if(strcmp(tkarray[0], aliases[i].name)==0){
			for(int j = 0; j < aliases[i].parameters; j++){
				tkarray[j] = aliases[i].command[j];
				addedTokens++;
			}
		}
	}

	return addedTokens; 				//for adjusting nt variable

}

/*
  removes an alias from the list
*/
void unalias(int noTokens){

	bool found = false;
	int amount = numAliases;
	int foundAt = 0;

	for(int i = 0; i < amount; i++){
		if(strcmp(aliases[i].name, tkarray[1]) == 0){
			foundAt = i;
			found = true;	
		}

	}

	if(found == true){	
		for(int i = foundAt; i < amount-1; i++){
			strcpy(aliases[i].name, aliases[i+1].name);
			aliases[i].parameters = aliases[i+1].parameters;
			for(int k = 0; k < aliases[i].parameters; k++){
				strcpy(aliases[i].command[k], aliases[i+1].command[k]);
			}
   		}
	}	

	if(found == false){
		printf("Error: No such alias exists.\n");
		numAliases++;
	} else { 
		strcpy(aliases[amount-1].name, "\0");
		aliases[amount-1].parameters = 0;
		for(int j = 0; j < aliases[amount-1].parameters; j++){
			strcpy(aliases[amount-1].command[j], "\0");

		}
		printf("alias successfully removed.\n");
	}
}

/*
    Writes text from argument to File 
*/

void writeToFile(char* fileName, char* text)
{
   	char* path = getenv("PATH");
	chdir(getenv("HOME")); //changes directory to home whenever history is saved
   	int num;
   	FILE *fptr;
   	char* currentHistory;
   	fptr = fopen(fileName,"a");

   	fprintf(fptr,"%s\n", text);
   	fclose(fptr);
	chdir(path); //changes directory back again
}


/*
    Reads the history from the file that is passed in as an argument to the history array 
*/
void readFile(char* fileName)
{
   
	int i = 0;
   	char string[512];
   	char* lines[20];
	char* path = getenv("PATH");
	char* histoken;
	chdir(getenv("HOME")); //changes directory to home whenever history is read to ensure right file

  	FILE *fptr;
   	fptr = fopen(fileName,"r");

   	//checks if file exists then prints each line
   	if(fptr){
	   	while(fgets(string, 512, fptr) != NULL){
			histoken = strtok (string, "\n");
		   	strcpy(history[i%historySize], histoken);
		   	i++;
		   	cmdcount++;
	   	}
		fclose(fptr);
		chdir(path); //changes directory back again
   	}
}
