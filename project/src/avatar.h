/* ========================================================================== */
/* File: avatar.h
 *
 * Project name: Amazing Final Project
 * Component name: avatar
 *	
 * Primary Authors: Andrew Ogren, Amos Cariati, James Thompson III
 * Date Created:	March 6th, 2016
 *
 * This file contains the main data structues and functions used by the avatars.
 *
 */
/* ========================================================================== */

// ---------------- Prerequisites

#include "amazing.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

#ifndef AVATAR_H
#define AVATAR_H

typedef struct Direction {		//stores the drection (North, East, South, West) 
								//that a move was attempted
	int north;
	int east;
	int south;
	int west;

} Direction;

typedef struct breadCrumbList {	//initializes linked list that stores breadCrumb structs
	
	struct breadCrumb *head;		//first breadCrumb in list
	struct breadCrumb *tail;		//last in list
	int length;						//number of breadCrumbs in the list
	
} breadCrumbList;


typedef struct breadCrumb {

	struct breadCrumb *next;		//points to next breadCrumb struct in linked list
	struct breadCrumb *prev;		//points to previous
	struct XYPos position;			//stores the breadCrumb's position in the maze as an XYPos struct

} breadCrumb;

typedef struct midPoint {		//struct that contains the x and y coordnates of the avatar's midpoint

	int x;
	int y;

} midPoint;

typedef struct prevPosition {	//contains the previous position and move direction 

	struct XYPos position;
	int direction;

} prevPosition;


typedef struct argStruct {		//struct used to pass information from 'AMStartup' to 'avatar'

	int width;						//maze width
	int height;						//maze height
	int id;							//identification integer							
	int mazePort;					//server port for the maze
	char *ip;						//IP address
	char *fileLog;					//output log file name

} argStruct;

/* InitializeMazeArray - allocates memory for the mazeArray of given width and hieght
 *
 * @width - width of the mazeArray
 * @height - height of the mazeArray
 *
 * Assumptions - Input parameters are integer values
 * 
 * Pseudo Code:
 * Takes the fed width and height parameters
 * Allocates memory for the mazeArray by looping through the width and height
 * Creates a Direction struct for each slot looped through
 * Each direction in the slot is set to -1 (undiscovered)
 * Once all slots have been initialized, outer edge slots are looped through
 * Boundary directions in the outer slots are set to 1 (wall present)
 * Returns 1 if there is an error, otherwise returns 0
 */
int InitializeMazeArray(int width, int height);


/* UpdateMazeArray - updates direction in Direction struct for a slot to indicate whether a wall is present
 *
 * @prev - avatar's previous x,y coordinate position
 * @curr - avatar's current x,y coordindate position
 * @moveDirection - direction in which an attempted move was made
 *
 * Assumptions - Avatar's current and previous positions are fed as XYPos structs and their was an attempted move in the 
 *	previous turn (moveDirection is not 8 - avatar opted not to move in the previous turn)
 * 
 * Pseudo Code:
 * Check if move has been already logged into the mazeArray
 * If not, check if the prev and curr position coordinates are the same
 * If positions are the same, there is a wall impeding their movement
 * Update the Direction struct for the moveDirection direction and draw a wall by changing from -1 (undetermined) to 1 (wall)
 * Shift to adjacent Direction slot and draw the second side of the same wall 
 * Else, if prev and curr are different, no wall exists
 * Update the Direction struct for the moveDirection direction and update no-wall by changing from -1 to 0 (no wall) 
 * Shift to adjacent Direction slot and update the second side of the no-wall
 */
void UpdateMazeArray(XYPos prev, XYPos curr, int moveDirection);


/* FreeMazeArray - frees memory allocated for the Direction structs in the mazeArray
 *
 * @width - width of the mazeArray
 * @height - height of the mazeArray
 *
 * Assumptions - Inout parameters are both integer values indicating the dimensions of the mazeArray
 * 
 * Pseudo Code:
 * Use passed height parameter to loop through each Direction struct of the mazeArray
 * Free each struct
 * After looping through, free the mazeArray itself
 */
void FreeMazeArray(int width, int height);


/* PrintMaze - draws the known walls present in the mazeArray and the positions of each avatar in the maze
 *
 * @avatarPositions - the x,y coordinates of each avatar in the mazeArray
 *
 * Assumptions - the avatar coordinates are passed as an XYPos struct
 * 
 * Pseudo Code:
 * Loop through each Direction struct in the mazeArray
 * If a direction in the struct has a wall (equals 1), print it to the standard output
 * Inside the above loop, loop through the passed avatar XYPos positions
 * If an avatar is present in the current Direction struct location, print their name to the standard output in that location
 */
void PrintMaze(XYPos * avatarPositions);


/* InitializeBreadList - allocate memory for and intializes a breadCrumbList struct
 *
 * Assumptions - returns a breadCrumbList, must be stored properly so memory is not lost
 * 
 * Pseudo Code:
 * Allocates memory for a breadCrumbList
 * Sets the head and tail pointers to NULL
 * Sets the length index to 0
 * Returns the newly allocated list
 */
breadCrumbList* InitializeBreadList();


/* push - allocates memory for and adds a new breadCrumb struct to the passed breadCrumbList
 *
 * @breadList - pointer to the breadList to be added to
 * @position - position of the breadCrumb struct to be added
 *
 * Assumptions - breadList must exist and position must be stored as an XYPos struct
 * 
 * Pseudo Code:
 * Allocates memory for a new breadCrumb
 * Adds the coordinates in the XYPos struct to the breadCrumb
 * Sets the breadCrumb's next pointer to NULL
 * Adds the breadCrumb to the end of the passed breadList linked list
 * Increments the breadCrumbList length counter by 1
 * Returns 1 if error allocating memory, otherwise returns 0
 */
int push(breadCrumbList *breadList, XYPos position);


/* pop - returns the last breadCrumb in the passed breadCrumbList
 *
 * @breadList - pointer to the breadList to be popped from
 *
 * Assumptions - breadList must exist and contain at at least a single breadCrumb
 * 
 * Pseudo Code:
 * Stores the last breadCrumb struct in the linked list in a temporary pointer
 * Removes the breadCrumb from the passed breadList and sets the previous crumb's next pointer to NULL
 * Decreases the breadList's length index by 1
 * Returns the popped breadCrumb
 */
breadCrumb* pop(breadCrumbList *breadList);


/* SeenBreadCrumb - checks if a breadCrumb exists in the passed breadList for the passed coordinates
 *
 * @breadList - pointer to the breadList to be checked
 * @position - position in mazeArray to be checked for
 *
 * Assumptions - breadList must exist and position must be fed as an XYPos struct
 * 
 * Pseudo Code:
 * Intialize two breadCrumb pointers and set one to the head of the passed breadList
 * Loop through the breadCrumb structs in the linked list one by one
 * Check if the passed XYPos coordinates are stored in an existing breadCrumb in the list
 * If a matching breadCrumb is found, return 1, otherwise return 0
 */
int SeenBreadCrumb(breadCrumbList *breadList, XYPos position);


/* printbreadList - used for testing, prints the list of breadCrumb coordinates for the passed breadCrumbList
 *
 * @list - pointer to the breadList to be printed
 *
 * Assumptions - breadList must exist
 * 
 * Pseudo Code:
 * Loops through each breadCrumb struct contained within the passed breadCrumbList
 * Starts at the head of the linked list and prints the x and y positions of each breadCrumb in the list
 * If the list is empty, prints "The list is empty"
 */
void printbreadList(breadCrumbList* list);


/* FreeBreadCrumbList - frees the memory allocated for the breadCrumb structs contained within the list and the list itself
 *
 * @list - pointer to the breadList struct to be freed
 *
 * Assumptions - breadList must exist
 * 
 * Pseudo Code:
 * Set breadCrumb pointer to the head of the passed breadList
 * If head is NULL, free the list
 * Otherwise, loop through every breadCrumb in the linked list freeing each breadCrumb as it goes
 * Once the end of the linked list is reached, free the breadCrumbList struct itself
 */
void FreeBreadCrumbList(breadCrumbList *list);

/* avatarRun -main operating code for each avatar thread
 *
 * @arguments - argStruct struct containing the information regarding themaze's width, height, the 
 	avatar's id, the server's mazeoprt number, IP address and the name of the output filelog
 *
 * Assumptions - argStruct has the appropriate contents in it before it is passed as the only parameter
 * 
 * Pseudo Code:
 * Intializes all the variables contained within the passed argStruct
 * Initializes a breadCrumbList struct for the current avatar
 * Ensures that the avatar can communicate properly with the server and sets up permanent communications
 * Enters an endless while-loop
 * Sets the midpoint of the avatars if this has not been done already by calling getMidpoint
 * Checks that the mazeArray is properly updated before calling moveAlgorithim to determine the optimal move direction
 * Avatar then contancts the server using an AM_AVATAR_MOVE message and requests to be moved int he designated direction
 * The server responds with a potentiall updated AM_AVATAR_TURN message and the mazeArray is updated accordingly
 * The continues until an error occurs and the program exits, the maximum number of moves is reached, or an AM_SOLVED_MAZE message is recieved
 * If the latter occurs, if the avatar is the first to recieve the message, it writes the winning hash number to the output log file
 * Then all avatar proceed to free memory allocated for their breadList structs
 * The code returns control to 'avatar.c' main
 */
void* avatarRun(void* arguments);


/* avatarInSpot - checks if any other avatars are currently in the passed coordinates
 *
 * @avTurn - AM_Message containing the current locations of all avatars
 * @spot - contains the x,y coordinates of the location to be checked
 * @id - current avatar's identification number
 *
 * Assumptions - there must be a valid AM_Message to be read and the spot location must be fed as an XYPos struct
 * 
 * Pseudo Code:
 * Takes the XYPos position and loops through all the current avatar positions contained within the AM_Message passed
 * Excluding the avatar with the passed identification number, if another avatar has a current position matching the 'spot'
 *	then return 1, otherwise returns 0
 */
int avatarInSpot(AM_Message avTurn, XYPos spot, int id);


/* moveAlgorithim - determines the optimal move direction for an avatar to request the server to make
 *
 * @currentPos - the avatar's current x,y coordinate position
 * @breadList - linked list containing the avatar's breadCrumbs
 * @avTurn - AM_Message containing the current location of all avatars
 * @id - current avatar's identification number
 *
 * Assumptions - avatar's current x,y coordinate must be in XYPos form, breadList must exist, must be a valid AM_Message
 * 
 * Pseudo Code:
 * if the avatar is at the midpoint, make a NULL_MOVE
 * if the avatar has hit a dead,
 * 	if the breadList is longer than 1, pop of the dead end node and the one before it
 * 		and move back out of the dead end, while also adding a one-sided wall to block off the dead end
 * 	if the breadList is 1, pop of the dead end node and do the same thing as above
 * 	if the breadList is empty, just move out of the dead end and add a one-sided wall
 * if the avatar is not at a dead end,
 * 	calculate the horizontal and vertical distance to the midpoint
 * 	based on which distance is greater as well as the mazeArray's knowledge of walls,
 * 	choose a direction to move
 * if this move is telling you to go somewhere you just were, don't go there and pick a different
 * direction using the dontGoBack function. This is necessary so that the avatar doesn't oscillate
 * between two spots while it attempts to move towards the midpoint.
 * if it can't move in the direction that it wants to, prioritize exploring directions (direction that
 * are set at -1 in the mazeArray), and if they are all explored, move in whatever direction doesn't have
 * a wall.
 * return the move
 *
 *
 */
int moveAlgorithm(XYPos currentPos, breadCrumbList *breadList, AM_Message avTurn, int id);


/* dontGoBack - determines the optimal move such that the avatar prioritizes moving in the direction of the midpoint so long
 * 	as there is not a dead end in the way
 *
 * @vDistance - vertical distance between current location and midpoint
 * @hDistance - horizontal distance between current location and the midpoint
 * @north - integer representing the presence of a wall or not in the given direction
 * @east - integer representing the presence of a wall or not in the given direction
 * @south - integer representing the presence of a wall or not in the given direction
 * @west - integer representing the presence of a wall or not in the given direction
 * @badDirection - previous direction in which a move was attempted
 *
 * Assumptions - all passed parameters must be integer values
 * 
 * Pseudo Code:
 * Takes the vertical and horizontal distances along with the the values indicating whether or not walls exist for that slot
 * Uses these values to determine the optimal move such that the avatar prioritizes moving in the direction of the midpoint
 * However, if there is a wall in the way, impeding such movement, it priopritizes not moving in the direction that the avatar
 *	just came from unless that is the only way out of a dead end
 * Returns an integer representing the optimal move direction based off of the given parameters
 */
int dontGoBack(int vDistance, int hDistance, int north, int east, int south, int west, int badDirection);


/* GetMidpoint - calculates the midpoint of all avatars based off of their location coordinates
 *
 * @avTurn - AM_Message containing the current location of all avatars
 *
 * Assumptions - there must be a valid AM_Message to be read
 * 
 * Pseudo Code:
 * Adds the X and Y components of each avatar's current position together into two variables
 * Divides the two variables each by the number of total avatars
 * Rounds the results to the nearest integer values
 * Sets the universally defined midPoint X and Y values to the respective average calculated values
 */
void GetMidpoint(AM_Message avTurn);

#endif // AVATAR_H
