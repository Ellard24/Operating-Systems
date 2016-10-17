/***********************************
 * Ellard Gerritsen van der Hoop
 * CS344 OTP
 *
 * gcc -o otp_enc otp_enc.c
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


void sockSetAndCheck(int *sockfd)
{
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (*sockfd < 0)
	{
		fprintf(stderr, "Error. Socket problems\n");
		exit(1);
	}



}


void portAndServerSetup(int *portNumber, char *argv[], struct sockaddr_in *serverAddress, struct hostent **server)
{
	char host[] = "localhost";
	
	*portNumber = atoi(argv[3]);


	if (*portNumber < 0 || *portNumber > 65536)
	{
		fprintf(stderr, "Error. Port needs to be between 0 and 65535\n");
		exit(2);

	}

	serverAddress->sin_family = AF_INET;
	serverAddress->sin_port = htons(*portNumber);

	*server = gethostbyname(host);

	if (*server == NULL)
	{
		fprintf(stderr, "Error. Can not find host :( \n");
		exit(1);
	}

}


int fileCheck(char *argv[], int *fileSize)
{
	struct stat st;
	int fileSize1;
	int fileSize2;
	int c;

	FILE *fp;
	fp  = fopen(argv[1], "r");
	
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


	if (stat(argv[1], &st) != 0)
	{
		fprintf(stderr, "Error with file size\n");
		exit(1);
	}
	else
		fileSize1 = st.st_size;

	if (stat(argv[2], &st) != 0)
	{
		fprintf(stderr, "Error with file size\n");
		exit(1);
	}
	else
		fileSize2 = st.st_size;



	if (fileSize1 != fileSize2)
		return -1;
	else{
		*fileSize = fileSize1;
		return 0;
	}





}

void fileTransfer(int *sockfd, char *argv[], int fileSize)
{

	//First the plaintext
	FILE *fp1;
	FILE *fp2;
	fp1 = fopen(argv[1], "r");	
	fp2 = fopen(argv[2], "r");

	//First Send file size over
	char fileSendSize[500];
	bzero(fileSendSize, sizeof(fileSendSize));
	sprintf(fileSendSize, "%d", fileSize);
	
//	printf("Make it to file Transfer\n");
//	printf("File Size %d    %s\n", fileSize, fileSendSize);
	//sending filesize over 
	if (send(*sockfd,fileSendSize, strlen(fileSendSize),0) < 0){
//	if (send(sockfd, fileSendSize, sizeof(fileSendSize),0) < 0){
		fprintf(stderr, "Error sending\n");
		exit(1);
	}
//	printf("made it past file size transfer\n");
	
	//going to loop through all characters and send them over 
	int ch1;
	int ch2;
	char sending1[1];
	char sending2[1];
	char receiving[1];
	while (((ch1 = getc(fp1)) != EOF)  && ((ch2 = getc(fp2)) != EOF))
	{
	
//		printf("Inside while loop in file transfer\n");
		bzero(sending1, sizeof(sending1));
		bzero(sending2, sizeof(sending2));
		bzero(receiving, sizeof(receiving));
			

		sending1[0] = ch1;
		sending2[0] = ch2;	

		if (sending1[0] == '\n')
			break;


//		printf("%c\n", sending1[0]);
		if (send(*sockfd, sending1, sizeof(sending1),0) < 0){
			fprintf(stderr, "Error sending\n");
			exit(1);
		}
		
//		printf("%c\n", sending2[0]);
		if (send(*sockfd, sending2, sizeof(sending2),0) < 0){
			fprintf(stderr, "Error sending\n");
			exit(1);
		}

		if (recv(*sockfd, receiving, sizeof(receiving), 0) < 0){
			fprintf(stderr, "Erro receiving\n");
			exit(1);

		}

		printf("%c",receiving[0]);
//		printf("Made it through one loop\n");

	}
//	printf("Transfer complete");
	fclose(fp1);
	fclose(fp2);

	



}



int main(int argc, char *argv[])
{

	int sockfd;
	int portNumber;
	int programNumber = 1;  //this will be used to verify connection eligibility
	int fileSize;
	int n;
	struct sockaddr_in serverAddress;
	struct hostent *server;
	
	
	if (argc < 4){
		fprintf(stderr, "Error. Incorrect amount of parameters. Usage: otp_enc [textfile] [key] [port]\n");
		exit(1);
	}

	//Check file sizes 

	if (fileCheck(argv, &fileSize) != 0)
	{
		fprintf(stderr, "Error. File sizes not the same\n");
		exit(1);
	}

//	printf("After file check\n");
	sockSetAndCheck(&sockfd);
	
	bzero((char *)&serverAddress, sizeof(serverAddress));
	portAndServerSetup(&portNumber, argv, &serverAddress, &server);

	bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);

//	printf("before connect\n");
	if (connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){

		fprintf(stderr, "Error connecting to server\n");
		exit(1);
	}

//	printf("after connect\n");
	//Quick verification to make sure otp_enc is able to connect to otp_dec_d
	char programID[10];
	char programRes[10];
	bzero(programRes, sizeof(programRes));
	bzero(programID, sizeof(programID));
	sprintf(programID, "%d", programNumber);

//	printf("ProgramID: %d    %s  \n",programNumber, programID);
//	printf("before sending programID\n");
	//sennding programID 
	if (send(sockfd, programID, sizeof(programID),0) < 0){
		fprintf(stderr, "Error sending\n");
		exit(1);
	}

	bzero(programID, sizeof(programID));
	if (recv(sockfd, programID, sizeof(programID), 0) < 0){
		fprintf(stderr, "Error receiving\n");
		exit(1);
	}		


	
//	printf("programID: %s\n", programID);
	if (strcmp(programID, "1") != 0)
	{
		fprintf(stderr, "Error. Communication with otp_dec_d is not allowed\n");
		exit(1);
	}
	else{
//		printf("Just starting file transfer function\n");
		fileTransfer(&sockfd, argv, fileSize);
	}


	//fileReceive 



	close(sockfd);









	return 0;
}
