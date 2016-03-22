
/* ========================================================================== */
/* File: AMStartup.c
 *
 * Authors: Andrew Ogren, Amos Cariati, James Thompson III

 * Date: 02/27/16
 *
 * Input: 
 * There are several parameters that AMStartup.c takes on the command line:
 *
 * 1.) [nAvatars] : This is the number of avatars that will be in the maze. The number of
 * avatars must be greater than 0 but less than or equal to 10.
 *
 * 2.) [Difficulty] : This is the level of difficulty for the maze. The difficulty must be 
 * greater than or equal to 0 but less than 10.
 *
 * 3.) [Hostname] : The hostname of the server. For this particular project, this server
 * will be "flume.cs.dartmouth.edu"
 *
 * Command line options: None    
 *
 * Output: Creates avatar.c process
 *
 * Error Conditions: 
 * -Memory allocation failures
 * -connection failures
 * -failure to receive or send message
 * -improper arguments
 *
 * Pseudocode: Check design_spec.pdf
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"
// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes
int getIP(char *, char *); 

// ---------------- Public functions


int main(int argc, char **argv){
	//General parameter checks

	if (argc != 4){
		fprintf(stdout, "Error: Incorrect number of paramters.");
		exit(1);
	}
	int numAvatars= atoi(argv[1]);

	if ((numAvatars < 1) || (numAvatars > 10)){
		fprintf(stdout, "Error: Number of avatars must be from 1 to 10.");
		exit(1);
	}
	int difficulty= atoi(argv[2]);
	if ((difficulty < 0) || (difficulty > 9)){
		fprintf(stdout, "Error: Difficulty must be from 0 to 9.");
		exit(1);
	}

	char *hostName= argv[3];
	if (strcmp(hostName, "flume.cs.dartmouth.edu") != 0){
		fprintf(stdout, "Error: hostname must be flume.cs.dartmouth.edu");
		exit(1);
	}

	//Get the ip address of the hostname
	char ip[200]; //ip definitely won't be longer than 200 characters
	getIP(hostName, ip);
	fprintf(stdout, "Host %s resolved to %s\n", hostName, ip);


	//Start the log file now
	char *userName = getlogin();
	char *fileLog = (char*)malloc(sizeof(char)*(strlen("Amazing_")+strlen(userName)+ \
	strlen(argv[1])+strlen(argv[2])+strlen("__.log")+1));
	if (fileLog == NULL){
		fprintf(stdout, "Error: Memory allocation error.");
		exit(1);
	}
	sprintf(fileLog, "Amazing_%s_%s_%s.log", userName, argv[1], argv[2]); 
	
	/* Adapted from echoClient.c example */
	int sockfd;
	struct sockaddr_in servaddr;
	AM_Message *send_signal = (struct AM_Message*)malloc(sizeof(AM_Message));
	AM_Message *receive_signal = (struct AM_Message*)malloc(sizeof(AM_Message));

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
	servaddr.sin_port =  htons(atoi(AM_SERVER_PORT)); //convert to big-endian order

	//Connection of the client to the socket 
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
		perror("Problem in connecting to the server");
		exit(3);
	}
	printf("Connection Succesful.\n"); // If we got here, connection didnt fail


	// Create AM_INIT signal
	send_signal->type = htonl(AM_INIT);
	send_signal->init.nAvatars = htonl(numAvatars);
	send_signal->init.Difficulty = htonl(difficulty);

	
	// Send initial signal to server
	send(sockfd, send_signal, sizeof(*send_signal), 0); 

	// Receive from server
	if (recv(sockfd, receive_signal, sizeof(*receive_signal),0) == 0){
		//error: server terminated prematurely
		perror("The server terminated prematurely"); 
		exit(4);
	}


	if (IS_AM_ERROR(ntohl(receive_signal->type))){
			printf("Error on return signal.\n");
			exit(1);
	}


	printf("REC TYPE:%d\n",ntohl(receive_signal->type));
	if(receive_signal->type == AM_INIT_FAILED){
		printf("Received AM_INIT_OK\n\n");
	}

	int mazePort = ntohl(receive_signal->init_ok.MazePort);
	int mazeWidth = ntohl(receive_signal->init_ok.MazeWidth);
	int mazeHeight = ntohl(receive_signal->init_ok.MazeHeight);

	printf("INIT_OK SIGNAL RECEIVED:\n");
	printf("MazePort: %d\n", mazePort);
	printf("MazeWidth: %d\n", mazeWidth);
	printf("MazeHeight: %d\n", mazeHeight);

	//Start the avatar program with appropriate arguments
	//Need to add memory allocation failure errors for the following
	//and free as appropriate
	int numWidthInts= snprintf(NULL, 0, "%d", mazeWidth);
	char *widthString = (char*)malloc(numWidthInts+1);
	sprintf(widthString, "%d", mazeWidth);

	int numHeightInts= snprintf(NULL, 0, "%d", mazeHeight);
	char *heightString = (char*)malloc(numHeightInts+1);
	sprintf(heightString, "%d", mazeHeight);
	
	int numInts= snprintf(NULL, 0, "%d", mazePort);
	char *mazePortString= (char*)malloc(numInts+1);
	sprintf(mazePortString, "%d", mazePort);


	execl("./avatar", "./avatar", argv[1], argv[2], ip, mazePortString, fileLog, widthString, heightString, NULL);
	perror("execl() failure!\n\n");

	//May have to move the location of this stuff
	shutdown(sockfd, 2);
	free(send_signal);
	free(receive_signal);

	free(widthString);
	free(heightString);
	free(mazePortString);
	free(fileLog);

	exit(0);
}


/* Adapted from example on https://srishcomp.wordpress.com */
int getIP(char *hostName, char *ip) {
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
	if ( (he = gethostbyname(hostName)) == NULL) {
		fprintf(stdout, "Error: Could not resolve hostname");
		return 1;
	}

	addr_list = (struct in_addr **) he->h_addr_list;
	
	for(i=0; addr_list[i] != NULL; i++){
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}
	if (ip == NULL){
		return 1;
	}

	return 0;		
}

