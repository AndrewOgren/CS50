
/* ========================================================================== */
/* File: AMStartup.c
 *
 * Authors: Andrew Ogren, Amos Cariati, James Thompson III

 * Date: 02/27/16
 *
 * Input: None
 *
 * Command line options:  None     
 *
 * Output:  Results of unit tests to stdout 
 *
 * Error Conditions:  None
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
// ---------------- Local includes  e.g., "file.h"
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


// ---------------- Local includes  e.g., "file.h"
#include "amazing.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 
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

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types
Direction **mazeArray;
// ---------------- Private variables
int width;
int height;
// ---------------- Private variables

// ---------------- Private prototypes
int InitializeMazeArray(int width, int height);
breadCrumbList* InitializeBreadList();
void printbreadList(breadCrumbList* list);
int push(breadCrumbList *breadList, XYPos position);
breadCrumb* pop(breadCrumbList *breadList);
void FreeBreadCrumbList(breadCrumbList *list);
void FreeMazeArray(int width, int height);
void UpdateMazeArray(XYPos prev, XYPos curr, int moveDirection);
// ---------------- Public functions



int main(int argc, char **argv)
{

	fprintf(stdout, "This is a unit test of the main data structures.\n");

	fprintf(stdout, "\nFirst, we will test the mazeArray functionality.\n");

	fprintf(stdout, "\nNow initializing the mazeArray.\n");

	width = 6;
	height = 9;

	int returnCode = InitializeMazeArray(width, height);

	if (returnCode == 1){
		fprintf(stdout, "\nFailed to initialize the mazeArray.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\nSuccessfully initialized the mazeArray.\n");
	}

	fprintf(stdout, "\nNow testing to see if the walls were correctly added.\n");

	for (int left = 0; left < height; left++) {
		if (mazeArray[left][0].west != 1) {
			fprintf(stdout, "\nWall boundary creation failed.\n");
			exit(1);
		}
	}
	for (int top = 0; top < width; top++) {
		if (mazeArray[0][top].north != 1){
			fprintf(stdout, "\nWall boundary creation failed.\n");
			exit(1);
		}
	}
	for (int right = 0; right < height; right++) {
		if (mazeArray[right][width-1].east != 1){
			fprintf(stdout, "\nWall boundary creation failed.\n");
			exit(1);
		}
	}
	for (int bottom = 0; bottom < width; bottom++) {
		if(mazeArray[height-1][bottom].south != 1) {
			fprintf(stdout, "\nWall boundary creation failed.\n");
			exit(1);
		}
	}
	fprintf(stdout, "\nWall boundary creation was successful.\n");

	fprintf(stdout, "\nNow testing the initialization of the breadCrumbsArray.\n");

	breadCrumbList *list = InitializeBreadList();
	if (list == NULL){
		fprintf(stdout, "\nFailed to initialize bread list.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\nSuccessfully initialized breadCrumbList.\n");
	}

	fprintf(stdout, "\nNow pushing 2 breadcrumbs onto the list.\n");

	XYPos position1;
	position1.x = (uint32_t)2;
	position1.y = (uint32_t)3;
	int returnPush = push(list, position1);
	if (returnPush == 1){
		fprintf(stdout, "\npush #1 failed.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\npush #1 was successful.\n");
	}

	XYPos position2;
	position2.x = (uint32_t)2;
	position2.y = (uint32_t)4;
	returnPush = push(list, position2);
	if (returnPush == 1){
		fprintf(stdout, "\npush #2 failed.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\npush #2 was successful.\n");
	}

	fprintf(stdout, "\nNow printing the list to examine its contents.\n"
	"\nThe list should be (2,3) and then (2,4).\n");

	printbreadList(list);

	fprintf(stdout, "\nNow popping one breadCrumb off.\n");

	breadCrumb *returnPop = pop(list);
	if (returnPop == NULL){
		fprintf(stdout, "\nThe pop failed.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\nThe pop was successful.\n");
		fprintf(stdout,"\nThe returned breadcrumb is from point %d,%d\n", returnPop->position.x, returnPop->position.y);
	}

	fprintf(stdout, "\nThe last breadcrumb will now be popped from the list.\n");
	returnPop = pop(list);
	if (returnPop == NULL){
		fprintf(stdout, "\nThe pop failed.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\nThe pop was successful.\n");
		fprintf(stdout,"\nThe returned breadcrumb is from point %d,%d\n", returnPop->position.x, returnPop->position.y);
	}

	fprintf(stdout, "\nNow we will try to pop from an empty list.\n");
	returnPop = pop(list);
	if (returnPop == NULL){
		fprintf(stdout, "\nSuccess. Avoided a seg fault.\n");
	}
	else {
		fprintf(stdout, "\nHmmmm. Strange behavior.\n");
	}

	fprintf(stdout, "\nFinally, we will test updating the mazeArray with these two points.\n");
	UpdateMazeArray(position1, position2, 2);
	fprintf(stdout, "\nThe mazeArray was successfully updated.\n");

	fprintf(stdout, "\nNow cleaning up the allocated memory.\n");

	FreeBreadCrumbList(list);
	FreeMazeArray(width, height);
	
	fprintf(stdout, "\nAll memory should be freed. Check with valgrind.\n");
	

	return 0;


}


int InitializeMazeArray(int width, int height) {
	mazeArray= (struct Direction**)calloc(height,sizeof(struct Direction*));
	if (mazeArray == NULL){
		return 1;
	}
	for (int i=0; i<height; i++) {
		mazeArray[i] = (struct Direction*)calloc(width+1,sizeof(struct Direction)); //WHY WIDTH+!
		if (mazeArray[i] == NULL){
			return 1;
		}
	}
	//set all spots to 0 initially
	for (int i=0; i<height; i++){
		for (int j=0; j<width; j++){
			//May need to allocate memory here for these Direction nodes
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
	int prevX = (prev.x);
	int prevY = (prev.y);
	int currX = (curr.x);
	int currY = (curr.y);
	//Check to see if this move has already been added to the mazeArray
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
	breadCrumb *crumb = (struct breadCrumb*)malloc(sizeof(breadCrumb));
	if (crumb == NULL){
		return 1;
	}
	
	if ((breadList->head == NULL) && (breadList->tail == NULL)) {
		breadList->head = crumb;
		breadList->tail = crumb;
		crumb->next = NULL;
		crumb->prev = NULL;
		crumb->position = position;

	}

	else {
		breadList->tail->next = crumb;
		crumb->prev = breadList->tail;
		crumb->next = NULL;
		breadList->tail = crumb;
		crumb->position = position;

	}
	(breadList->length)++;
	return 0;

}

breadCrumb* pop(breadCrumbList *breadList){
	breadCrumb *temp;

	if (breadList->length > 1){
		temp = breadList->tail;
		breadList->tail = breadList->tail->prev;
		breadList->tail->next = NULL;
	}

	else {
		temp = breadList->tail;
		breadList->head = NULL;
		breadList->tail = NULL;	
	}
	(breadList->length)--;
	return temp;

}


void printbreadList(breadCrumbList* list){
	breadCrumb *breadptr = list->head;

	if (breadptr == NULL){

		fprintf(stdout, "The list is empty.\n");
	}

	else {
		fprintf(stdout, "Here is the list: ");
		while (breadptr != NULL){
			int x = (breadptr->position.x);
			int y = (breadptr->position.y);
			fprintf(stdout, "'%d,%d' ", x, y);
			breadptr = breadptr->next;
		}
		fprintf(stdout,"\n");
	}

}

void FreeBreadCrumbList(breadCrumbList *list){
	breadCrumb *current= list->head;
	if (list->head == NULL){
		free(list);
	} 
	else {
		while (current != NULL){
			breadCrumb *next = current->next;
			free(current);
			current = next;
		}
		free(list);
	}

	fprintf(stdout, "Freed BreadCrumb List\n");
	fflush(stdout);
}



