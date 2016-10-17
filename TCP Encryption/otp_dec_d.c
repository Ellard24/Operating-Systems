/*********************************************
 *Ellard Gerritsen van der Hoop
 *CS344 - OTP 
 *
 *
 *otp_dec_d.c
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
 * Function: modulo
 * Description: Checks for if number is below 0 and adds
 * 		27 if need be. Then mods by total number
 * 		of available letters/space
 * Parameters: int a
 * Returns: Returns modulated number
 *******************************************************/
int modulo(int a)
{
	if (a < 0) 
		a = a + 27;
	
	a = a % 27;

	//C % is actually a remainder which doesnt work for negative numbers so we need this 	
	
	return a;

}

/*******************************************************
 * Function:letterNumber
 * Description: Returns a number based on the letter 
 * 		passed in
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
 * Description: Does the oppposite of letterNummber. 
 * 		Turns a number into a letter
 * Parameters: int number
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
 * Function: sockSetAndCheck
 * Description: Creates the socket and error checks
 * Parameters: int *sockfd
 * Returns: -
 *******************************************************/
void sockSetAndCheck(int *sockfd){

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (*sockfd < 0){
		fprintf(stderr, "Error with socket setup\n");
		exit(1);
	}

	
}

/*******************************************************
 * Function: portAndServerSetup
 * Description: Error checks portNumber, sets up serverAddress
 * 		that will be used with the server/client interface
 * Parameters: int *portNumber, char *argv[], struct sockaddr_in
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
	serverAddress->sin_port = htons(*portNumber); //Converts to usable port Number




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
	//Bind function. Returns < 0 if error
	if (bind(*sockfd, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) < 0)
	{
		fprintf(stderr, "Error with binding\n");
		exit(1);

	}
	else
//		printf("Success\n");

	//Uses macro to set up 5 possible connections
	listen(*sockfd, BACKLOG);

} 

/*******************************************************
 * Function: fileTransfer
 * Description: The meat of the program. Two possible 
 * 		executions. If a file is small the the decrypt
 * 		process will go char by char which is a relatively 
 * 		cool thing but the grader would likely want to kill
 * 		me since with plaintext4 it would take waaaay 
 * 		too long. Thus the second execution here basically
 * 		stores both the text and key in one buffer and 
 * 		then converts.
 * Parameters: int accept_fd
 * Returns: -
 *******************************************************/
void fileTransfer(int accept_fd)
{
	int fileSize = 0; //this will change based on the recv number
	char recvBuffer[500]; 
	char verify[1];  //will be used to check if program will run

	//Zero the memory. Common practice before send/recv	
	bzero(verify, sizeof(verify));
	bzero(recvBuffer, sizeof(recvBuffer));	

	//Retrieve ProgramID first. Remember otp_enc can not connect to this program
	if (recv(accept_fd, verify, sizeof(verify),0) < 0)
	{
		fprintf(stderr, "Error receiving\n");
		return;
	}
	

	//Checks received message and cancels connection if not 2
	if (strcmp(verify, "2") != 0)  //Stop connection
	{
		strcpy(verify, "1");
		send(accept_fd, verify, sizeof(verify), 0);
		return;
	}
	else
	{
		strcpy(verify, "2"); //good connection
		send(accept_fd, verify, sizeof(verify),0);

	}            


	
	int status = 0;
	
	
	status = recv(accept_fd, recvBuffer, sizeof(recvBuffer),0);

	//Receives file size 
	status = recv(accept_fd, recvBuffer, sizeof(recvBuffer),0);

	int startSize = 0;
	//Converts the string to a number
	fileSize = atoi(recvBuffer);

	//Here is the first execution. Small files only
	if (fileSize < 100){

		//Total,temp1, temp2 will be used for decrypting
		int total = 0;
		int temp1 = 0;
		int temp2 = 0;
		char toRecv1[1]; //Cipher letter
		char toRecv2[1]; //Key letter
		char toSend[1]; //the one char to return
		while (fileSize > startSize)
		{
			total = 0;
			//zero memory
			bzero(toRecv1, sizeof(toRecv1));
			bzero(toRecv2, sizeof(toRecv2));
			bzero(toSend, sizeof(toSend));
		
			//retrieve 1 letter from cipher
			if (recv(accept_fd, toRecv1, sizeof(toRecv1),0) < 0)
			{
				fprintf(stderr, "Error receiving\n");
				return;
			}

			//retrieve 1 letter from key
			if (recv(accept_fd, toRecv2, sizeof(toRecv2), 0) < 0)
			{
				fprintf(stderr, "Error receiving\n");
				return;

			}	
	
			//Convert the letters
			temp1 = letterNumber(toRecv1[0]);
			temp2 = letterNumber(toRecv2[0]);	
	
			//Get the total
			total = temp1 - temp2;
	
			//Send to modulo function to get appropriate value
			total = modulo(total);
	
		
			//Convert back to letter and send off
			toSend[0] = numberLetter(total);
		

			if (send(accept_fd, toSend, sizeof(toSend), 0) < 0)
			{
				fprintf(stderr, "Error sending\n");
				return;

			}

			//Track startSize		
			startSize = startSize + sizeof(toRecv1);

		}
	}
	else{
		//This is the big file execution
		char fileBuffer[140000];
		char toSend[70000];
		int recv1 = 0;	 //tracks received data
		int send1 = 0;  //tracks sent data
		int total = 0;  
		int temp1 = 0;  //same as before
		int temp2 = 0;
		int i = 0;
		char message[100000];   //will be the text buffer
		bzero(message, sizeof(message));

		
		bzero(fileBuffer, sizeof(fileBuffer));
	
		
		//helps with tracking data. Might be overkill but it works
		int tracker = 0;
		startSize = fileSize;	
		//Loops until transmission end
		while (1)
		{
			//receive data until no more data	
			recv1 = recv(accept_fd, fileBuffer, fileSize*2,0);
	//		printf("recv1: %d\n", recv1);
			tracker += recv1;
			if (recv1 == 0 || recv1 < 0 || tracker >= fileSize *2)
				break;

		}
			
		//Copies the cipher portion to a new array so that we can send 
		//both through decrypt process
		strncpy(message,fileBuffer, fileSize);
		
		//this holds the place where the text changes to key
		int key = fileSize;
		
		//Calls upon encrypt helpers letterNumber and numberLetter to change each letter
		for (i = 0; i < fileSize; i++)
		{
			temp1 = letterNumber(message[i]);
			temp2 = letterNumber(fileBuffer[key]);

			total = temp1 - temp2;
			total = modulo(total);
			
			toSend[i] = numberLetter(total);			
			key++;
		}
		toSend[i] = '\0'; //Null terminate it
	
		startSize = 0;
	
//		printf("FileSize: %d\n", fileSize);

		//Loop through and sent all data while startSize is less than fileSize
		while(fileSize  > startSize){
			
			//Send the data back to client
			send1 = send(accept_fd, toSend, fileSize,0);		
//			printf("Send1 in dec_d: %d\n",send1);	
			if (send1 == 0)
				return;
			if (send1 < 0)
			{
				fprintf(stderr, "Error sending\n");
				return;
			}
	
			//increment startSize with sent data
			startSize += send1;
		

		}
			
//		printf("finished\n");	

			
		


	}	




}



/*******************************************************
 * Function: main
 * Description: Runs the simulation and calls the appropriate
 * 		server setup functions, forks proces off and
 * 		cleans up the aftermath
 * Parameters: int argc, char *argv[]
 * Returns: Returns 0
 *******************************************************/

int main(int argc, char *argv[]){


	//Two sockets used for listening and accepting
	int sockfd, accept_fd;
	int portNumber; //command line parameter goes here later
	int forkHandler; //handles procceses
	socklen_t clilen; //length of client address
	int n;

	//Two structs we need to set up server and connect to client
	struct sockaddr_in serverAddress, clientAddress;
	
	//Zero the memory of serverAddress. Common practice
	bzero((char *)&serverAddress, sizeof(serverAddress));

	//Remember the order: socket, bind, listen, accept


	//Error check number of arguments
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

	//setsockopt lets you reuse sockets. Hasn't change much in this implementation
	//but might be useful for more connections
	int optival = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optival, sizeof(int));

	//Endless server loop to accept new connections
	for(;;)
	{
		accept_fd = accept(sockfd, (struct sockaddr*)&clientAddress, &clilen);
		//Fork bombs are bad  :(   This stops that from happening
		if (forkCount >= 6){ 
			
			close(accept_fd);
			close(sockfd);
			exit(1);
		}
		//fork off new process and increase count
		forkHandler = fork();
		forkCount++; 	
		

		//If error during process fork, end it
		if (forkHandler == -1)
		{
			close(accept_fd);
			exit(1);
		}

		//Child process. Start the transfer
		if (forkHandler == 0)
		{	
			fileTransfer(accept_fd);
	//		childHandler(&forkHandler);
			

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
