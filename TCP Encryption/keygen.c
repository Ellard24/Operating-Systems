/******************************************
 * Ellard Gerritsen van der Hoop
 * CS344
 * keygen.c
 *
 * gcc -o keygen keygen.c
 * *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/*******************************************************
 * Function: main
 * Description: Takes command line arg and creates 
 * 		a randomized key of that number. 
 * Parameters: int argc, char *argv[]
 * Returns: Returns 0
 *******************************************************/
int main(int argc, char *argv[]){

	//Get keylength from argument and convert to int 
	int keylength;	
	char *number = argv[1];
	
	//Check to see if argument was properly passed
	if (argc < 2){
		fprintf(stderr, "Argument count incorrect. Usage: keygen [number]\n");
		exit(1);
	}


	int i = 0;
	int check = 0;
	
	//Loop through checking for digits 
	for (i = 0; i < number[i]; i++)
	{
		if (isdigit(number[i]) == 0)
			check = 1;
	
	}
	

	
	//Error. Print to stderr
	if (check  == 1){
		fprintf(stderr, "Argument is not a number. Usage: keygen [number]\n");
		exit(1);
	}
	
	//Convert to int
	keylength = atoi(argv[1]);	

	//Set randomizer seed 
	srand(time(NULL));
	
	//printf("KeyLength: %d\n", keylength);
	
	
	

	
	//Loop through size of keylength and output random capital letter or space 
	for (i = 0; i < keylength; i++)
	{
		//27 available letters/space
		int num = rand() % 27;

		//Makes sure we have probabiity for space 
		if (num == 26)
			num = ' ';
		else
			num += 'A'; 
		

		//This makes sure we convert to a char
		printf("%c", (char)num);



	}

	//Last one has to be a newline	
	printf("\n");









	return 0;
}
