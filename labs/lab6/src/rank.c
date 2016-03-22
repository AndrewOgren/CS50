/*	rank.c 

	Project name: Tiny Search Engine
	Component name: Query
	
	Primary Author:	Andrew Ogren
	Date Created: 02/20/2016

	Special considerations:  
		(e.g., special compilation options, platform limitations, etc.) 
	
======================================================================*/
// do not remove any of these sections, even if they are empty
//
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------- Local includes  e.g., "file.h"
#include "rank.h"
#include "../util/libtseutil.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 
/*====================================================================*/
/*
 * InitializeList - initializes the list of query nodes
 *
 * Assumptions - the list has not been initialized already
 * Pseudo Code:
 * Allocate space for the list
 * set the head to null
 * return the newly initialized list
 */
List* InitializeList() {
	List *urlList= (struct List*)malloc(sizeof(List));
	urlList->head = NULL;
	return urlList;
}

/*
 * RankDocuments - for each document in the docArray, it orders it by occurrences of all of the words and adds the appropriate url
 *
 * @docArray - the array with all of the documents that should be displayed
 * @dirPath - the path to the crawled html files
 * @wordArray - the array containing all of the queried words
 * @queryList - the initialized singly-linked list
 * @index - the rebuilt index containing all of the words and their occurrences
 *
 * Assumptions - the parameters are all appropriately allocated
 *
 * Pseudo code:
 * Get the number of documents in the docArray
 * For all of the documents in the docArray
 * If the docID is 0, this means that it should not be included, so skip it
 * Initialize a query node of the appropriate size
 * Add the doc id
 * For each word in the word array
 * Find it in the index, find the associated document, and add the number of occurrences to the total # of occurrences
 * Add the url to the query node by calling AddUrl
 * Add it at the appropriate point in the singly-linked list based on the number of occurrences
 */
int RankDocuments(int* docArray, const char* dirPath, char* wordArray, List* queryList, GenHashTable* index){
	int numElems = docArray[0];
	for(int i=1; i<(numElems+1); i++){
		if (docArray[i] == 0){
			continue;
		}

		QueryResponse *queryNode = (struct QueryResponse*)malloc(sizeof(QueryResponse));
		if (queryNode == NULL){
			return 1;
		}
		queryNode->docID = docArray[i];
		//initially have the total occurrences set to 0
		int totalOccurrences= 0;
		//need to create a temporary copy of the wordArray since strtok() eats it up and permanently changes it
		char *wordArrayCopy = malloc(1+strlen(wordArray));
		if (wordArrayCopy == NULL){
			free(queryNode);
			return 1;
		}
		strcpy(wordArrayCopy, wordArray);

		char *token = strtok(wordArrayCopy, " ");
		while (token != NULL){

			unsigned long modulo= MAX_HASH_SLOT;
			unsigned long hashNum= JenkinsHash(token, modulo);
			//Find the word in the index
			WordNode *currentWord= index->table[hashNum];
			while(currentWord != NULL) {
	            if(strcmp(currentWord->word, token) == 0) {
	                break;
	            }
	            currentWord = currentWord->next;
	        } //Find the right document in the word's singly-linked list
			DocNode *currentDocNode = currentWord->doc;
			while(currentDocNode != NULL) {
				if (currentDocNode->docID == queryNode->docID) {
					totalOccurrences = totalOccurrences + currentDocNode->occurrences;
				}
				currentDocNode = currentDocNode->next;
			}

			token = strtok(NULL, " ");
		}
		free(wordArrayCopy);
		//add the totalOccurences member, so that it can be compared in the query List
		queryNode->totalOccurrences = totalOccurrences;
		AddURL(queryNode, docArray[i], dirPath);
		//add the node to the query List
		if (queryList->head == NULL) {
			queryList->head = queryNode;
			queryList->head->next = NULL;
			
		}

		else {
			QueryResponse *current = queryList->head;
			QueryResponse *previous = NULL;
			int added = 0;
			while(current != NULL){
				if((queryNode->totalOccurrences) >= current->totalOccurrences){
					if (previous != NULL){
						previous->next = queryNode;
					}
					else {
						queryList->head = queryNode;
					}
					queryNode->next = current;
					added = 1;
					break;	
				}
				previous = current;
				current = current->next;
			}
			if (added == 0){
				previous->next = queryNode;
				queryNode->next = NULL;
			}
		}
	}
	return 0;
}

/*
 * AddURL - adds the appropriate url for the crawler document
 *
 * @queryNode - the node that was created in RankDocuments
 * @docID - the document id found in the docArray
 * @dirPath - the path to the crawled files
 *
 * Assumptions - the dirPath is a valid path to the crawled html files
 * - the docID is the name of one of the files in that directory
 * 
 * Pseudo Code:
 * make the integer docID into a string
 * concatenate the docID with the dirPath to form the full filepath
 * open the file
 * the first line should be the url, so
 * scan the first line and get how many characters it is
 * allocate enough space for that many characters
 * rewind and scan the url, and add it to the querynode's member
 * free memory and return
 * 
 */
int AddURL(QueryResponse *queryNode, int docID, const char *dirPath) {
	int intLength = snprintf(NULL, 0, "%d", docID);
    char *docIDString= malloc(intLength + 1);
    // Make sure malloc was successful
    if (docIDString == NULL) {
        return 1;
    }
    else {
        sprintf(docIDString, "%d", docID);
    }
	
	char *pathName = (char*)malloc(sizeof(char)*(strlen(dirPath)+strlen(docIDString))+1);
	if (pathName == NULL){
		free(docIDString);
		return 1;
	}
	//Form the full path to the appropriate file
	sprintf(pathName, "%s%s", dirPath, docIDString);

	FILE *fp;
	fp = fopen(pathName, "r");

	if (fp == NULL){
		free(docIDString);
		free(pathName);
		return 1;
	}
	//Allocate enough space for the url
	int numChars;
	fscanf(fp, "%*s%n", &numChars);
	char *url = (char*)malloc(sizeof(char)*numChars+1);
	if (url == NULL){
		free(docIDString);
		free(pathName);
		fclose(fp);
		return 1;
	}
	rewind(fp);
	fscanf(fp, "%s", url);
	queryNode->url = url;
	// Add the url member
	free(docIDString);
	free(pathName);
	fclose(fp);

	return 0;		
}

/*
 * FreeQueryNodes - frees the nodes and their members from the singly-linked query list
 *
 * @queryList - the list that is used to output the docids and urls
 *
 * Assumptions - the queryList has been initialized
 * 
 * Pseudo Code:
 * If there are no nodes, simply free the list
 * If there are nodes, 
 * start at the head
 * while there are remaining nodes
 * free the current node and its members
 * move to the next node
 * finally free the list
 */
void FreeQueryNodes(List* queryList){
	if (queryList->head == NULL){
		free(queryList);
	}
	else {
		QueryResponse *currentQueryNode = queryList->head;
		QueryResponse *nextNode;
		while (currentQueryNode != NULL){
			nextNode = currentQueryNode->next;
			free(currentQueryNode->url);
			free(currentQueryNode);
			currentQueryNode = nextNode;
		}
		free(queryList);

	}

}





