/*	avatar.c	Each avatar runs this code to move throughout the maze

	Project name: Amazing Final Project
	Component name: avatar
	
	Primary Authors: Andrew Ogren, Amos Cariati, James Thompson III
	Date Created:	02/29/16
	
	Input: Passed from AMStartup.c
		1.) nAvatars : total number of Avatars
		2.) Difficulty: difficulty of the maze
		3.) IP address of the server (129.170.212.235)
		4.) MazePort : as returned in the AM_INIT_OK message
		5.) Filename of the log the Avatar should open for writing in append mode.
		6.) The width of the maze.
		7.) the height of the maze.

	Command Line options: None

	Output: 
		The first line with the user, mazeport, date, and time is written to the log file.

		The positions of the avatars within the maze are logged for each turn. This is sent to the
		logfile that was passed as a parameter. Then, when the maze is solved or too many moves are used,
		this is also logged in the logfile.

		The maze graphics will also be updated after every round of turns. These are ASCII graphics and
		can simply be viewed on the terminal.

	Error conditions:
		-memory allocation failure
		-send or receive message failure
		-improper message sent/received

	Pseudocode: Check design_spec.pdf

	Special considerations:  
		(e.g., special compilation options, platform limitations, etc.) 
	
======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <ncurses.h>



// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
#include "avatar.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions
#define TILE_W 8
#define TILE_H 3
// ---------------- Structures/Types 
Direction **mazeArray;
midPoint midpoint;
// ---------------- Private variables
int numAvatars; //allows all of the function to use the number of avatars in for-loops
int avt; //For testing purposes
int done = 0; //make sure the program doesn't exit until all of the threads have cleaned up 
int started = 0; //make sure to not update the maze array until every avatar has had a chance to move
int gotMidpoint = 0; //only create the midpoint once
int turn; //keeps track of the total number of turns that have been made
int winningHash = 0; //the hash number that is returned when the maze is solved
char *fileLog; //the file to write moves and winning hash to
FILE *fp; //the pointer to the file that moves are written to

int height; //height of the maze
int width; //width of the maze



// ---------------- Private prototypes 

/*====================================================================*/


int main(int argc, char **argv){
	//Don't have to check parameters, because they are passed directly from 
	//AMStartup.c, and so they are definitely correct

	//Set the passed arguments
	char *ip = argv[3]; 
	int mazePort = atoi(argv[4]);
	fileLog = argv[5];

	//open output log file
	fp = fopen(fileLog, "w");
	if (fp == NULL) {
		fprintf(stdout, "Error: Could not open the file %s for writing", fileLog);
		exit(1);
	}

	//Add the first line to the logfile
	char *userName = getlogin();

	char *currentTime;
	time_t *timeSpace = malloc(sizeof(time_t));
	time(timeSpace);
	if (timeSpace != NULL){
		currentTime = ctime(timeSpace);
	}
	else {
		fprintf(stdout, "Error: Failed to get the current time.");
		exit(1);
	}

	int numInts= snprintf(NULL, 0, "%d", mazePort);
	char *firstLine=(char*)malloc(sizeof(char)*(strlen(userName)+ \
	strlen(currentTime)+strlen(", , \n")+numInts+1));

	if (firstLine == NULL){
		fprintf(stdout, "Error: Memory allocation failure.");
		exit(1);
	}
	sprintf(firstLine, "%s, %d, %s", userName, mazePort, currentTime);
	fprintf(fp, "%s\n", firstLine);

	free(timeSpace);
	free(firstLine);


	//Initialize main data structure (mazeArray)
	width = atoi(argv[6]); 
	height = atoi(argv[7]);
	int initializeSuccess = InitializeMazeArray(width, height);
	if (initializeSuccess == 1){
		fprintf(stdout, "Error: Memory allocation failure.");
		exit(1);
	}
	printf("Initialized Maze:\n");

	fprintf(stdout, "Initialized mazeArray. Now threading...\n");
	avt = 1;
	numAvatars = atoi(argv[1]);
	argStruct args;
	args.width = width;
	args.height = height;
	args.mazePort = mazePort;
	args.ip = ip;
	args.fileLog = fileLog;
	pthread_t threads[numAvatars];
	//Create the threads for each of the avatars here
	turn = 0;
	for(int i=0; i<numAvatars; i++){
		args.id = i;
		int iret1 = pthread_create(&threads[i], NULL, &avatarRun, (void*)&args);
		if (iret1) {
			fprintf(stderr, "pthread_create failed, rc=%d\n",iret1);
			exit(iret1);
		}
		sleep(2);
	}
	fprintf(stdout, "Created all of the threads for the avatars.\n");
	fflush(stdout);

	// Initialize Maze Display and colors
	initscr();
	start_color();
	init_pair(1,COLOR_GREEN,COLOR_RED);
	init_pair(2,COLOR_MAGENTA,COLOR_GREEN);
	init_pair(3,COLOR_RED,COLOR_CYAN);
	init_pair(4,COLOR_BLUE,COLOR_MAGENTA);
	init_pair(5,COLOR_WHITE,COLOR_BLACK); //default
	init_pair(6, COLOR_YELLOW,COLOR_BLUE);

	//join the threads so that it doesn't exit until all threads have returned
	for(int i=0; i<numAvatars; i++){
		pthread_join(threads[i], NULL);
	}

	while(1){
		if (done == numAvatars){
			break;
		}
	}
	//cleanup 
	fclose(fp);
	FreeMazeArray(width, height);
	endwin();


	return 0;
}


void* avatarRun(void *arguments){
	//Get the passed arguments out of the argstruct
	struct argStruct *args = (struct argStruct*)arguments;
	int id = args->id;
	fprintf(stdout, "%d\n", id);
	int mazePort = args->mazePort;
	fprintf(stdout, "%d\n", mazePort);
	char *ip = args->ip;

	fprintf(stdout, "I am avatar number %d\n", avt);
	avt = avt + 1;

	//Create the breadCrumbsList for each of the avatars
	breadCrumbList *breadList = InitializeBreadList();
	if (breadList == NULL){
		fprintf(stdout, "Error: Memory allocation failure.");
		exit(1);
	}

	//Initialize the prevPosition struct;
	prevPosition prevPos;

	/* Adapted from echoClient.c example */
	int sockfd;
	struct sockaddr_in servaddr;

	//Create a socket for the client
	//If sockfd<0 there was an error in the creation of the socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0) {
		perror("Problem in creating the socket");
		exit(2);
	}
	//Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr= inet_addr(ip); // flume.cs.dartmouth.edu IP
	servaddr.sin_port =  htons(mazePort); //convert to big-endian order

	//Connection of the client to the socket 
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
		perror("Problem in connecting to the server");
		exit(3);
	}
	fprintf(stdout, "Connection Succesful.\n"); // If we got here, connection didnt fail

	AM_Message send_signal;
	AM_Message receive_signal;
	memset(&receive_signal, 0, sizeof(receive_signal));
	// Create AM_AVATAR_READY signal
	send_signal.type = htonl(AM_AVATAR_READY);
	send_signal.avatar_ready.AvatarId = htonl(id);

	// Send initial signal to server
	if (send(sockfd, &send_signal, sizeof(send_signal), 0) == -1) {
		fprintf(stdout, "Error: send failed.");
		FreeBreadCrumbList(breadList);
		shutdown(sockfd, 2);
		exit(1);
	} 
	//keep running until program exits on error or all of the avatars have received the winning message
	while (done < numAvatars){
		AM_Message serverResponse;
		memset(&serverResponse, 0, sizeof(serverResponse));
		//Check the possible failure of the received message from the server
		int recvSize = 0;
		recvSize = recv(sockfd, &serverResponse, sizeof(serverResponse), 0);
		if ((recvSize) < 0){
			FreeBreadCrumbList(breadList);
			fprintf(stdout, "Error: Couldn't receive message from the server.");
			shutdown(sockfd, 2);
			exit(1);
		}

		if ((recvSize) == 0){
			FreeBreadCrumbList(breadList);
			fprintf(stdout, "Error: the server terminated prematurely.");
			shutdown(sockfd, 2);
			exit(1);
		}

		if (IS_AM_ERROR(ntohl(serverResponse.type))){
			FreeBreadCrumbList(breadList);
			int message = ntohl(serverResponse.type);
			fprintf(stdout, "Error message received from the server: %d", message);
			shutdown(sockfd, 2);
			exit(1);
		}

		//When a turn message is received
		if((ntohl(serverResponse.type)) == AM_AVATAR_TURN){
			//If it's this avatar's turn
			if (ntohl(serverResponse.avatar_turn.TurnId) == id){
				if (gotMidpoint == 0){ //get the midpoint once
					GetMidpoint(serverResponse);
					fprintf(stdout, "\nMidpoint is %d,%d\n\n", midpoint.x, midpoint.y);
					fprintf(fp, "Midpoint is %d,%d\n", midpoint.x, midpoint.y);
					gotMidpoint = 1;
				}
				//save the avatar's current position to compare after move is attempted
				XYPos currentPosition = serverResponse.avatar_turn.Pos[id];
				int currentX = ntohl(currentPosition.x);
				int currentY = ntohl(currentPosition.y);
				
				// Update mazearray before choosing which direction to go
				if (started == numAvatars){ 
					UpdateMazeArray(prevPos.position, currentPosition, prevPos.direction);
					if ((currentX != ntohl(prevPos.position.x)) || (currentY != ntohl(prevPos.position.y))){
						int returnCode = push(breadList, currentPosition);
						if (returnCode == 1){
							FreeBreadCrumbList(breadList);
							fprintf(stdout, "Error: Memory allocation failure.");
							exit(1);
						}
					}
				}
				else { //initially, we can't update the mazeArray until the avatar has had a chance to move
					started++;
					fprintf(stdout, "Started is equal to %d\n", started);
					int returnCode = push(breadList, currentPosition);
					if (returnCode == 1){
						FreeBreadCrumbList(breadList);
						fprintf(stdout, "Error: Memory allocation failure.");
						exit(1);
					}	
				}

				//print maze before you send response, so next thread does not start printing before this one is done.
				if (id == 0){ //only one avtar prints the maze per turn.
					PrintMaze(serverResponse.avatar_turn.Pos); //x y vals unconverted (print maze at end of each turn)
				}
				//choose a direction to move
				int direction = moveAlgorithm(currentPosition, breadList, serverResponse, id);


				//write the avatar's current position and direction it's moving to the logfile here
				turn = turn + 1;
				fprintf(fp, "\nTurn: %d\nAvatar %d is at %d,%d attempting to move: %d\n", turn, id, currentX, currentY, direction);
				//fprintf(stdout, "%d\n", turn);
				AM_Message move_signal;
				move_signal.type = htonl(AM_AVATAR_MOVE);
				move_signal.avatar_move.AvatarId = htonl(id);
				move_signal.avatar_move.Direction = htonl(direction);

				//send the move
				if (send(sockfd, &move_signal, sizeof(send_signal), 0) == -1){
					FreeBreadCrumbList(breadList);
					fprintf(stdout, "Error: failed to send message.");
					shutdown(sockfd, 2);
					exit(1);
				}
				//set the current position to previous
				prevPos.position = currentPosition;
				prevPos.direction = direction;
				
			}
			
		}
		//if the maze has been solved
		if(ntohl(serverResponse.type) == AM_MAZE_SOLVED) {
			int message = ntohl(serverResponse.maze_solved.Hash);
			
			++done;
			if (id == numAvatars-1){ //last maze update
				attron(A_BLINK);
				attron(COLOR_PAIR(6));
				mvprintw(height*TILE_H+2,10, "Here is the winning hash: %d\n", message);
				mvprintw(2,width*TILE_W+9,"MAZE SOLVED.");
				attron(COLOR_PAIR(5));
				PrintMaze(serverResponse.avatar_turn.Pos); //x y vals unconverted (print maze at end of each turn)
				sleep(15);
			}
			if (winningHash == 0){
				fprintf(fp, "\nMaze has been solved! Winning Hash: %d\n", message);
				winningHash = 1;
			}
			FreeBreadCrumbList(breadList);
			shutdown(sockfd, 2);
			pthread_exit(NULL);
		}
	}
	return NULL;
}



int moveAlgorithm(XYPos currentPos, breadCrumbList *breadList, AM_Message avTurn, int id){
	int move;
	int x = ntohl(currentPos.x);
	int y = ntohl(currentPos.y);
	Direction location = mazeArray[y][x];

	int north = location.north;
	int east = location.east;
	int south = location.south;
	int west = location.west;
	//calculate distances to midpoint
	int hDistance = (midpoint.x) - x;
	int vDistance = (midpoint.y) - y;
	//if it's at the midpoint, don't move
	if ((hDistance == 0) && (vDistance == 0)){
		move = M_NULL_MOVE;
		return move;
	}

	//Check for a dead end
	if ((north+east+south+west) == 3){
		if (breadList->length > 1){
			breadCrumb *currentCrumb = pop(breadList); //gets rid of the dead end tile
			XYPos currentPos = currentCrumb->position;
			int currentX = ntohl(currentPos.x);
			int currentY = ntohl(currentPos.y);
			
			breadCrumb *prevCrumb = pop(breadList); //the tile to move to
			XYPos prevPos = prevCrumb->position;
			int prevX = ntohl(prevPos.x);
			int prevY = ntohl(prevPos.y);
			int horizontal = prevX - currentX;
			int vertical = prevY - currentY;
			//decide proper move based on knowledge of the maze and distance to midpoint
			if (horizontal > 0 && mazeArray[currentY][currentX].east != 1){
				move = M_EAST;
				mazeArray[prevY][prevX].west = 1;
			}
			else if (horizontal < 0 && mazeArray[currentY][currentX].west != 1){
				move = M_WEST;
				mazeArray[prevY][prevX].east = 1;
			}
			else if (vertical > 0 && mazeArray[currentY][currentX].south != 1){
				move = M_SOUTH;
				mazeArray[prevY][prevX].north = 1;
			}
			else if (vertical < 0 && mazeArray[currentY][currentX].north != 1){
				move = M_NORTH;
				mazeArray[prevY][prevX].south = 1;
			}
			else if (mazeArray[currentY][currentX].east != 1){
				move = M_EAST;
				mazeArray[prevY][prevX].west = 1;
			}
			else if (mazeArray[currentY][currentX].west != 1){
				move = M_WEST;
				mazeArray[prevY][prevX].east = 1;
			}
			else if (mazeArray[currentY][currentX].south != 1){
				move = M_SOUTH;
				mazeArray[prevY][prevX].north = 1;
			}
			else if (mazeArray[currentY][currentX].north != 1){
				move = M_NORTH;
				mazeArray[prevY][prevX].south = 1;
			}
			
		} //if the only tile on breadlist is the dead end
		else if (breadList->length == 1){
			breadCrumb *currentCrumb = pop(breadList);
			XYPos currentPos = currentCrumb->position;
			int currentX = ntohl(currentPos.x);
			int currentY = ntohl(currentPos.y);
			if (mazeArray[currentY][currentX].east != 1){
				move = M_EAST;
				mazeArray[currentY][currentX+1].west = 1;
			}
			else if (mazeArray[currentY][currentX].west != 1){
				move = M_WEST;
				mazeArray[currentY][currentX-1].east = 1;
			}
			else if (mazeArray[currentY][currentX].south != 1){
				move = M_SOUTH;
				mazeArray[currentY+1][currentX].north = 1;
			}
			else if (mazeArray[currentY][currentX].north != 1){ 
				move = M_NORTH;
				mazeArray[currentY-1][currentX].south = 1;
			}
		}
		else { //if there are no tiles on the breadlist
			if (mazeArray[y][x].east != 1){
				move = M_EAST;
				mazeArray[y][x+1].west = 1;
			}
			else if (mazeArray[y][x].west != 1){
				move = M_WEST;
				mazeArray[y][x-1].east = 1;
			}
			else if (mazeArray[y][x].south != 1){
				move = M_SOUTH;
				mazeArray[y+1][x].north = 1;
			}
			else if (mazeArray[y][x].north != 1){ 
				move = M_NORTH;
				mazeArray[y-1][x].south = 1;
			}

		}
		return move;
	}
	//if it's not at a dead end or a midpoint, determine the proper move, with a priority
	// to explore over going back to previously explored places
	if (abs(hDistance) >= abs(vDistance)){
		if((hDistance > 0) && (east != 1)){
			move = M_EAST;
		}
		else if((hDistance < 0) && (west != 1)){
			move = M_WEST;
		}
		else if((vDistance > 0) && (south != 1)){
			move = M_SOUTH;
		}
		else if((vDistance < 0) && (north != 1)){
			move = M_NORTH;
		}
		else if((north == -1)){
			move = M_NORTH;
		}
		else if((south == -1)){
			move = M_SOUTH;
		}
		else if((east == -1)){
			move = M_EAST;
		}
		else if((west == -1)){
			move = M_WEST;
		}
		else if((north == 0)){
			move = M_NORTH;
		}
		else if((south == 0)){
			move = M_SOUTH;
		}
		else if((east == 0)){
			move = M_EAST;
		}
		else if((west == 0)){
			move = M_WEST;
		}
		else {
			move = M_NULL_MOVE;
		}

	}
	else {
		if((vDistance > 0) && (south != 1)){
			move = M_SOUTH;
		}
		else if((vDistance < 0) && (north != 1)){
			move = M_NORTH;
		}
		else if((hDistance > 0) && (east != 1)){
			move = M_EAST;
		}
		else if((hDistance < 0) && (west != 1)){
			move = M_WEST;
		}
		else if((east == -1)){
			move = M_EAST;
		}
		else if((west == -1)){
			move = M_WEST;
		}
		else if((south == -1)){
			move = M_SOUTH;
		}
		else if((north == -1)){
			move = M_NORTH;
		}
		else if((east == 0)){
			move = M_EAST;
		}
		else if((west == 0)){
			move = M_WEST;
		}
		else if((south == 0)){
			move = M_SOUTH;
		}
		else if((north == 0)){
			move = M_NORTH;
		}
		else {
			move = M_NULL_MOVE;
		}

	}
	XYPos test;
	int westTried = 0;
	int northTried = 0;
	int southTried = 0;
	int eastTried = 0;
	//if this move is telling you to go somewhere you have gone before, dont go there
	while(1) {

		if ((move == M_WEST) && (westTried == 0)){ //west
			westTried = 1;
			test.x = x -1; //next x
			test.y = y;  // current y
			if (SeenBreadCrumb(breadList,test)){
				int badDirection = M_WEST;
				move = dontGoBack(vDistance, hDistance, north, east, south, west, badDirection);
			}
			else {
				break;
			}
		}
		else if ((move == M_NORTH) && (northTried == 0)){ //north
			northTried = 1;
			test.x = x; //current x
			test.y = y -1; // next y
			if (SeenBreadCrumb(breadList,test)){
				int badDirection = M_NORTH;
				move = dontGoBack(vDistance, hDistance, north, east, south, west, badDirection);
			}
			else {
				break;
			}
		}
		else if ((move == M_SOUTH) && (southTried == 0)){ //south
			southTried = 1;
			test.x = x; //current x
			test.y = y +1;// next y
			if (SeenBreadCrumb(breadList,test)){
				int badDirection = M_SOUTH;
				move = dontGoBack(vDistance, hDistance, north, east, south, west, badDirection);
			}
			else {
				break;
			}
		}
		else if ((move == M_EAST) && (eastTried == 0)){ //east
			eastTried = 1;
			test.x = x +1; //next x
			test.y = y; // current y
			if (SeenBreadCrumb(breadList,test)){
				int badDirection = M_EAST;
				move = dontGoBack(vDistance, hDistance, north, east, south, west, badDirection);
			}
			else {
				break;
			}
		}
		else { //if the move was to the tile that it just came from, pick an appropriate different move
			if ((westTried == 0) && (west == -1)){
				move = M_WEST;
				break;
			}
			else if ((northTried == 0) && (north == -1)){
				move = M_NORTH;
				break;
			}
			else if ((southTried == 0) && (south == -1)){
				move = M_SOUTH;
				break;
			}
			else if ((eastTried == 0) && (east == -1)){
				move = M_EAST;
				break;
			}
			else if (west == -1){
				move = M_WEST;
				break;
			}
			else if (north == -1){
				move = M_NORTH;
				break;
			}
			else if (south == -1){
				move = M_SOUTH;
				break;
			}
			else if (east == -1){
				move = M_EAST;
				break;
			}
			else if ((westTried == 0) && (west == 0)){
				move = M_WEST;
				break;
			}
			else if ((northTried == 0) && (north == 0)){
				move = M_NORTH;
				break;
			}
			else if ((southTried == 0) && (south == 0)){
				move = M_SOUTH;
				break;
			}
			else if ((eastTried == 0) && (east == 0)){
				move = M_EAST;
				break;
			}
			else if (west == 0){
				move = M_WEST;
				break;
			}
			else if (north == 0){
				move = M_NORTH;
				break;
			}
			else if (south == 0){
				move = M_SOUTH;
				break;
			}
			else if (east == 0){
				move = M_EAST;
				break;
			}
		}
	}

	return move;
}


int dontGoBack(int vDistance, int hDistance, int north, int east, int south, int west, int badDirection){
	int move;
	//pick an appropriate move, trying not to move back in the direction from which it just came
	//in order to avoid oscillating avatars in the maze
	if (abs(hDistance) >= abs(vDistance)){
		if((hDistance > 0) && (east != 1) && (badDirection != 3)){
			move = M_EAST;
		}
		else if((hDistance < 0) && (west != 1) && (badDirection != 0)){
			move = M_WEST;
		}
		else if((vDistance > 0) && (south != 1) && (badDirection != 2)){
			move = M_SOUTH;
		}
		else if((vDistance < 0) && (north != 1) && (badDirection != 1)){
			move = M_NORTH;
		}
		else if((north == -1) && (badDirection != 1)){
			move = M_NORTH;
		}
		else if((south == -1) && (badDirection != 2)){
			move = M_SOUTH;
		}
		else if((east == -1) && (badDirection != 3)){
			move = M_EAST;
		}
		else if((west == -1) && (badDirection != 0)){
			move = M_WEST;
		}
		else if((north == 0) && (badDirection != 1)){
			move = M_NORTH;
		}
		else if((south == 0) && (badDirection != 2)){
			move = M_SOUTH;
		}
		else if((east == 0) && (badDirection != 3)){
			move = M_EAST;
		}
		else if((west == 0) && (badDirection != 0)){
			move = M_WEST;
		}
		else {
			move = M_NULL_MOVE;
		}

	}
	else {
		if((vDistance > 0) && (south != 1) && (badDirection != 2)){
			move = M_SOUTH;
		}
		else if((vDistance < 0) && (north != 1) && (badDirection != 1)){
			move = M_NORTH;
		}
		else if((hDistance > 0) && (east != 1) && (badDirection != 3)){
			move = M_EAST;
		}
		else if((hDistance < 0) && (west != 1) && (badDirection != 0)){
			move = M_WEST;
		}
		else if((east == -1) && (badDirection != 3)){
			move = M_EAST;
		}
		else if((west == -1) && (badDirection != 0)){
			move = M_WEST;
		}
		else if((south == -1) && (badDirection != 2)){
			move = M_SOUTH;
		}
		else if((north == -1) && (badDirection != 1)){
			move = M_NORTH;
		}
		else if((east == 0) && (badDirection != 3)){
			move = M_EAST;
		}
		else if((west == 0) && (badDirection != 0)){
			move = M_WEST;
		}
		else if((south == 0) && (badDirection != 2)){
			move = M_SOUTH;
		}
		else if((north == 0) && (badDirection != 1)){
			move = M_NORTH;
		}
		else {
			move = M_NULL_MOVE;
		}

	}
	return move;
}


int InitializeMazeArray(int width, int height) {
	mazeArray= (struct Direction**)calloc(height,sizeof(struct Direction*));
	if (mazeArray == NULL){
		return 1;
	}
	for (int i=0; i<height; i++) {
		mazeArray[i] = (struct Direction*)calloc(width+1,sizeof(struct Direction)); 
		if (mazeArray[i] == NULL){
			return 1;
		}
	}
	//set all spots to 0 initially
	for (int i=0; i<height; i++){
		for (int j=0; j<width; j++){
			mazeArray[i][j].north = -1;
			mazeArray[i][j].east = -1;
			mazeArray[i][j].south = -1;
			mazeArray[i][j].west = -1;
		}
	}

	// draw boundary walls for the maze
	for (int left = 0; left < height; left++) {
		mazeArray[left][0].west = 1;
	}
	for (int top = 0; top < width; top++) {
		mazeArray[0][top].north = 1;
	}
	for (int right = 0; right < height; right++) {
		mazeArray[right][width-1].east = 1;
	}
	for (int bottom = 0; bottom < width; bottom++) {
		mazeArray[height-1][bottom].south = 1;
	}

	return 0;

}


void UpdateMazeArray(XYPos prev, XYPos curr, int moveDirection){
	int prevX = ntohl(prev.x);
	int prevY = ntohl(prev.y);
	int currX = ntohl(curr.x);
	int currY = ntohl(curr.y);
	//Check to see if this move has already been added to the mazeArray
	//This also serves the purpose of not deleting walls that were created artificially at dead ends
	int move;
	if ((prevX == currX) && (prevY == currY)){
		
		if (moveDirection == 0){
			move = (mazeArray[prevY][prevX].west);
		}
		else if (moveDirection == 1){
			move = (mazeArray[prevY][prevX].north);	
		}
		else if (moveDirection == 2){
			move = (mazeArray[prevY][prevX].south);
		}
		else if (moveDirection == 3){
			move = (mazeArray[prevY][prevX].east);
		}
	}
	else {

		if (moveDirection == 0){
			move = (mazeArray[prevY][prevX].west);
		}
		else if (moveDirection == 1){
			move = (mazeArray[prevY][prevX].north);
		}
		else if (moveDirection == 2){
			move = (mazeArray[prevY][prevX].south);
		}
		else if (moveDirection == 3){
			move = (mazeArray[prevY][prevX].east);
		}
	}
	if (move != -1){
		return;
	}

	//move unsuccessful, means wall
	if ((prevX == currX) && (prevY == currY)) {
		//add wall to west of avatar
		if (moveDirection == 0) {
			mazeArray[prevY][prevX].west = 1;
			mazeArray[prevY][prevX - 1].east = 1; 
		}
		//add wall to north of avatar
		else if (moveDirection == 1) {
			mazeArray[prevY][prevX].north = 1;
			mazeArray[prevY - 1][prevX].south = 1;
		}
		//add wall to south of avatar
		else if (moveDirection == 2) {
			mazeArray[prevY][prevX].south = 1;
			mazeArray[prevY + 1][prevX].north = 1;
		}
		//add wall to east of avatar
		else if (moveDirection == 3) {
			mazeArray[prevY][prevX].east = 1;
			mazeArray[prevY][prevX + 1].west = 1;
		}
		return;
	}

	//otherwise, move was successful, means no wall
	else {
		//no wall to west of avatar
		if (moveDirection == 0) {
			mazeArray[prevY][prevX].west = 0;
			mazeArray[prevY][prevX - 1].east = 0;
		}
		//no wall to north of avatar
		else if (moveDirection == 1) {
			mazeArray[prevY][prevX].north = 0;
			mazeArray[prevY - 1][prevX].south = 0;
		}
		//no wall to south of avatar
		else if (moveDirection == 2) {
			mazeArray[prevY][prevX].south = 0;
			mazeArray[prevY + 1][prevX].north = 0;
		}
		//no wall to east of avatar
		else if (moveDirection == 3) {
			mazeArray[prevY][prevX].east = 0;
			mazeArray[prevY][prevX + 1].west = 0;
		}
		return;
	}
	return;
}

void FreeMazeArray(int width, int height){
	
	for (int i=0; i<height; i++) {
		free(mazeArray[i]);
	}
	free(mazeArray);
}


breadCrumbList* InitializeBreadList() {
	breadCrumbList *breadList= (struct breadCrumbList*)malloc(sizeof(breadCrumbList));
	breadList->head = NULL;
	breadList->tail = NULL;
	breadList->length = 0;
	return breadList;
}


int push(breadCrumbList *breadList, XYPos position){
	//properly allocate memory for a new 'crumb'
	breadCrumb *crumb = (struct breadCrumb*)malloc(sizeof(breadCrumb));
	if (crumb == NULL){
		return 1;
	}
	//if this is the first crumb in the list, make it the head and tail
	if ((breadList->head == NULL) && (breadList->tail == NULL)) {
		breadList->head = crumb;
		breadList->tail = crumb;
		crumb->next = NULL;
		crumb->prev = NULL;
		crumb->position = position;

	}
	//make it the new tail
	else {
		breadList->tail->next = crumb;
		crumb->prev = breadList->tail;
		crumb->next = NULL;
		breadList->tail = crumb;
		crumb->position = position;

	}
	//increment the length of the list
	(breadList->length)++;
	return 0;

}


breadCrumb* pop(breadCrumbList *breadList){
	breadCrumb *temp;
	//if there's more than just the head in the list
	if (breadList->length > 1){
		temp = breadList->tail;
		breadList->tail = breadList->tail->prev;
		breadList->tail->next = NULL;
	}
	//if only one crumb remains
	else {
		temp = breadList->tail;
		breadList->head = NULL;
		breadList->tail = NULL;	
	}
	//decrement the length of the list
	(breadList->length)--;
	return temp;

}

int SeenBreadCrumb(breadCrumbList *breadList, XYPos position)
{ 
	breadCrumb * temp_ptr;
	breadCrumb * ptr = breadList->head;
	//go through all of the crumbs in the list to check if has been there already
	while (ptr != NULL){
		temp_ptr = ptr->next;

		if ((ntohl(ptr->position.x) == (position.x)) && (ntohl(ptr->position.y) == (position.y))){
			return 1;
		}
		ptr = temp_ptr;
	}
	return 0;
}

/*Uncomment the lines in this function to print out the lists of each
of the avatars while solving the maze - mainly for testing purposes*/
void printbreadList(breadCrumbList* list){
	breadCrumb *breadptr = list->head;

	if (breadptr == NULL){

		// fprintf(stdout, "The list is empty.\n");
	}

	else {
		// fprintf(stdout, "Here is the list: ");
		while (breadptr != NULL){
			// int x = ntohl(breadptr->position.x);
			// int y = ntohl(breadptr->position.y);
			// fprintf(stdout, "'%d,%d' ", x, y);
			breadptr = breadptr->next;
		}
		fprintf(stdout,"\n");
	}

}

void FreeBreadCrumbList(breadCrumbList *list){
	breadCrumb *current= list->head;
	//if the list has no crumbs, just free the list
	if (list->head == NULL){
		free(list);
	} 
	else {
		//while the list has crumbs, free them, and then free the whole list
		while (current != NULL){
			breadCrumb *next = current->next;
			free(current);
			current = next;
		}
		free(list);
	}

	//fprintf(stdout, "Freed BreadCrumb List\n");
	fflush(stdout);
}



void GetMidpoint(AM_Message avTurn){
	int averageX = 0;
	int averageY = 0;
	int currentX, currentY;

	//sum all X and Y coordinates for each avatar
	for (int i = 0; i < numAvatars; i++){
		currentX = ntohl(avTurn.avatar_turn.Pos[i].x); 
		currentY = ntohl(avTurn.avatar_turn.Pos[i].y);
		averageX = averageX + currentX;
		averageY = averageY + currentY;
	}

	//ensure that average is stored as an integer
	averageX = (averageX / numAvatars);
	averageY = (averageY / numAvatars);

	//initialize and fill midpoint XYPos struct
	midpoint.x = averageX;
	midpoint.y = averageY;

	return;
}


void PrintMaze(XYPos * avatarPositions)
{
  char* middle = malloc(1000);
  int color_id = -1;
  int i;
  int j;

  for (int i = 0; i < height ; i++){ //for every row of tiles
    if (1){// used for debugging
      if ((i==0)){ //draw numers on top
        for (int j = 0 ; j<width;j++){ // top row of x coordinates
          mvprintw(i*TILE_H,j*TILE_W,"%8d",j);
          if (j == width - 1){
          	attron(COLOR_PAIR(6));
          	attron(A_BOLD);
          	mvprintw(i*TILE_H,j*TILE_W+15,"TEAM NAME: JOUBERTIN");
          	mvprintw(i*TILE_H+1,j*TILE_W+15,"TURN NUMBER: %d",turn);
          	attron(A_NORMAL);
          	attron(COLOR_PAIR(5));
          }
        }
        attron(COLOR_PAIR(5));
      }

      for (j = 0 ; j<width;j++){ //top row of each tile
        if ( (mazeArray[i][j].west > 0) && (mazeArray[i][j].north > 0) && (mazeArray[i][j].east > 0) ){
          mvprintw(i*TILE_H+1,j*TILE_W+3,"|------|");
        }
        if ( (mazeArray[i][j].west > 0) && (mazeArray[i][j].north > 0) && (mazeArray[i][j].east <= 0) ){
          mvprintw(i*TILE_H+1,j*TILE_W+3,"|------ ");
        }
        if ( (mazeArray[i][j].west <= 0) && (mazeArray[i][j].north > 0) && (mazeArray[i][j].east > 0) ){
          mvprintw(i*TILE_H+1,j*TILE_W+3," ------|");
        }
        if ((mazeArray[i][j].west <= 0) && (mazeArray[i][j].north > 0) && (mazeArray[i][j].east <= 0)){
          mvprintw(i*TILE_H+1,j*TILE_W+3," ------ ");
        }
        if ((mazeArray[i][j].west > 0) && (mazeArray[i][j].north <= 0) && (mazeArray[i][j].east <= 0)){
          mvprintw(i*TILE_H+1,j*TILE_W+3,"|       ");
        }
        if ((mazeArray[i][j].west <= 0) && (mazeArray[i][j].north <= 0) && (mazeArray[i][j].east > 0)){
          mvprintw(i*TILE_H+1,j*TILE_W+3,"       |");
        }
        if ((mazeArray[i][j].west <= 0) && (mazeArray[i][j].north <= 0) && (mazeArray[i][j].east <= 0)){
          mvprintw(i*TILE_H+1,j*TILE_W+3,"        ");
        }
        if ((mazeArray[i][j].west > 0) && (mazeArray[i][j].north <= 0) && (mazeArray[i][j].east > 0)){
          mvprintw(i*TILE_H+1,j*TILE_W+3,"|      |");
        }
      }

      for (j = 0 ; j<width;j++){ //middle row of each tile
        if (j == 0){
          mvprintw(i*TILE_H+2,j*TILE_W,"%3d",i);
        }
        sprintf(middle," ");
        for (int k = 0; k <numAvatars;k++){
          XYPos avatarPosition = avatarPositions[k];
          int avX = ntohl(avatarPosition.x);
          int avY = ntohl(avatarPosition.y);
          if ((i == midpoint.y) && (j == midpoint.x)){
          	attron(COLOR_PAIR(4));
          	sprintf(middle,"Midpoint");
          }
          else if ((i == avY) && (j == avX)){
            color_id = k%3 + 1;
            sprintf(middle,"av%d",k);
            attron(COLOR_PAIR(color_id)); 
          }
        }
        if ((mazeArray[i][j].east > 0) && (mazeArray[i][j].west >0)){
          mvprintw(i*TILE_H+2,j*TILE_W+4,"%6s",middle);
          attron(COLOR_PAIR(5));
          mvprintw(i*TILE_H+2,j*TILE_W+3,"|");
          mvprintw(i*TILE_H+2,j*TILE_W+10,"|");
        }
        if ((mazeArray[i][j].east <= 0) && (mazeArray[i][j].west > 0)){

          mvprintw(i*TILE_H+2,j*TILE_W+4,"%6s",middle);
          attron(COLOR_PAIR(5));
          mvprintw(i*TILE_H+2,j*TILE_W+3, "|",middle);
        }
        if ((mazeArray[i][j].east > 0) && (mazeArray[i][j].west <= 0)){
          mvprintw(i*TILE_H+2,j*TILE_W+4,"%6s",middle);
          attron(COLOR_PAIR(5));
          mvprintw(i*TILE_H+2,j*TILE_W+10,"|");
        }
        if ((mazeArray[i][j].east <= 0) && (mazeArray[i][j].west <= 0)){
          mvprintw(i*TILE_H+2,j*TILE_W+4, "%6s" ,middle);
          attron(COLOR_PAIR(5));
        }
        if (j == width-1){
          mvprintw(i*TILE_H+2,(j+1)*TILE_W+3,"%d",i);
        }
        attron(COLOR_PAIR(5));
      }

      for (j = 0 ; j<width;j++){ //bottom row of each tile
        if ( (mazeArray[i][j].west > 0) && (mazeArray[i][j].south > 0) && (mazeArray[i][j].east > 0) ){
          mvprintw(i*TILE_H+3,j*TILE_W+3,"|------|");
        }
        if ( (mazeArray[i][j].west > 0) && (mazeArray[i][j].south > 0) && (mazeArray[i][j].east <= 0) ){
          mvprintw(i*TILE_H+3,j*TILE_W+3,"|------ ");
        }
        if ( (mazeArray[i][j].west <= 0) && (mazeArray[i][j].south > 0) && (mazeArray[i][j].east > 0) ){
          mvprintw(i*TILE_H+3,j*TILE_W+3," ------|");
        }
        if ((mazeArray[i][j].west <= 0) && (mazeArray[i][j].south > 0) && (mazeArray[i][j].east <= 0)){
          mvprintw(i*TILE_H+3,j*TILE_W+3," ------ ");
        }
        if ((mazeArray[i][j].west > 0) && (mazeArray[i][j].south <= 0) && (mazeArray[i][j].east <= 0)){
          mvprintw(i*TILE_H+3,j*TILE_W+3,"|       ");
        }
        if ((mazeArray[i][j].west <= 0) && (mazeArray[i][j].south <= 0) && (mazeArray[i][j].east > 0)){
          mvprintw(i*TILE_H+3,j*TILE_W+3,"       |");
        }
        if ((mazeArray[i][j].west <= 0) && (mazeArray[i][j].south <= 0) && (mazeArray[i][j].east <= 0)){
          mvprintw(i*TILE_H+3,j*TILE_W+3,"        ");
        }
        if ((mazeArray[i][j].west > 0) && (mazeArray[i][j].south <= 0) && (mazeArray[i][j].east > 0)){
          mvprintw(i*TILE_H+3,j*TILE_W+3,"|      |");
        }
      }
      if ((i==height-1)){
        for (j = 0 ; j<width;j++){// y coordinates on right side
          mvprintw((i+1)*TILE_H+1,j*TILE_W,"%8d",j);
        }
      }
    }

  }
  if (0){ //debugging
    for (i = 0; i < height ; i++){
      if (i==0){
        for (j = 0 ; j<width;j++){
          mvprintw(i*TILE_H,j*TILE_W,"%15d",j);
        }
      } 
      for (j = 0 ; j<width;j++){
        if (j == 0){
          mvprintw(i*TILE_H,j*TILE_W,"%3d ",i);
        }
        mvprintw(i*TILE_H,j*TILE_W,"%2d,%2d,%2d,%2d    ", mazeArray[i][j].north,mazeArray[i][j].east,mazeArray[i][j].south,mazeArray[i][j].west);
      }
    }
  }
  free(middle);
  refresh();
}





