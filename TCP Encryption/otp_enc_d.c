/*********************************************
 *Ellard Gerritsen van der Hoop
 *CS344 - OTP 
 *
 *
 *otp_enc_d.c
 *******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



#define BACKLOG 5

/*******************************************************
 * Function: module 
 * Description: Checks for if number is below 0 and adds
 * 		27 if need be. Then mods by total number
 * 		of available letters/space
 * Parameters: int a 
 * Returns: Returns modulated number
 *******************************************************/
int modulo(int a)
{
	if (a > 27)
		a = a - 27;
	
	a  = a % 27;


	return a;
	//C % is actually a remainder which doesnt work for negative numbers so we need this 	
	
	

}

/*******************************************************
 * Function: letterNumber
 * Description: Returns a number based on the letter
 * Parameters: Char letter
 * Returns: Returns number
 *******************************************************/

int letterNumber(char letter){

	if (letter ==  'A')
		return 0;
	if (letter == 'B')
		return 1;
	if (letter == 'C')
		return 2;
	if (letter == 'D')
		return 3;
	if (letter == 'E')
		return 4;
	if (letter == 'F')
		return 5;
	if (letter == 'G')
		return 6;
	if (letter == 'H')
		return 7;
	if (letter == 'I')
		return 8;
	if (letter == 'J')
		return 9;
	if (letter == 'K')
		return 10;
	if (letter == 'L')
		return 11;
	if (letter == 'M')
		return 12;
	if (letter == 'N')
		return 13;
	if (letter == 'O')
		return 14;
	if (letter == 'P')
		return 15;
	if (letter == 'Q')
		return 16;
	if (letter == 'R')
		return 17;
	if (letter == 'S')
		return 18;
	if (letter == 'T')
		return 19;
	if (letter == 'U')
		return 20;
	if (letter == 'V')
		return 21;
	if (letter == 'W')
		return 22;
	if (letter == 'X')
		return 23;
	if (letter == 'Y')
		return 24;
	if (letter == 'Z')
		return 25;
	if (letter == ' ') 
		return 26;
}


/*******************************************************
 * Function: numberLetter
 * Description: Does the opposite of letterNumber
 * Parameters: Int number
 * Returns: Returns a char
 *******************************************************/

char numberLetter(int number)
{


	switch (number){

		case 0:
			return 'A';
			break;
		case 1:
			return 'B';
			break;
		case 2:
			return 'C';
			break;
		case 3:
			return 'D';
			break;
		case 4:
			return 'E';
			break;
		case 5:
			return 'F';
			break;
		case 6:
			return 'G';
			break;
		case 7:
			return 'H';
			break;
		case 8:
			return 'I';
			break;
		case 9:
			return 'J';
			break;
		case 10:
			return 'K';
			break;
		case 11:
			return 'L';
			break;
		case 12:
			return 'M';
			break;
		case 13:
			return 'N';
			break;
		case 14:
			return 'O';
			break;
		case 15:
			return 'P';
			break;
		case 16:
			return 'Q';
			break;
		case 17:
			return 'R';
			break;
		case 18:
			return 'S';
			break;
		case 19:
			return 'T';
			break;
		case 20:
			return 'U';
			break;
		case 21:
			return 'V';
			break;
		case 22:
			return 'W';
			break;
		case 23:
			return 'X';
			break;
		case 24:
			return 'Y';
			break;
		case 25:
			return 'Z';
			break;
		case 26:
			return ' ';
			break;
	
	}


}



/*******************************************************
 * Function: socketSetAndCheck
 * Description: Creates the socket and error checks
 * Parameters: int *sockfd
 * Returns: -
 *******************************************************/

void sockSetAndCheck(int *sockfd){

	//IPv4, TCP, flag set to 0
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (*sockfd < 0){
		fprintf(stderr, "Error with socket setup\n");
		exit(1);
	}

	
}



/*******************************************************
 * Function: portAndServerSetup
 * Description: Error checks portnumber, sets up serverAddress
 * 		that will be used with the server/client interface
 * Parameters: int *portNumber. char *argv[], struct sockaddr_in
 * 		*serverAddress
 * Returns: -
 *******************************************************/

void portAndServerSetup(int *portNumber, char *argv[], struct sockaddr_in *serverAddress)
{
	//Takes port number from command line	
	*portNumber = atoi(argv[1]);
	

	//Acceptable range though reserved ports may cause issues
	if (*portNumber < 0 || *portNumber > 65536)
	{
		fprintf(stderr, "Error. Ports need to be between 0 and 65535\n");
		exit(1);

	}

	serverAddress->sin_family = AF_INET; //IPv4
	serverAddress->sin_addr.s_addr = INADDR_ANY;
	serverAddress->sin_port = htons(*portNumber); //converts port number 




}


/*******************************************************
 * Function: bindAndListen
 * Description: Binds socket with serverAddress and error
 * 		checks then sets up backlog listening
 * Parameters: int *sockfd, struct sockaddr_in *serverAddress
 * Returns: -
 *******************************************************/

void bindAndListen(int *sockfd, struct sockaddr_in *serverAddress)
{
	//Binds socket. Returns <0 if error
	if (bind(*sockfd, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) < 0)
	{
		fprintf(stderr, "Error with binding\n");
		exit(1);

	}
	else
//		printf("Success\n");

	//BACKLOG macros. 5 max
	listen(*sockfd, BACKLOG);

} 

/*******************************************************
 * Function: fileTransfer
 * Description: The meat of the program. Two possible 
 * 		executions. If a file is small the encrypt
 * 		process will go char by char which is a relatively
 * 		cool thing but not practical time wise for larger
 * 		files. Second execution takes care of bigger files
 * Parameters: int accept_fd
 * Returns: -
 *******************************************************/
void fileTransfer(int accept_fd)
{
	int fileSize = 0;  //this will change based on the recv number
	char recvBuffer[500]; //holds recv number letter
	char verify[1]; //will be used to check program ID
	
	//Zero the memory. Good practice before send/recv
	bzero(verify, sizeof(verify));
	bzero(recvBuffer, sizeof(recvBuffer));	

	//Retrieve ProgramID first. Remember otp_dec can not connect to this program
	if (recv(accept_fd, verify, sizeof(verify),0) < 0)
	{
		fprintf(stderr, "Error receiving\n");
		return;
	}
	
	//Checks received message and cancels connection if not 1
	if (strcmp(verify, "1") != 0)  //Stop connection
	{
		strcpy(verify, "2");
		send(accept_fd, verify, sizeof(verify), 0);
	
		return;
	}
	else
	{
		strcpy(verify, "1");
		send(accept_fd, verify, sizeof(verify),0);

	}            




	
	int status = 0;
	
	status = recv(accept_fd, recvBuffer, sizeof(recvBuffer),0);
//	
	//Recevies files size
	status = recv(accept_fd, recvBuffer, sizeof(recvBuffer),0);


	//Converts the string to number
	fileSize = atoi(recvBuffer);

	int startSize = 0;

	//Here is the first execution. Small files only
	if (fileSize < 100){
	
		//Total, temp1. temp2 will be used for encrypting
		int total = 0;
		int temp1 = 0;
		int temp2 = 0;
		char toRecv1[1]; //text letter 
		char toRecv2[1]; //key letter
		char toSend[1]; //the one char to send
		
		//Loops while fileSize is bigger than the startSize
		while (fileSize > startSize)
		{
			total = 0;
	
			//Zero memory
			bzero(toRecv1, sizeof(toRecv1));
			bzero(toRecv2, sizeof(toRecv2));
			bzero(toSend, sizeof(toSend));

			//Retrieve 1 letter from text
			if (recv(accept_fd, toRecv1, sizeof(toRecv1),0) < 0)
			{
				fprintf(stderr, "Error receiving\n");
				return;
			}

			//Retrieve 1 letter from key
			if (recv(accept_fd, toRecv2, sizeof(toRecv2), 0) < 0)
			{
				fprintf(stderr, "Error receiving\n");
				return;

			}	
		
			//Conver the letters
			temp1 = letterNumber(toRecv1[0]);
			temp2 = letterNumber(toRecv2[0]);	
	
			//Get the total
			total = temp1 + temp2;
	
			//Chec the modded version of total
			total = modulo(total);
	

			
			//Convert back to letter and send off
			toSend[0] = numberLetter(total);
	

			//Send off the letter
			if (send(accept_fd, toSend, sizeof(toSend), 0) < 0)
			{
				fprintf(stderr, "Error sending at fileSize\n");
				return;

			}

			//Track startSize
			startSize = startSize + sizeof(toRecv1);
	
		}
	}
	else{
	

		


		//This is the big file execution

		char fileBuffer[140000]; //big buffer to hold text/key
	
		char toSend[70000];
					 //doesnt need to be as big since it holds just the encrpyed text
		int recv1 = 0; //tracks received data		
		int send1 = 0; //tracks sent data
		int total = 0;
		int temp1 = 0; //same as before
		int temp2 = 0;
		int i = 0;
		char message[100000]; //will be the text buffer

		//zero the buffers 
		bzero(message, sizeof(message));
		bzero(fileBuffer, sizeof(fileBuffer));
		bzero(toSend, sizeof(toSend));

		int tracker = 0;		
	
		//Loops until transmission end
		while (1)
		{		
			//receieve the data until loop conditions met
			recv1= recv(accept_fd, fileBuffer, fileSize*2,0);

			tracker += recv1;
	//		printf("recv in enc_d: %d\n", recv1);	
		
			//Break loop conditions
			if (recv1 == 0 || recv1 < 0 || tracker >= fileSize*2)
				break;
			
			
			
		}

	

		
	
//		printf("%s\n",fileBuffer);
		//Copies the text portion to a new array so that we can send 
		//both through encrypt process
		strncpy(message, fileBuffer, fileSize);	
		


		int key = fileSize;
//		printf("FIrst letter of key: %c\n", fileBuffer[key]);	
	
		//Loop through filesize and change all letters to appropriate
		// new letters. Same as before
		for (i = 0 ; i < fileSize; i++)
		{
		
			temp1 = letterNumber(message[i]);
		
			temp2 = letterNumber(fileBuffer[key]);
			total = temp1 + temp2;
			total = modulo(total);
			
			toSend[i] = numberLetter(total);			
			key++;
		}
		toSend[i] = '\0'; //Null terminate it
	
//		printf("toSend size: %d\n", sizeof(toSend));

//		printf("FIleSize in enc_d: %d\n", fileSize);
		startSize = 0;
	
		tracker = 0;
		while (fileSize > startSize){

		
			//Send the new encrypted data back	
			send1 = send(accept_fd, toSend, sizeof(toSend),0);
			tracker += send1;
	//		printf("send1 in enc_d %d for fileSize: %d \n", send1, fileSize);	
			if (send1 == 0)
				return;	
		
			//if tracker is size of file it means end of data to be sent
			if (tracker >= fileSize){
	//			fprintf(stderr, "Inside tracker: %d    fileSize: %d\n", tracker, fileSize);
				return;
			
			}
			//error check
			if (send1 < 0)
			{
	//			fprintf(stderr, "Error sending\n");
				return;
			}
			startSize += send1;
		
		}
			

			
		

	}	

}

/*******************************************************
 * Function: main
 * Description: Runs the simulation and calls the appropriate
 * 		server setup functions, forks process off and
 * 		cleans up the aftermath
 * Parameters: int argc, char *argv[]
 * Returns: Returns 0
 *******************************************************/
int main(int argc, char *argv[]){


	//Two sockets used for listening and accepting
	int sockfd, accept_fd;
	int portNumber; //Command line parameter goes here later
	int forkHandler; //handles processes
	socklen_t clilen; //length of client address
	int n;

	//Two structs we need to set up server and connect to client
	struct sockaddr_in serverAddress, clientAddress;
	
	//Zero the memory of serverAddress before setting it up
	bzero((char *)&serverAddress, sizeof(serverAddress));

	//Remember the order: socket, bind, listen, accept


	//Error checks number of args
	if (argc < 2){
		fprintf(stderr, "Error: Incorrect amount of arguments. Usage: otp_enc_d [port]\n");
	}
	

	//Set up socket
	sockSetAndCheck(&sockfd);


	//Set up port and Server. Then bind socket and listen for connections
	portAndServerSetup(&portNumber, argv, &serverAddress);
	bindAndListen(&sockfd, &serverAddress);

	//Set up size of client
	clilen = sizeof(clientAddress);
	int forkCount = 0;


	//setsockopt lets you reuse sockets. Hasnt change much in this implementation but not be more useful with more connections
	int optival = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optival, sizeof(int));

//	Endless server loop to accept new connections
	for(;;)
	{		//accept if connection made 
		accept_fd = accept(sockfd, (struct sockaddr*)&clientAddress, &clilen);
		
		//For bombs are bad  :(   This stops that 
		if (forkCount >= 6){
			close(accept_fd);
			exit(1);
		}
		
		//fork off new process and increase count	
		forkHandler = fork();
		forkCount++;
	
		//If error during process fork, end it
		if (forkHandler == -1)
		{
			close(accept_fd);
			close(sockfd);
			exit(1);	
		}


		//Child process. Start the transfer
		if (forkHandler == 0)
		{
			
			fileTransfer(accept_fd);
	

		}
		//Close the socket and then wait for all child processes to end
		close(accept_fd);	
		int status;
		
		forkHandler = waitpid(-1, &status, WNOHANG); 
		while (forkHandler > 0)
		{
			forkHandler = waitpid(-1, &status, WNOHANG);
		
		
		}
			
		
	
	}
	close(sockfd);






	return 0;
}
