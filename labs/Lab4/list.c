/* ========================================================================== */
/* File: list.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * Author: Andrew ogren
 * Date: Feb 6, 2016
 *
 * These functions are used to initialize a doubly linked list, append 
 * nodes to the doubly linked list, and remove nodes from the doubly linked list.
 */
/* ========================================================================== */
//
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include "list.h"
#include "utils.h"
#include "common.h"
#include "hashtable.h"
#include <stdlib.h>

// ---------------- Local includes  e.g., "file.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 

/*====================================================================*/

/*
 * initializeList - initializes the doubly linked list
 * 
 * Assumptions: None
 * 
 * Pseudocode: 
 * Allocate memory for urlList of type List
 * Set its head and tail to NULL to start
 * return that list.
 */
List* initializeList(){
	List *urlList= (struct List*)malloc(sizeof(List));
	urlList->head = NULL;
    urlList->tail = NULL;
	return urlList;
}

/*
 * AppendList - appends a ListNode to the doubly linked list
 * 
 * Assumptions: the webpage has been properly allocated and initialized
 * the urlList has already been initialized via initializeList()
 * 
 * Pseudocode:
 * Allocate memory for a new ListNode
 * If allocation fails, return with 1
 * Set the node's page member to the provided page
 * If this is the first node in the list, set pointers appropriately
 * If this is not the first node in the list, set pointers appropriately
 * Return upon successful completion
 */
int AppendList(WebPage *page, List *urlList) {
	ListNode *node;
	node = malloc(sizeof(ListNode));
	if (node == NULL){
		return 1;
	}
	node->page = page;
	// If the list is empty
	if ((urlList->head == NULL) && (urlList->tail == NULL)) {
		node->prev = urlList->tail;
		node->next = NULL;
		urlList->head = node;
		urlList->tail = node;
		return 0;
	}
  	// If there are already nodes in the list
	else {
		node->prev = urlList->tail;
		(urlList->tail)->next = node;
		urlList->tail = node;
		urlList->tail->next = NULL;
		return 0;
	}
	return 0;
}
/*
 * PopList - Removes a ListNode from the front of the list
 * 
 * Assumptions: the urllist has already been initialized and allocated
 * and the list is not empty
 * 
 * Pseudocode:
 * As a precaution, if there is nothing in the list, return NULL
 * If this is the last node in the list, set pointers appropriately
 * If not, set pointers appropriately
 * Get the page from the listnode
 * free the node
 * return the page
 */

WebPage *PopList(List *urlList) {

	if (urlList->head == NULL) {
		return NULL;
	}

	ListNode *returnNode = urlList->head;
	
	if ( (urlList->head->next) == NULL) {
		urlList->head = returnNode->next;
		urlList->tail = NULL;
	}
	
	else {
		urlList->head = returnNode->next;
		urlList->head->prev= NULL;
	}
	// Save the page before getting rid of the node
	WebPage *returnPage = returnNode->page;

	free(returnNode);

	return returnPage;
}



