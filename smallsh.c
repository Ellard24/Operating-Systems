/*************************************
 * Author: Ellard Gerritsen van der Hoop
 * CS344- Operating Systems Spring 2016
 * Assignment 3: smallsh
 *
 * To compile: gcc -o smallsh smallsh.c
 * ***********************************/




#include <stdio.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>


//protoype functions 
void parseLine(char *line, char **args, int *argCount, char **inputFile, char **outputFile, int *background);
int executeCommand(char **args, int background, int *forkCounter, char **outputFile, char **inputFile, char *msg, int *statNum);
int builtInCommands(char **args, int *argCount, int *exitLoop, char *msg, int *statNum);
void initialize(char **outputFile, char **inputFile, char **args, int *argCount, int *background);
void process_cleanup();


//Struct responsibile for handling Signals 
struct sigaction act;


/******************************************************************
*Function: parseLine
*Description: This function is responsibile for breaking up the fgets stdin line
			into arguments that will be stored in args,seeing if there
			is an input/output file specified, and finding out if background process. 
			
*Parameters: char *line, char **args, int *argsCount, char **inputFile, char **outputFile, int *background
*Returns: No return value. Void function 
*******************************************************************/
void parseLine(char *line, char **args, int *argCount, char **inputFile, char **outputFile, int *background){


	int tempCount = 0; //variable used to keep track of arguments 
 
	char *p; //this will be what is tokenized 
	
	
	//Strtok loop that goes through the line received from fgets and figures
	//out if it is an argument, if there is an input file/outputfile, and if 
	//background is specified 
	for (p = strtok(line, " "); p != NULL; p = strtok(NULL, " "))
	{
		
		//This ensures that a newline is removed since fgets tends to put a newline at the end 
		int i = strlen(p) - 1;
		if ((i > 0) && (p[i] == '\n'))
			p[i] = '\0';

		
		//Checks to see if redirection is specified. This case is for input 
		if (strcmp(p, "<") == 0)
		{
			//tokenizes until end and allocates memory for inputfile 
			p = strtok(NULL, " \n");
			*inputFile = (char *)malloc(sizeof(char) * sizeof(p) + 1);
	
			strcpy(*inputFile, p);  //copies the value of p into inputFile 

		}

		else if (strcmp(p, ">") == 0)
		{

			p = strtok(NULL, " \n");

			*outputFile = (char *)malloc(sizeof(char) * sizeof(p) + 1);
	
			strcpy(*outputFile, p);  //copies the value of p into outputFile 

		}
		//checks to see if background process and if it is, then set background to 1 
		else if (strcmp(p, "&") == 0)
		{
			*background = 1;


		}
		else{   //stores p in the args char array 
			args[tempCount] = p;

			tempCount++;  //increments to next space in array 
		}
	}




	*argCount = tempCount;  //sets argCount to equal final tempCount 
	
	//printf("Arg Count in parse: %d\n", *argCount);
	args[tempCount] = NULL;    //For execvp to work the last spot needs to be NULL 


	//Debugging 
//		int i = 0;
//		for (i = 0; i < *argCount; i++)
//			printf("%s\n", args[i]);


}

/******************************************************************
*Function: executeCommand
*Description: This function is responsibile for executing the specified command
			that is located in args[0]. It checks to see if input/output file 
			are not null and uses them in the execution if they have names. If 
			background process was specified earlier, then the process created will
			be in the background. 
*Parameters: char **args, int background, int *forkCounter, char **outputFile, 
				char **inputFile, char *msg, int *statNum
*Returns: Returns 0 on success, 1 on failure 
*******************************************************************/
int executeCommand(char **args, int background, int *forkCounter, char **outputFile, char **inputFile, char *msg, int *statNum)
{
	
	
	
	pid_t pid; //Process ID variable used for forking 
	
	
	fflush(stdin);
	fflush(stdout);	
	
	//a precaution in case of fork bomb, will cause function to exit out if above 100 
	if (*forkCounter < 100){	
		
		if (background == 0){
			
		//	printf("made it inside exe\n");			
		
		
			//Fork return values below 0 are errors. Exits out if so
			if ((pid = fork()) < 0){
					printf("Forking Error");
					exit(1); 				
			}
				
				//Success 
				if (pid == 0)
				{
					
					//We set the child process to have default action when signal occurs 
					act.sa_handler = SIG_DFL;
					act.sa_flags = 0;
					sigaction(SIGINT, &act, NULL);    
					*forkCounter++;  //increase fork counter for control mechanism 
					int i = 0;

					
					//Checks whether input or output file are null if so it means that no further action is needed
					//besides executing args[0]
					if (*inputFile == NULL && *outputFile == NULL){
//						printf("inside file == null\n");
						
						//Checks for execvp error, displays error, and returns 1 exit value 
						if(execvp(args[0], args) == -1){
							printf("%s: no such file or directory\n", args[0]);
							exit(1);	
						}
	
					}

					else{
						
						
						if (*outputFile != NULL)
						{
				
							//create file, set permissions 
							int fp; 
							fp = open(*outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
						
							//Error check file and exits with 1 if error 
							if (fp == -1){
								printf("%s: no such file or directory\n", *outputFile);
								exit(1);
							}				
							
							//flush stdin/stdout because I am paranoid about it 
							fflush(stdin);  
							fflush(stdout);			
					//		printf("made it passed fp open\n");
							int fileDup = dup2(fp, 1);	//duplicate the file descriptor 
					//		printf("after dup2 variable\n");
					
							//Error check the file descriptor and exits with 1 if so 
							if (fileDup == -1){
								printf("Dup2 Error\n");
								exit(1);
							}
		
					
							//we dont need the original file descriptor so we close it 
							close(fp);
						
							//error checks the execvp command 
							if (execvp(args[0], args)){
								printf("Error with execvp\n");
								exit(1);
							}
							

						}
						
						//does the same thing as before but with inputfile 
						else if (*inputFile != NULL)
						{
							int fp;
						
							fp = open(*inputFile, O_RDONLY);
							
							if (fp == -1){
								printf("%s: no such file or directory\n", *inputFile);
								exit(1);
							}				
							
							int fileDup;
							fileDup = dup2(fp, 0);
							if (fileDup == -1){
								printf("Dup2 Error\n");
								exit(1);
							}
							close(fp);
							if (execvp(args[0], args)){
								printf("Error with execvp\n");
								exit(1);

							}
							
						}
				
						
					}


				}
				
				//this is the parent process
				else{
		
					//we need to set it up again so that we cant interrupt the parent since that would exit the shell 
					act.sa_handler = SIG_IGN;
					sigaction(SIGINT, &act, NULL);		
					int status;
				
					//here we wait for the child processes to finish in the foregound 
					waitpid(pid, &status, 0);
					if (WIFEXITED(status))     //we are going to store the exit status number in statNum so we can display this later when "status" command 
						*statNum = WEXITSTATUS(status);
					if (WIFSIGNALED(status)){
						strncpy(msg, "", 100);  
						printf("terminated by signal %d\n", WIFSIGNALED(status));
					
						//we are going store the terminated message + the signal in the msg variable so we can display this later when "status" command
						sprintf(msg, "terminated by signal %d", WIFSIGNALED(status));
						

						//printf("%s\n", msg);									
					}

					

	
					
					

				}
		}
		else if (background == 1)	//this means fgets line had & symbol so we are going to run this process in the back
		{
			if ((pid = fork()) < 0){ 	//same error check as before 
				printf("Forking Error");
				exit(1); 				
			}
			
			if (pid == 0)
			{
				*forkCounter++;		//increment forkCounter
				int i = 0;
			
				//same as before, check for NULL and execute args[0] if both are 
				if (*inputFile == NULL || *outputFile == NULL){
					if(execvp(args[0], args)){
						printf("%s: no such file or directory\n");
						exit(1);
					}
				}
				else{	
					
					
					if (*outputFile != NULL)
					{
						//create file
						int fp; 
		
						fp = open(*outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
						
				
						if (fp == -1){
							printf("%s: no such file or directory\n", args[i+1]);
							exit(1);
						}				
						
						int fileDup;
						fileDup = dup2(fp, 1);
						if (fileDup == -1){
							printf("Dup2 Error\n");
							exit(1);
						}
						close(fp);

						execvp(args[0], args);


					}
					
					else if (*inputFile != NULL)
					{
						int fp;
			
						fp = open(*inputFile, O_RDONLY);		
						

						if (fp == -1){
							printf("%s: no such file or directory\n", args[i+1]);
							exit(1);
						}				
						
						int fileDup;
						fileDup = dup2(fp, 1);
						if (fileDup == -1){
							printf("Dup2 Error\n");
							exit(1);
						}
						close(fp);
						execvp(args[0], args);

					
			
					
					}
				}


			}
			

			else{     
		
				
				//prints out message with process ID 
				printf("background PID is %d\n", pid);
				
				int status;
					
				//The key difference here is that we use WNOHANG in place of 0 	
				//I call this function often since background functions can terminate at any time 
				waitpid(pid, &status, WNOHANG);

			
		

		


			}
		
			

		}
			
		return 0;  //returns 0 if everything was successful 

	}
	else
		return 1;			//returns 1 if forkCount limit was reached which is bad 

	



}

/******************************************************************
*Function: builtInCommands
*Description: This function is responsibile for checking to see if args[0] 
			,which is the command the user specifies, is a command that is 
			built in to the shell. Options include cd, status, and exit.
			It also ensures that if #(a comment) or " "(a space) occurs that
			no command is executed and the main shell loop starts over 
*Parameters:char **args, int *argCount, int *exitLoop, char *msg, int *statNum
*Returns: Returns 1 if builtInCommand was successful(besides exit which will return 2),
			returns 0 if no builtInCommand was executed
*******************************************************************/
int builtInCommands(char **args, int *argCount, int *exitLoop, char *msg, int *statNum)
{
//	printf("Arg Count %d\n", *argCount);
//	printf("First Arg: %s\n", args[0]);

	int status;

	//Checks args[0] to see if comment symbol was input, will cause no execvp to occur 
	if ((strcmp(args[0], "#") == 0))
	{
		
		//comment line
		//printf("This is a comment\n");
		return 1;

	}
	//Checks args[0] to see if space symbol was input, will cause no execvp to occur 
	else if (strcmp(args[0], "\n") == 0)
		return 1;


	//Checks args[0] to see if cd was specified
	else if (strcmp(args[0], "cd") == 0)
	{
		if (args[1] == NULL){ //checks the argument after args[0] to see if user specifed directory 
			
			
			char *home;			//create variable that will hold environment variable name 
			home = getenv("HOME");  //set it to environment variable HOME, thus home directory 
			chdir(home);			//changes directory to home 
			return 1;
		}
		else{
					//if args[1] does exist, then this is the directory we go to
			DIR *directory;		   //dirent.h variable 
			if ((directory = opendir(args[1])) == NULL){		//checks to see if directory exists
				printf("Not a directory\n");
				return 1;
			}
			else{	//if directory does exist we go there 
				chdir(args[1]);				
				return 1;
			}
		}
	}

	//Checks args[0] to see if exit was specified, sets exitLoop to 1, and then exits. 
	else if (strcmp(args[0], "exit") == 0){
		//printf("inside exit\n"); 
		*exitLoop = 1;
	//	exit(1);
		return 2;
	}

	//Checks args[0] to see if status was specified
	else if (strcmp(args[0], "status") == 0){
		
		//if msg does not exist then it means the last process ended on its own so we display the exit value 
		// we saved in execvp 
		if (strncmp(msg, "", 100) == 0)
		{
			printf("Exit value: %d\n", *statNum);



		}
		else{		//else we print the terminated signal message 
			printf("%s\n", msg);
		}
		strncpy(msg, "", 100);   //set both to default of blank and 0 for next time 
		*statNum = 0;	

		return 1;
	}
	else
		return 0; 

}

/******************************************************************
*Function: process_cleanup
*Description: This function is responsible for printing out the 
		exit value or terminated signal for background processes.
*Parameters: - 
*Returns: No return value. Void function 
*******************************************************************/
void process_cleanup()
{
	int status;  //to be used with waitpid,WIFEXITED, WIFSIGNALED
	int cpid = waitpid(-1, &status, WNOHANG);

	//Takes care of background processes by printing out when they finish 
	// and whether by termination or just standard completion
	while (cpid > 0){
	
		
		if (WIFEXITED(status))
			printf("Background PID %d is done: exit value %i\n", cpid, WEXITSTATUS(status));
		if (WIFSIGNALED(status))
			printf("Background PID %d is done: terminated by signal %i\n",cpid, status);

		cpid = waitpid(-1, &status, WNOHANG);
	}		

}

/******************************************************************
*Function: initialize
*Description: This function is responsbile for re-initializing variables everytime the 
			main shell loop starts over. Memory is also freed when necessary.
*Parameters:char **outputFile, char **inputFile, char **args, int *argCount, int *background
*Returns: No return value. Void function 
*******************************************************************/
void initialize(char **outputFile, char **inputFile, char **args, int *argCount, int *background)
{
	int i = 0;
	for (i = 0; i < 512; i++)
		args[i] = NULL;				//sets all values in args array to NULL 

	if (outputFile != NULL)
		free(*outputFile);					//frees memory is there is a value for outputFile 

	if (inputFile != NULL)
		free(*inputFile);	//frees memory is there is a value for inputFile 

	*outputFile = NULL;		//set both files to NULL
	*inputFile = NULL;
	*argCount = 0;			//reset count
	*background = 0;		//reset background 




}



/******************************************************************
*Function: main
*Description: Declares variables that will be used in the simulation
			and then runs the shell loop 
*Parameters:-
*Returns: returns 0 
*******************************************************************/
int main(){
	int exitLoop = 0;		//controls shell loop
	int status = 0;
	int statNum = 0;		//will hold process exit value 
	int builtInCheck = 0;	//used to error check built in commands 
	int executeCheck = 0;	//used to error check executeCommand return value 
	char line[2048];		//max length of stdin line
	char *args[513];		//holds arguments with max number of arguments possible  
	char *inputFile = NULL;			//will hold inputFile name if one is specified
	char *outputFile = NULL;		//will hold outputFile name if one is specified
	char msg[100];
	
	int forkCounter = 0; //safety precaution tracker to control process forking

	int argCount = 0;		//current arg count of args array
	int background = 0;		//specifies whether background process: 0 if no, 1 if yes


	act.sa_handler = SIG_IGN;		//sets struct act to not terminate on signal at start 
	sigaction(SIGINT, &act, NULL);

	//Main shell loop 
	while (exitLoop == 0 && forkCounter < 100){
	
		process_cleanup(); 
	
		initialize(&outputFile, &inputFile, args, &argCount, &background);
		


	
		

		printf(": ");	//Each input needs to have : before 
		fflush(stdout);
		fgets(line, sizeof(line), stdin);
		fflush(stdin);
		


		parseLine(line, args, &argCount, &inputFile, &outputFile, &background);

		


//		printf("Status before built: %d\n", statNum);
		builtInCheck = builtInCommands(args, &argCount, &exitLoop, msg, &statNum);
//		printf("builtInCheck %d\n", builtInCheck);
		if (builtInCheck == 1)
			continue;
		else if (builtInCheck == 2)
			return 0;
		else if (builtInCheck == 0){
			executeCheck = 	executeCommand(args, background, &forkCounter, &outputFile, &inputFile, msg, &statNum); 

			if (executeCheck != 0)
			{
			
				exit(1);   //exits out 
			}
		}



	}

	return 0;



}
