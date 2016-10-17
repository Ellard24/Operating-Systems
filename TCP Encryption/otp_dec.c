/***********************************
 * Ellard Gerritsen van der Hoop
 * CS344 OTP
 *
 * gcc -o otp_dec otp_dec.c
 *
 * *********************************/

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

/*******************************************************
 * Function: sockSetAndCheck
 * Description: Sets up the socket and error checks
 * Parameters: int *sockfd
 * Returns: -
 *******************************************************/

void sockSetAndCheck(int *sockfd)
{
	//IPv4 = AF_INET   SOCK_STREAM = TCP
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (*sockfd < 0)
	{
		fprintf(stderr, "Error. Socket problems\n");
		exit(1);
	}



}

/*******************************************************
 * Function: portAndServerSetup
 * Description: Checks portnumber. sets up server struct
 * 		and serverAddress struct
 * Parameters: int *portNumber, char *argv[], struct sockaddr_in
 * 		*serverAddress, struct hostent **server
 * Returns: -
 *******************************************************/

void portAndServerSetup(int *portNumber, char *argv[], struct sockaddr_in *serverAddress, struct hostent **server)
{
	char host[] = "localhost"; //testing is being done on local host
	
	//Sets port number to command line arg
	*portNumber = atoi(argv[3]);

	//Acceptable port number range
	if (*portNumber < 0 || *portNumber > 65536)
	{
		fprintf(stderr, "Error. Port needs to be between 0 and 65535\n");
		exit(2);

	}
	
	//Sets to IPV4, and converts port number to usable format
	serverAddress->sin_family = AF_INET;
	serverAddress->sin_port = htons(*portNumber);

	//SEts server host to localhost
	*server = gethostbyname(host);

	//Error checks
	if (*server == NULL)
	{
		fprintf(stderr, "Error. Can not find host :( \n");
		exit(1);
	}

}

/*******************************************************
 * Function: fileCheck
 * Description: Opens both files to check for bad characters
 * 		then compares sizes.  Then sets appropriate size
 * 		for fileSize to be used later. 
 * Parameters: char *argv[], int *fileSize
 * Returns:
 *******************************************************/

int fileCheck(char *argv[], int *fileSize)
{
	struct stat st; //struct stat can be used to check file sizes
	int fileSize1; //size for file 1
	int fileSize2; //size for file 2
	int c; //will be used during file loop  

	//open the first file and error check
	FILE *fp;
	fp  = fopen(argv[1], "r");
	

	if (fp == NULL)
	{
		fprintf(stderr, "Can't open file\n");
		exit(1);
	}	


	//Loop through each character and check for bad letters
	while ((c = fgetc(fp)) != EOF)
	{
		
	
		if (!isalpha(c) && !isspace(c))
		{
			fprintf(stderr, "Bad character in file\n");
			exit(1);
		}
	

	}
	fclose(fp);


	//Repeat the process for the second file
	fp = fopen(argv[2], "r");
	
	if (fp == NULL)
	{
		fprintf(stderr, "Can't open file\n");
		exit(1);
	}	

	while ((c = fgetc(fp)) != EOF)
	{

	
		if (!isalpha(c) && !isspace(c))
		{
			fprintf(stderr, "Bad character in file\n");
			exit(1);
		}
	

	}
	fclose(fp);

	//Use the stat function and struct to find size of the first file
	if (stat(argv[1], &st) != 0)
	{
		fprintf(stderr, "Error with file size\n");
		exit(1);
	}
	else
		fileSize1 = st.st_size; //if no error set fileSize to stat size

	//Do the same for the second file
	if (stat(argv[2], &st) != 0)
	{
		fprintf(stderr, "Error with file size\n");
		exit(1);
	}
	else
		fileSize2 = st.st_size;


	//The key must be the same size or bigger than the cipher
	if (fileSize1 > fileSize2)
		return -1;
	else{
		*fileSize = fileSize1; //set fileSize to size of cipher
		return 0;
	}





}


/*******************************************************
 * Function: fileSize
 * Description: Small file transfer. Sends file size over 
 * 		to server and then starts interaction loop
 * 		that will send one letter from each the text
 * 		and key and then receive the cipher letter
 * Parameters: int *sockfd, char *argv[], int fileSize
 * Returns: -
 *******************************************************/

void fileTransfer(int *sockfd, char *argv[], int fileSize)
{

	//Open the two files to be used	
	int fileSize1 = fileSize;
	//First Send file size over
	char fileSendSize[500];
	bzero(fileSendSize, sizeof(fileSendSize));
	sprintf(fileSendSize, "%d", fileSize); //append size to fileSendSize string
	

	//sending filesize over 
	if (send(*sockfd,fileSendSize, strlen(fileSendSize),0) < 0){
		fprintf(stderr, "Error sending\n");
		exit(1);
	}


	FILE *fp1;
	FILE *fp2;
	fp1 = fopen(argv[1], "r");
	fp2 = fopen(argv[2], "r");
	
	
	//going to loop through all characters and send them over 
	
	//ch1 and ch2 will be the looping variables
	int ch1; 
	int ch2;

	//sending1 and sending2 will hold the one letter to be sent 
	char sending1[1];
	char sending2[1];

	//this will be the returning letter after being decryped
	char receiving[1];

	//Main file loop that only ends when one file reaches end of file
	while (((ch1 = getc(fp1)) != EOF)  && ((ch2 = getc(fp2)) != EOF))
	{
	
		//Zero the memory
		bzero(sending1, sizeof(sending1));
		bzero(sending2, sizeof(sending2));
		bzero(receiving, sizeof(receiving));
			
		//Set sending1 and sending2 to next char in line
		sending1[0] = ch1;
		sending2[0] = ch2;	

		//Break if newline
		if (sending1[0] == '\n')
			break;


//		printf("%c\n", sending1[0]);
//
		
		//Send over sending1
		if (send(*sockfd, sending1, sizeof(sending1),0) < 0){
			fprintf(stderr, "Error sending\n");
			exit(1);
		}
		
//		printf("%c\n", sending2[0]);
//
		//Send over sending2
		if (send(*sockfd, sending2, sizeof(sending2),0) < 0){
			fprintf(stderr, "Error sending\n");
			exit(1);
		}

		//REceive encrpted character
		if (recv(*sockfd, receiving, sizeof(receiving), 0) < 0){
			fprintf(stderr, "Erro receiving\n");
			exit(1);

		}

		//need to print it out 
		printf("%c",receiving[0]);


	}
//	printf("Transfer complete");

	//Print newline we stripped off earlier then close files
	printf("\n");
	fclose(fp1);
	fclose(fp2);

	



}

/*******************************************************
 * Function: bigTransfer
 * Description: My previous transfer method would work for 
 * 		large files but it would just take far too long
 * 		so I had to implement a faster method but its
 * 		not as cool in my opinion. Just for you plaintext4
 * Parameters: int *sockfd, char *argv[], int fileSize
 * Returns:-
 *******************************************************/

void bigTransfer(int *sockfd, char *argv[], int fileSize)
{

	//Open the two files for read only
	int fp1;
	int fp2;
	fp1 = open(argv[1], 'r');
	fp2 = open(argv[2], 'r');

	//First Send file size over
	char fileSendSize[500];
	bzero(fileSendSize, sizeof(fileSendSize));
	sprintf(fileSendSize, "%d", fileSize);
	

	//sending filesize over 
	if (send(*sockfd,fileSendSize, strlen(fileSendSize),0) < 0){
		fprintf(stderr, "Error sending\n");
		exit(1);
	}

		

	//this buffer will hold all the data we need
	char transBuffer[100000];
	int readIn1 = 0; //tracks read in material from files
	int sent1 = 0; //tracks sent data 
	int size = fileSize; //we will use this as a looping control


	
	//printf("made it to before reciving data\n");	
	//Size will be decrimental as data is read in
	bzero(transBuffer, sizeof(transBuffer));
	while (size > 0)
	{

		
		//read in the data
		readIn1 = read(fp1, transBuffer, fileSize);
//		printf("Readin1 : %d\n", readIn1);	
	
		//if 0 it means end of file has been reached
		if (readIn1 == 0){
			break;
		}
	
		//update the size variable based on readin data
		size  -= readIn1;

		
		//Iterate over readin data to make sure to send all of it
		while (readIn1 > 0)
		{
			sent1 = send(*sockfd, transBuffer, readIn1,0);

			if (sent1  < 0){
			
				fprintf(stderr, "Error sending\n");
				exit(1);
			}
		
			//Update readin based on sent data
			readIn1 -= sent1;
		
		}


	}
	
	bzero(transBuffer, sizeof(transBuffer));
	//Send key message. This will be the exact same as before
	size = fileSize;
	while (size > 0)
	{
		bzero(transBuffer, sizeof(transBuffer));
	
		readIn1 = read(fp2, transBuffer, fileSize);
		
		//end of transmission
		if (readIn1 == 0)
			break;
		
		//ensures we break the loop 
		size -= readIn1;

		
		//makes sure we send all data we read in from the file
		while (readIn1 > 0)
		{

			sent1 = send(*sockfd, transBuffer, readIn1,0);

			//error check
			if (sent1  < 0){
				fprintf(stderr, "Error sending\n");
				exit(1);
			}
			//ensures we break loop	
			readIn1 -= sent1;
		}
	


	}
	
	
	//ReZero the buffer and wait for receiving message
	bzero(transBuffer, sizeof(transBuffer));

	size = fileSize;
	
//	printf("made it before recv data\n");

	//Decrement fileSize based on received data
	int tracker = 0;
	while (size > 0)
	{
//		printf("just before recv\n");
	
		//read in the data 
		readIn1 = recv(*sockfd, transBuffer, sizeof(transBuffer),0);
		tracker += readIn1;

		//print out received data as we get it	
		printf("%s", transBuffer);

		//ensure we break the loop based on any of these three conditions
		//either by end of data or error
		if (readIn1 == 0 || readIn1 < 0 || tracker >= fileSize){
		
			break;
		}
		size -= readIn1;


	}
		
	
	

	

//	printf("Transfer complete");




}

/*******************************************************
 * Function: main
 * Description: Runs the simulation, sets up the client socket
 * 		, host, then calls the appropriate functions
 * 		to connect to server and then carry out interaction
 * Parameters: int argc, char *argv[]
 * Returns: Returns 0
 *******************************************************/

int main(int argc, char *argv[])
{
	//Client only needs one socket file descriptor
	int sockfd;
	int portNumber; //to be changed by specied command line
	int programNumber = 2;  //this will be used to verify connection eligibility
	int fileSize;
	int n;
	struct sockaddr_in serverAddress; //struct for server
	struct hostent *server; //struct for localhost
	
	//Argument check. If incorrect amount, end it
	if (argc < 4){
		fprintf(stderr, "Error. Incorrect amount of parameters. Usage: otp_enc [textfile] [key] [port]\n");
		exit(1);
	}

	//Check file sizes 

	if (fileCheck(argv, &fileSize) != 0)
	{
		fprintf(stderr, "Error.Key too short.\n");
		exit(1);
	}

	//Set up socket
	sockSetAndCheck(&sockfd);
	
	//Zero out struct serverAddress
	bzero((char *)&serverAddress, sizeof(serverAddress));
	
	//Set up port and Server
	portAndServerSetup(&portNumber, argv, &serverAddress, &server);

	//Copies the amount of bytes from h_addr, to s_addr, based on h_length
	bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);


	//Attemps connection to server
	if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){

		fprintf(stderr, "Error connecting to server\n");
		exit(1);
	}


	//Quick verification to make sure otp_enc is able to connect to otp_dec_d
	char programID[10];
	
	bzero(programID, sizeof(programID));
	sprintf(programID, "%d", programNumber);


	//sennding programID 
	if (send(sockfd, programID, sizeof(programID),0) < 0){
		fprintf(stderr, "Error sending\n");
		exit(1);
	}

	//Rezero the buffer and wait for servers answer
	bzero(programID, sizeof(programID));
	if (recv(sockfd, programID, sizeof(programID), 0) < 0){
		fprintf(stderr, "Error receiving\n");
		exit(1);
	}		


	//printf("made it to before transfer\n");
	//If server has incorrect reponse. End connection. Checks compatibility
	if (strcmp(programID, "2") != 0)
	{
		fprintf(stderr, "Error. Communication with otp_dec_d is not allowed\n");
		close(sockfd);
		exit(1);
	}
	else{
		//Calls bigTransfer or fileTransfer based on fileSize
		if (fileSize > 100)
			bigTransfer(&sockfd, argv, fileSize);   
		else
			fileTransfer(&sockfd, argv, fileSize);
	}


	


	close(sockfd);









	return 0;
}
