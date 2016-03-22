/*	queryengine_test.c	Tests the main data structures of the query engine

	Project name: Tiny Search Engine
	Component name: Query
	
	Primary Author:	Andrew Ogren
	Date Created: 02/24/16

	Special considerations:  
		(e.g., special compilation options, platform limitations, etc.) 
	
======================================================================*/
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ---------------- Local includes  e.g., "file.h"
#include "../util/libtseutil.h"
#include "../src/rank.h"
// ---------------- Constant definitions 

// ---------------- Macro definitions
#define INDEXFILE "/net/class/cs50/web/tse/indexer/cs_lvl3.dat"
#define CRAWLERFILES "/net/class/cs50/web/tse/crawler/lvl2/"

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 
GenHashTable *ReadFile(char *file);
int* SearchIndex(char* word, GenHashTable *index);


/*====================================================================*/

int main(int argc, char* argv[]){
	//Unit test of the index
	
	fprintf(stdout, "\nTesting the re-building of the index.\n");
	char *indexFile = "/net/class/cs50/web/tse/indexer/cs_lvl3.dat";
	TestingIndex();
	GenHashTable *table = ReadFile(indexFile);
	
	if (table != NULL){
		fprintf(stdout, "\nSuccessfully initialized the index.\n");
	}
	else {
		fprintf(stdout, "\nFailed to initialize the index. Exiting the test.\n");
		exit(1);
	}

	fprintf(stdout, "\nDetermining if a word exists in the index.\n");

	char *word;
	word = "computer";

	fprintf(stdout, "\nThis word should exist in the index.\n");

	int *returnCode = SearchIndex(word, table);

	if (returnCode != NULL){
		fprintf(stdout, "\nThe word was successfuly found in the index.\n");
	}
	else {
		fprintf(stdout, "\nThe search failed. The word was not found.\n");
	}

	fprintf(stdout, "\nDetermining if a word exists in the index.\n");

	char *word2;
	word2= "lfnalfalfn;ae";

	fprintf(stdout, "\nThis word should not exist in the index.\n");

	int *returnCode2= SearchIndex(word2, table);
	if (returnCode2 == NULL){
		fprintf(stdout, "\nThe word was not found in the index. Success.\n");
	}
	else {
		fprintf(stdout, "\nThe search failed. The word was somehow found.\n");
	}

	//Unit test of the Query's singly-linked list

	fprintf(stdout, "\nNow initializing the singly-linked list for the query.\n");

	List* list = InitializeList();

	if (list != NULL){
		fprintf(stdout, "\nThe list was successfully initialized.\n");
	}
	else {
		fprintf(stdout, "\nThe list failed to be initialized. Exiting the test.\n");
		exit(1);
	}
	
	fprintf(stdout, "\nCreating a document array and a word array for the word, 'computer'\n");

	int *docArray = SearchIndex(word, table);

	char *wordArray = "computer";

	fprintf(stdout, "\nTesting to see whether the 'computer' documents can be ranked.\n");

	int rankReturn = RankDocuments(docArray, CRAWLERFILES, wordArray, list, table);

	if (rankReturn == 1){
		fprintf(stdout, "\nFailed to rank the document by occurrences of the word, 'computer'\n");
	}
	else {
		fprintf(stdout, "\nSuccessfully ranked the documents.\n");
	}

	fprintf(stdout, "\nNow we will see if the urls were properly added to the queryNodes. We will"
	" also see if the documents are ranked correctly.\n");

	int urlCounter = 0;
	int ranker= 0;
	QueryResponse *currentQueryNode = list->head;
	QueryResponse *prevQueryNode = NULL;
	if (currentQueryNode != NULL){
		while (currentQueryNode != NULL){
			if (currentQueryNode->url == NULL){
				urlCounter++;
			}
			if (prevQueryNode != NULL){
				if (currentQueryNode->totalOccurrences > prevQueryNode->totalOccurrences){
					ranker = 1;
				}
			}
			prevQueryNode = currentQueryNode;
			currentQueryNode = currentQueryNode->next;
		}
	}
	if (urlCounter != 0){
		fprintf(stdout, "\nThere were %d urls that were not properly added.\n", urlCounter);
	}
	else {
		fprintf(stdout, "\nAll urls were properly added.\n");
	}

	if (ranker == 1){
		fprintf(stdout, "\nThe ranking failed.\n");
	}
	else {
		fprintf(stdout, "\nThe ranking of the documents was a success.\n");
	}

	fprintf(stdout, "\nFreeing the allocated memory. Running this with valgrind should give 0 memory leaks.");
	//Freeing the allocated memory
	free(docArray);
	free(returnCode);
	free(returnCode2);
	CleanDynamicList(table);
	FreeQueryNodes(list);
	free(table);

	fprintf(stdout, "\nEnd of queryengine_test.c\n");
	
	return 0;

}





