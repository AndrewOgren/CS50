/* ========================================================================== */
/* File: rank.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query
 *
 * This file contains the document ranking functionality.
 *
 */
/* ========================================================================== */

// ---------------- Prerequisites e.g., Requires "math.h"
#include "../util/libtseutil.h"
// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros

#ifndef RANK_H
#define RANK_H



typedef struct QueryResponse {					//A node of the list that contains information that will be printed out
	struct QueryResponse *next;
	int docID;
	int totalOccurrences;
	char* url;
} QueryResponse;

typedef struct List {
    QueryResponse *head;                          // "beginning" of the list
} List;


/*
 * InitializeList - initializes the list of query nodes
 *
 * Returns the newly initialized singly-linked list
 */
List* InitializeList();

/*
 * RankDocuments - for each document in the docArray, it orders it by occurrences of all of the words and adds the appropriate url
 *
 * @docArray - the array with all of the documents that should be displayed
 * @dirPath - the path to the crawled html files
 * @wordArray - the array containing all of the queried words
 * @queryList - the initialized singly-linked list
 * @index - the rebuilt index containing all of the words and their occurrences
 *
 * Returns 0 if successful, or 1 if there was a memory allocation failure
 */
int RankDocuments(int* docArray, const char* dirPath, char* wordArray, List* queryList, GenHashTable* index);


/*
 * AddURL - adds the appropriate url for the crawler document
 *
 * @queryNode - the node that was created in RankDocuments
 * @docID - the document id found in the docArray
 * @dirPath - the path to the crawled files
 *
 * Returns 0 if successful, or 1 if there was a memory allocation failure
 */
int AddURL(QueryResponse *queryNode, int docID, const char* dirPath);


/*
 * FreeQueryNodes - frees the nodes and their members from the singly-linked query list
 *
 * @queryList - the list that is used to output the docids and urls
 *
 * Returns void
 */
void FreeQueryNodes(List* queryList);
















#endif // RANK_H