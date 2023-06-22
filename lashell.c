/** This is a C program that implements a single-command shell.
  * It supports :
  *      - execution of commands with arguments
  *      - previous commands per session (not persistent) using arrow keys (up and down)
  *      - file name autocompletion using tab key
  *      - clearing the console by using 'CTRL+L" keyboard shortcut
  * It doesn't support advanced features like:
  *      - pipes `|`
  *      - input and output redirection `>`, `>>`, `<`, `<<`
  *      - conditional command execution `&&`, `||` and many more.

  * To use this shell first install the readline library using:
  *      `sudo apt-get install libreadline-dev`
  *  Then run the shell.sh file using:
  *      `bash shell.sh`
  *
  * This program was built by a team of 5 consisting of:
  *     Full Name                   ID
  *     1. Adem Mohammed            ETS0080/11
  *     2. Abdellah Hussein         ETS0006/11
  *     3. Abeba Molla              ETS0020/11
  *     4. Abel Endeshaw            ETS0029/11
  *     5. Abigya Getachew          ETS0050/11
  */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define MAXCOM 1000 
#define MAXLIST 100 


#define clear() printf("\033[H\033[J")

// accept input from user
int acceptInput(char* string){
    
    // print prompt text as
    // userName(in green)@hostName(in yellow):(in white)absoluteCurrentDirectory(in blue)

    char absoluteCurrentDirectory[1024]; // 1024 being the largest possible length for a directory
    getcwd(absoluteCurrentDirectory, sizeof(absoluteCurrentDirectory)); // using getcwd

    char hostNameBuffer[64]; // 64 being the largest possible hostname
    hostNameBuffer[63] = '\0';
    gethostname(hostNameBuffer, 63);

    char hostName[64];

    strcpy(hostName, hostNameBuffer);

    char wildcard[20] = "\033[3;32m\033[1;32m";
    
    char* prompt = wildcard;

    strcat(prompt, getenv("USER"));
    strcat(prompt, "\033[0m\033[1;33m@"); // adding 
    strcat(prompt, hostName);
    strcat(prompt, "\033[1m:\033[1;34m");
    strcat(prompt, absoluteCurrentDirectory);
    strcat(prompt, "\033[1;35m");
    strcat(prompt, " $ \033[0m");

    char* buffer = readline(prompt);

    if(strlen(buffer) != 0){
        add_history(buffer);
        strcpy(string, buffer);
        return 0;
    } else {
        return 1;
    }
}


void executeCommandWithArguments(char** parsed)
{
	
	pid_t pid = fork(); 			

	if (pid == -1) {

		printf("\nFailed to fork a child ...");
		
        return;
	
    } else if (pid == 0) {

        // we call execvp() inside an if statement
        // to check for execution failures
        // if the execvp function fails to execute, it will return -1
        // which will lead to the following print statement
        // otherwise we wait for execvp() to complete its task

		if (execvp(parsed[0], parsed) < 0) {
            // parsed[0] gets the first parsed value 
            // i.e., the command we want to execute and it oints 
            // to the file‐name associated with the file being executed
            // `parsed` is array of pointers to null-terminated strings that
            // represent the argument list available to the new program
			
            printf("\nCould not execute the command `%s`\n", parsed[0]);
		
        }
		// when execvp() completes its task we exit the forked child
        exit(0);
	
    } else {
        // blocking the flow of excution and waiting for a
        // signal (stop or resume) or
		// state change (termination in our case)
        // in the forked child process to avoid "zombie" processes
        wait(NULL);
		
        // retrun execution control to the caller of 
        // this function when the command is succesfully executed
        // to allow execution of subsequent commands
        return;
	
    }
}

int customCommandHandler(char** parsed){
    
    // if the parsed input is `exit`
    // use system call `exit(0)`
    // to close the program
    if(strcmp(parsed[0], "exit") == 0){
        exit(0);
    
    // else if the parsed input is `cd`
    // use `chdir()` system call
    // to change current working
    // directory to `parsed[1]` 
    // which holds the argument for `cd`
    } else if(strcmp(parsed[0], "cd") == 0){
        chdir(parsed[1]);
        return 1; // custom command executed succesfully
    }

    // if parsed[0] is not a custom command return 0
    return 0;
}

// function to parse(separate) `str` by
// space and put each word into the array `parsed`
void parseSpace(char* str, char** parsed)
{
	for (int i = 0; i < MAXLIST; i++) {
        // put the words into `parsed` one-by-one
		parsed[i] = strsep(&str, " ");

		// break when the input is empty
        if (parsed[i] == NULL)
			break;
        // parse words and skip multiple spaces 
		if (strlen(parsed[i]) == 0)
			i--;
	}
}

int processString(char* str, char** parsed)
{
	parseSpace(str, parsed);

    // if `parsed[0]` is either `exit` or `cd`
    // execute them and end the flow of execution
    // else return 1 to allow execution of `parsed[0]`
    if (customCommandHandler(parsed))
		return 0;
    else
        return 1;
}

int main(void){

    char inputString[MAXCOM], 
         *parsedArguments[MAXLIST];

    int executionFlag = 0;
    
    clear();

    // keep processing input indefinitely 
    // unitl `exit` command is issued or the program is interrupted
    for(;;){

        if(acceptInput(inputString))
            continue;
        
        executionFlag = processString(inputString, parsedArguments);
        
        if(executionFlag)
            executeCommandWithArguments(parsedArguments);
    }

    return 0;
}