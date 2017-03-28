/**************************************
*Author: Ellard Gerritsen van der Hoop
*CS344 - Assignment 2
*gerritse.adventure.c
*
* **********************************/


/*********************************
Interface:

CURRENT LOCATION: <     >
POSSIBLE CONNECTIONS:
WHERE TO?>

* ****************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>



#define numOfNames 10
#define numRooms 7



//array of room names
char roomNames[numOfNames][20] = {
	"Bedroom",
	"Basement",
	"Attic",
	"LivingRoom",
	"Kitchen",
	"DiningRoom",
	"Bathroom",
	"Foyer",
	"Patio",
	"Office"
};

//array of room types
char roomTypes[3][20] = {
	"START_ROOM",
	"MID_ROOM",
	"END_ROOM"
};


//struct room with all needed information. type,name cur_connectNum, connections(max amount), RoomConnections
struct room {

	char *type; //type of room
	char *name;  //name of room
	int cur_connectNum; //defaults to 0
	int connections; //randomized in generateRooms()
	struct room *RoomConnections[7];  //holds the connections to other rooms
};



//holds the initial data of room before writing and reading
struct room rooms[numRooms];



//Prototypes
int checkName(char *name, int number);
void startEndRandomizer();
void generateRooms();
void generateConnections();
int checkRoomConnect(int r1, int r2);
void printCurrent(struct room currentRoom, int spaceController);
void writeFiles();
int checkEnd(struct room currentRoom);
int switchRoom(struct room *currentRoom, struct room gameRooms[],  char* list[], int steps);
struct room *pick_right_room(char *in);
char *wSpaceRemove(char *str);
void readFiles(struct room gameRooms[]);




//used during creation of rooms. Keeps duplicate rooms from happening
int checkName(char* name, int number){
	int i = 0;


	//	printf("made it inside checkName\n");

	for (i = 0; i < number; i++){

		if (strcmp(name, rooms[i].name) == 0){
			//			printf("Same name\n");
			return 0;
		}


	}

	return 1;

}

//used during creation of rooms. Creates two random numbers and sents these as the start and end
void startEndRandomizer(){
	int randomStart = rand() % 7;
	int randomEnd = rand() % 7;

	//if start and end equal the same, we obviously cant have that. Keep creating a new value until they are different
	while (randomStart == randomEnd)
		randomStart = rand()% 7;


	//Sets the start and end rooms to these values
	rooms[randomStart].type = roomTypes[0];
	rooms[randomEnd].type = roomTypes[2];

}




//this function will create all the rooms 
void generateRooms(){


	int i = 0;
	int three = 0;
	int four = 0;

	//create 7 files with the random information. 
	//we will read it back in later before gameplay starts
	for (i = 0; i < 7; i++){

		char *randomName = roomNames[rand() % 10];
	

		//Checks to see if room is available, if it isnt then we get another room name until it works
		while (checkName(randomName, i) == 0){
			//		printf("%s\n", randomName);
			randomName = roomNames[rand() % 10];
		}
		rooms[i].name = randomName;



		//next is setting up number of connections
		//we can worry about the actual connections later	
		int randomNumber;
		randomNumber = rand() % (6 - 3) + 3;
		 
		

		//The nature of rand() % (6-3) + 3 can mean that we get strange distributions
		//This spices it up a little 
		if (three != 2 && randomNumber == 5){
	
			randomNumber = 3;		
			
			three++;
		}
		if (four != 2 && randomNumber == 5)
		{

			randomNumber = 4;
			four++;
		}
	


		rooms[i].connections = randomNumber;
		rooms[i].cur_connectNum = 0; //sets up cur_connectNum to be the first connection for later



		//Set all rooms to MID and then we will we randomly select the start and end
		rooms[i].type = roomTypes[1];
	}
	
	//Sets the Start and End rooms randomly
	startEndRandomizer(); 



	//Debugging Print script
	/**
	for (i = 0; i < 7; i++)
	{
		printf("Room %d: %s\n", i, rooms[i].name);
		printf("Type:  %s\n", rooms[i].type);
		printf("Connections: %d\n", rooms[i].connections);
	}
		**/
}


//this function is responsbile for generating all the connections between the rooms based
//on the number of connections randomly selected in generateRooms()
void generateConnections(){

	int i = 0;  //i and j are loop controllers
	int j = 0;
	int newRoom;  //this will be used to pick a random room


	//loop through all the rooms
	for (i = 0; i < 7; i++)
	{
		//loop through all connections of each room
		for (j = 0; j < rooms[i].connections; j++)
		{

			newRoom = rand() % 7;

			//need to keep a loop going to check for connectivity 
			//rooms cant connec to themselves and they cant connect twice to others
			while (checkRoomConnect(i, newRoom) == 0){
				newRoom = rand() % 7;
			}
		}
	}

}


//This helper function allows us to check to see if the rooms are feasible connections. 
int checkRoomConnect(int r1, int r2){

	//pointers to the two rooms
	struct room *temp1 = &rooms[r1];
	struct room *temp2 = &rooms[r2];
	int i = 0;

	if (temp1 == temp2)
		return 0;  //same room, exit out


	if (temp1->cur_connectNum == 6)
		return 1;  //reached max, exit out

	//check second room as well
	if (temp2->cur_connectNum == 6)
		return 1;

	for (i = 0; i < temp1->cur_connectNum; i++){
		//cant connect to yourself obviously
		if (temp1->RoomConnections[i] == temp2 && temp1->RoomConnections[i] != NULL)
			return 0;


	}


//	printf("Connection Successful between %s      and    %s\n", temp1->name, temp2->name);

	//Rooms are set to be connected. Connect them and then increment their connection count
	temp1->RoomConnections[temp1->cur_connectNum++] = temp2;
	temp2->RoomConnections[temp2->cur_connectNum++] = temp1;

	return 1;



}


//this function will create the directory, open it, then create the room files in there
void writeFiles(){

	//this handy function returns the process ID which is going to be part of the name of our directory
	int processID = getpid();

	
	//should be enough room for both processID and gerritse.rooms
	char directoryName[20 + sizeof(processID)];
	sprintf(directoryName, "gerritse.rooms.%d", processID);

//	printf("%s\n", directoryName);


	//make the directory and set the permissions
	mkdir(directoryName, 0777);
	//enter the directory
	chdir(directoryName);

	int i = 0;
	int j = 0;
	FILE *fp;

	//this loop will go through the generated rooms and make files out of them 
	for (i = 0; i < 7; i++){
//		printf("Current Room: %s\n", rooms[i].name);
		fp = fopen(rooms[i].name, "w");
		fprintf(fp, "ROOM NAME: %s\n", rooms[i].name);

//		printf("Total connections: %d\n", rooms[i].connections);
		
		//loops through each connection and prints to file, new line for each one
		for (j = 0; j < rooms[i].connections; j++){
//			printf(" Connection name: %d  %s\n", j, rooms[i].RoomConnections[j]->name);
			fprintf(fp, "CONNECTIONS %d: %s\n", j, rooms[i].RoomConnections[j]->name);
		}
		//adds Room type to fie 
		fprintf(fp, "ROOM TYPE: %s\n", rooms[i].type);
//		printf("Room type: %s\n", rooms[i].type);

		fclose(fp);

	}

	//exit out of directory
	chdir("..");
//	printf("All Rooms Named\n");







}


//this function is responsible for printing current rooms information.
void printCurrent(struct room currentRoom, int spaceController){

	int i = 0;

	//this is just a way of controller the newlines to adhere to project formatting rules
	if (spaceController == 0)
		printf("\n\nCURRENT LOCATION:  ");
	else if (spaceController == 1)
		printf("\nCURRENT LOCATION:  ");
	
	printf("%s\n", currentRoom.name);

	//print connections
	printf("POSSIBLE CONNECTIONS: ");
	for (i = 0; i < currentRoom.connections; i++)
	{

		//if last connection for room, we need to add a period and new line 
		if (i == currentRoom.connections - 1)
			printf("%s.\n", currentRoom.RoomConnections[i]->name);
		else //print name with a comma and space
			printf("%s, ", currentRoom.RoomConnections[i]->name);
	}
	printf("WHERE TO?>");
	//fgets(





}

//this function is responsible for letting a user change rooms.
//it checks user input, if good room will change else it will show failure message
int switchRoom(struct room *currentRoom, struct room gameRooms[], char **list, int steps)
{
	//should be enough room for input
	char* inputRoom = (char *)malloc(15);
	int i = 0;
	int goodName = 0;
	fgets(inputRoom, 15, stdin); //gets line from standard input

	wSpaceRemove(inputRoom);  //trims whitespace if any
//	printf("%s\n", inputRoom);

	//Loops through each room to check if the rooms match up 
	for (i = 0; i < currentRoom->connections; i++)
	{
		if (strcmp(inputRoom, currentRoom->RoomConnections[i]->name) == 0){
			//printf("made it within strcmp\n");
			goodName = 1;  //a good input 
			list[steps] = currentRoom->RoomConnections[i]->name;  //add room to list that will be displayed at end
			*currentRoom = *currentRoom->RoomConnections[i];    //change current Room to new room
			break;
		}
		//do nothing

	}
	
	//display failure message if goodName wasn't picked else. 
	if (goodName == 0)
	{
		printf("\n\nHUH? I DON'T UNDERSTAND ROOM. TRY AGAIN.\n\n");
		free(inputRoom); //free up space
		return 0;  
	}
	else if (goodName == 1){
		free(inputRoom); //free up space
		return 1;
	}



}


//this function is responsible for seeing if current room is in fact the end room. Returns 1 if it is 
int checkEnd(struct room currentRoom){
	char end[20] = "END_ROOM";
	//	printf("roomTypes[3]: %s\n", roomTypes[3]);
	//	printf("currentRoom Type: %s\n", currentRoom.type);
	if (strcmp(currentRoom.type, end) == 0) 
		return 1;

	return 0;

}

//Modified version from stackoverflow.com/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
//This removes the whitespace. It cleans up some of the strings 
char *wSpaceRemove(char *str){
	int len = 0;  //unsigned type for length
	char *frontp = str;  //set pointer to parameter
	char *endp = NULL;  //pointer equal to NULL

//	if (str == NULL) { return NULL; }   
	if (str[0] == '\0') { return str; } //if first spot is null term


	len = strlen(str);  //len is set qual to length of string parameter
	endp = str + len;   //endp is equal to string + string length


	//isspace checks for whether a passed character is a white space. 
	while (isspace(*frontp)) { ++frontp; }      //trim the leading space
	if (endp != frontp){              
		while (isspace(*(--endp)) && endp != frontp){} //trim the trailing
	}


	if (str + len - 1 != endp)
		*(endp + 1) = '\0'; //sets to nul term
	else if (frontp != str && endp == frontp)
		*str = '\0'; 

	endp = str;
	if (frontp != str)
	{
		while (*frontp) { *endp++ = *frontp++; }
		*endp = '\0';
	}

	//returns the cleaned up string
	return str;



}


//this functions job is to just read in all the data from the files we 
//created. then use that data for the actual game and now the data we used 
//to create the file. 
void readFiles(struct room gameRooms[]){

	FILE *fp;

	//relies on dirent.h. Creates the appropriate struct and DIR variable that will allow us to read all files in directory
	struct dirent *pDirent;
	DIR *pDir;
	int count = 0; //this variable will control which room we reading to

	//same as before. we are going to open the directory where we originally stored the files
	int processID = getpid();
	char directoryName[15 + sizeof(processID)];
	sprintf(directoryName, "gerritse.rooms.%d", processID);

	chdir(directoryName);

	//some char arrays we will use to retrieve data
	char roomType[12];
	char roomName[20];
	char connectionName[20];

	//open the directory and print error if we cant
	pDir = opendir(".");
	if (pDir == 0){
		printf("Directory problem\n");
		
	}

	char string[40];
	//time to read in the data
	while ((pDirent = readdir(pDir)) != NULL){
		fp = fopen(pDirent->d_name, "r");  //we only need to read the info so "r" works


		//printf("[%s]\n", pDirent->d_name);		
		//there are hidden directories like .  and ..   We dont want those 
		if ('.' == pDirent->d_name[0])
			continue; 

		//scans for the ROOM NAME 
		fscanf(fp, "ROOM NAME: %s\n", pDirent->d_name);
		
		//alocate the space and then copies the information from file name to gameRooms name
		gameRooms[count].name = (char *)malloc(20);
		strcpy(gameRooms[count].name, pDirent->d_name);
	

		//All the fun variables we will need to use to get the connections read in accurately
		int i = 0;  // outer loop
		int j = 0; //inner loop
	
		char *thing; //thing and thing2 are helper variables to cut down the string we get from fgets
		char *thing2;
		const char ch = ':';  //we dont want : when we read in our name
		const char ch2 = ' '; //the white space is also removed though trim could be used as well

		//loop through total number of connections
		for (i = 0; i < rooms[count].connections; i++)
		{
			fgets(string, 50, fp);  //gets line of text up til newline
			//			printf(" %s\n", string);
			thing = strchr(string, ch);  //using strchr to remove : and ' '
			thing2 = strchr(thing, ch2); 
			//			printf(" %s\n", thing2);
			thing2 = wSpaceRemove(thing2); //trim any extra white space that might have happened when writing/reading
			
			//loop through avaible rooms in game and then compare the values and set rooms 
			for (j = 0; j < 7; j++)
			{
				if (strcmp(thing2, rooms[j].name) == 0)
				{
			//		printf("%s\n", rooms[i].name);
					gameRooms[count].RoomConnections[i] = &rooms[j];
				}
			}

		}
		gameRooms[count].connections = rooms[count].connections;  //we also need the appropriate total connections


		//scan for room type
		fscanf(fp, "ROOM TYPE: %s\n", roomType);
	//	printf("ROOMtype: %s\n", roomType);
		gameRooms[count].type =  (char *)malloc(20); //allocate the memory needed
		strcpy(gameRooms[count].type, roomType); //copy string over 

		//close current file and increment count for next room
		fclose(fp);
		count++;


	}


	//close directory 
	closedir(pDir);
	chdir("..");


}




int main(){
	srand(time(NULL));  //sets random seed. otherwise the same results happen each time
	int victory = 0;  //controls victory condition
	int i; 
	int j;
	int steps = 0; //increments on appropriate room movement
	char *stepName[15]; //this will hold all the rooms we have traveled through
	struct room gameRooms[numRooms]; //struct array of rooms 
	struct room currentRoom; //struct room that holds our current location

	//create the room, create the connections, write the files, then read them back in
	generateRooms();
//	printf("Generate rooms -pass\n");
	generateConnections();
//	printf("Generate connections - pass\n");
	writeFiles();
//	printf("writeFiles pass\n");
	readFiles(gameRooms);     //   this will load the data in from files
//	printf("readFiles pass\n");

	
	

	//Finds and then sets the starting room at the currentRoom
	for (i = 0; i < 7; i++)
	{	
	//	printf("gameRooms type:  %s    roomTypes: %s\n", gameRooms[i].type, roomTypes[0]);
	
		if (strcmp(gameRooms[i].type, roomTypes[0]) == 0){
			currentRoom = gameRooms[i]; //sets currentRoom is a match
		//	printf("currentRoom: %s\n", currentRoom.name);
			break;
		}
	}
	
//	printf("Game Start\n");

	//While loop thats dictated by victory condition
	while (victory == 0){

		//Checks if the currentRoom is infact the end room. Sets victory to 1 to stop while loop if true
		if (checkEnd(currentRoom) == 1){
			victory = 1;
		}
		else{
			printCurrent(currentRoom,0);  //Prints current room, connections, and type 
			while (switchRoom(&currentRoom, gameRooms, stepName, steps) == 0){ //if input is wrong it will keep looping 
				printCurrent(currentRoom,1);
			}
			steps++;  //only increment steps based on successful room transfer
		}




	}
	printf("\n\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);

	//Show all the rooms we traveled through
	for (i = 0; i < steps; i++)
	{
		printf("%s\n", stepName[i]);
	}

	//Lets clean up that memory
	for (i = 0; i < 7; i++)
	{
		free(gameRooms[i].type);
		free(gameRooms[i].name);
	}	
	
	return 0;
}
