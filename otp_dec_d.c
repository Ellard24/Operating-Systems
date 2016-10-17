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


int modulo(int a)
{
	if (a < 0) 
		a = a + 27;
	
	int number = a % 27;

	//C % is actually a remainder which doesnt work for negative numbers so we need this 	
	if (number < 0)
		return number + 27;
	else
		return number;

}

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
			return 'M';
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

void sockSetAndCheck(int *sockfd){

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (*sockfd < 0){
		fprintf(stderr, "Error with socket setup\n");
		exit(1);
	}

	
}

void portAndServerSetup(int *portNumber, char *argv[], struct sockaddr_in *serverAddress)
{
	
	*portNumber = atoi(argv[1]);
	
	if (*portNumber < 0 || *portNumber > 65536)
	{
		fprintf(stderr, "Error. Ports need to be between 0 and 65535\n");
		exit(1);

	}

	serverAddress->sin_family = AF_INET;
	serverAddress->sin_addr.s_addr = INADDR_ANY;
	serverAddress->sin_port = htons(*portNumber); 




}

void bindAndListen(int *sockfd, struct sockaddr_in *serverAddress)
{

	if (bind(*sockfd, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) < 0)
	{
		fprintf(stderr, "Error with binding\n");
		exit(1);

	}
	else
//		printf("Success\n");

	listen(*sockfd, BACKLOG);

} 


void fileTransfer(int accept_fd)
{
	int fileSize = 0;
	char recvBuffer[500];
	char verify[1];
	
	bzero(verify, sizeof(verify));
	bzero(recvBuffer, sizeof(recvBuffer));	

	//Retrieve ProgramID first. Remember otp_dec can not connect to this program
	if (recv(accept_fd, verify, sizeof(verify),0) < 0)
	{
		fprintf(stderr, "Error receiving\n");
		return;
	}
	
//	printf("Received program id\n");
//	printf("ID: %c\n", verify[0]);
	if (strcmp(verify, "2") != 0)  //Stop connection
	{
		strcpy(verify, "1");
		send(accept_fd, verify, sizeof(verify), 0);
		return;
	}
	else
	{
		strcpy(verify, "2");
		send(accept_fd, verify, sizeof(verify),0);

	}            

//	printf("passed strcmp\n");

	//Retrieve File size first
//	printf("right before reciving file size\n");
	
	int status = 0;
	
	status = recv(accept_fd, recvBuffer, sizeof(recvBuffer),0);
//	printf("Status: %d\n", status);	
	status = recv(accept_fd, recvBuffer, sizeof(recvBuffer),0);
//	printf("passed file size\n");

//	printf("received number: %d\n", ntohl(recFile));
//	printf("Received message: %s\n", recvBuffer);	
	fileSize = atoi(recvBuffer);
//	printf("fileSize: %d\n", fileSize);
	int startSize = 0;
	
	fileSize = fileSize - 1;
//	printf("fileSize: %d\n",fileSize);
	int total = 0;
	int temp1 = 0;
	int temp2 = 0;
	char toRecv1[1];
	char toRecv2[1];
	char toSend[1];
	while (fileSize > startSize)
	{
		total = 0;
//		printf("inside encryption loop\n");
		bzero(toRecv1, sizeof(toRecv1));
		bzero(toRecv2, sizeof(toRecv2));
		bzero(toSend, sizeof(toSend));
		if (recv(accept_fd, toRecv1, sizeof(toRecv1),0) < 0)
		{
			fprintf(stderr, "Error receiving\n");
			return;
		}

//		printf("Received 1: %c\n", toRecv1[0]);	
		if (recv(accept_fd, toRecv2, sizeof(toRecv2), 0) < 0)
		{
			fprintf(stderr, "Error receiving\n");
			return;

		}	
//		printf("Received 2: %c\n", toRecv2[0]);
		temp1 = letterNumber(toRecv1[0]);
		temp2 = letterNumber(toRecv2[0]);	
//		printf("Temp: %d     Temp2:   %d\n", temp1, temp2);

		total = temp1 - temp2;
//		printf("Total: %d\n", total);

		total = modulo(total);
//		printf("Total after mod: %d\n", total);
	
	
		toSend[0] = numberLetter(total);
//		printf("To send: %c\n", toSend[0]);

		if (send(accept_fd, toSend, sizeof(toSend), 0) < 0)
		{
			fprintf(stderr, "Error sending\n");
			return;

		}

		
		startSize = startSize + sizeof(toRecv1);
//		printf("Start Size: %d\n", startSize);
	}

	

//	printf("Waiting for clients\n");


}


int main(int argc, char *argv[]){



	int sockfd, accept_fd;
	int portNumber;
	int forkHandler;
	socklen_t clilen;
	int n;

	struct sockaddr_in serverAddress, clientAddress;
	
	bzero((char *)&serverAddress, sizeof(serverAddress));

	//Remember the order: socket, bind, listen, accept



	if (argc < 2){
		fprintf(stderr, "Error: Incorrect amount of arguments. Usage: otp_enc_d [port]\n");
	}
	


	sockSetAndCheck(&sockfd);

	portAndServerSetup(&portNumber, argv, &serverAddress);
	bindAndListen(&sockfd, &serverAddress);

	clilen = sizeof(clientAddress);
	int forkCount = 0;

//	printf("Waiting for clients\n");
	for(;;)
	{
		accept_fd = accept(sockfd, (struct sockaddr*)&clientAddress, &clilen);
	/*	
		if (forkCount < 6){
			forkHandler = fork();
			forkCount++;
		}
		else{
			close(accept_fd);
			break;
		}
		if (forkHandler == -1)
		{
			close(accept_fd);
			continue;	
		}

		if (forkHandler == 0)
		{
			fileTransfer(accept_fd);

		}
		close(accept_fd);	
	*/
//		printf("Before file transfer function\n");
		fileTransfer(accept_fd);	
	}

	close(sockfd);






	return 0;
}
