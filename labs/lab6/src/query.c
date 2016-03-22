/*	query.c	- searches for words in html documents, ranks the documents based on occurrences, and returns their urls

	Project name: Tiny Search Engine
	Component name: Query
	
	Primary Author:	Andrew Ogren
	Date Created: 02/20/2016

	Special considerations: None 
		(e.g., special compilation options, platform limitations, etc.) 
	
======================================================================*/
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <getopt.h>
// ---------------- Local includes  e.g., "file.h"
#include "rank.h"
#include "../util/libtseutil.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions
#define MAXCHARS 1000

// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 
int AcceptQueries(GenHashTable *index, const char* dirPath);

char* CleanUpWord(char* token);
/*====================================================================*/



int main(int argc, char* argv[]){

	// Allows for a help option
	static struct option longOptions[] =
	{
		{ "help", no_argument, 0, 'h'}
	};

	char opt;
	//Gets the help option if it's used, if any other option is used, it is invalid
	while ((opt = getopt_long(argc, argv, "h", longOptions, NULL)) != -1) {
		
		switch(opt) {
			case 'h':
				fprintf(stdout, "To properly run Query, do the following:\n"
				"./query [INDEX FILE PATH] [CRAWLER DIRECTORY PATH]\n"
				"The [INDEX FILE PATH] is the path to the index file that was created in Indexer.\n"
				"The [CRAWLER DIRECTORY PATH] is the path to the directory that holds the html files from crawler.\n"
				"Once query is running, type any words to search for them in the urls.\n"
				"If you want both words to appear in the document, use 'AND' in between them.\n"
				"If you want either word to appear in the document, use 'OR' in between them.\n"
				"When you are done, type 'q' and then return to exit.\n");
				return 1;

			case '?':
			default:
				fprintf(stdout, "Error: bad option \'%c\' \n", optopt);
				fprintf(stdout, "If you need more information, try --help");
				return 1;
		}
	}
	
	//Parameter checks for Command-line execution
	//Make sure there are the appropriate number of arguments given
	if (argc != 3){
		fprintf(stdout, "There must be two arguments provided on the command line"
		" in order to run query.\nThe first argument must be a valid path to a file that"
		" should contain an inverted index.\nThe second argument must be a valid path to a"
		" directory that should contain crawled html files.");
		exit(1);
	}
	//Make sure that the first argument is a valid index file
	struct stat statbuf;
	const char* path= argv[1];
	if ((!stat(path, &statbuf) && S_ISREG(statbuf.st_mode)) == 0) {
		fprintf(stdout, "The first argument must be a valid file path."
		" The file must already exist, and this file should contain the inverted index.");
		exit(1);
	}
	//Make sure that the second argument is a valid path to a directory
	const char* dirPath= argv[2];
	if ((!stat(dirPath, &statbuf) && S_ISDIR(statbuf.st_mode)) == 0) {
		fprintf(stdout, "The second argument must be a valid path to a directory."
		" This directory should contain the crawled html files.");
		exit(1);
	}
	//Make sure that the path to the directory ends in a '/', so that the files in the directory can be opened
	const char *endPath = "/";
	if (strcmp(endPath, &dirPath[strlen(dirPath)-1]) != 0) {
		fprintf(stdout, "The directory path must end with a '/'.");
		exit(1);
	}

	char *indexFile= argv[1];
	//Build the Index Again
	fprintf(stdout, "Building the index...please wait to query.\n");
	fflush(stdout);
	TestingIndex();
	GenHashTable *index = ReadFile(indexFile);

	//take queries
	int queryReturnCode = AcceptQueries(index, dirPath);
	if (queryReturnCode == 1){
		fprintf(stdout, "Error: Memory Allocation Failure");
		exit(1);
	}

	//Free the remaining allocated memory before quitting
	CleanDynamicList(index);
	free(index);

	return 0;
}


/*
 * AcceptQueries - takes all of the user's queries and handles them accordingly
 *
 * @index - the index that was just rebuilt from the index file
 * @dirPath - the path to the directory that contains the crawled files
 *
 * Assumptions - None - assume that the user can type anything
 *
 * Pseudo Code:
 * Prompt for queries
 * Wait for input permanently
 * When input is received, initialize a query list
 * Declare several arrays:
 * 	docArray holds the current word's documents that it was found in
 * 	compareArray keeps a running list of documents that contain all of the words up until an 'OR'
 * 	archiveArray holds all of the documents that will eventually need to be sorted and have their urls added
 * 	wordArray is a list of all of the words, not including the special keywords, 'AND' and 'OR'
 * Split the input up by spaces
 * For each word
 * Make it all lowercase and get rid of any whitespace unless it's 'AND' or 'OR'
 * If the word is 'q', exit query
 * If the word is 'OR', 
 * 	create or update the archive array appropriately
 * If the word is 'AND',
 * 	move on to the next word, because two words next to each other without an 'AND' are treated the same way
 * If it's a regular word, search for it in the index
 * If it's not found, NULL is returned, and noMatches is set to 1 until an 'OR' is used with another word
 * If it's found, then the docArray is returned, and
 * the word is added to the wordArray and
 * the docArray is compared with the compareArray, so that the compareArray is updated
 * Once there are no more words,
 * If there were no matches, let the user know
 * If OR was never used, but there are matches, rank the documents in the compareArray
 * If the archiveArray is not NULL (meaning OR was used), fully update the archiveArray, and then rank the documents in it
 * With the returned singly-linked list from the RankDocuments function, go through each node and print out the docID and url
 * Free any allocated memory
 * Wait for more user input
 */
int AcceptQueries(GenHashTable *index, const char* dirPath) {
	char buffer[MAXCHARS];

	fprintf(stdout, "QUERY:>");
	fflush(stdout);
	//Wait for more input from the user up to 1000 chars at a time
	while((fgets(buffer, MAXCHARS, stdin))) {
		List* queryList = InitializeList();
		if (queryList == NULL){
			fprintf(stdout, "Error: Memory allocation failure.");
			return 1;
		}
		int *docArray= NULL;
		int *compareArray= NULL;
		int *archiveArray = NULL;
		char *wordArray = NULL;
		int wordArraySize= 0;
		int noMatches= 0;
		
		const char OR[]="OR";
		const char AND[]="AND";
		const char SPACE[]=" ";
		//Split up the input by spaces (e.g. get each individual word as a token)
		char *spaceToken= strtok(buffer, SPACE);
		//While there are more words
		while( spaceToken != NULL){
			char *word= CleanUpWord(spaceToken);
			if (strcmp(word, "q") == 0){
				free(queryList);
				return 0;
			}

			if (strcmp(word, OR) == 0) {
				if (noMatches == 0){ //Don't add anything to the archiveArray if no docs should be added
					if(archiveArray == NULL){ //The first time adding to the archiveArray
						if(compareArray != NULL){
							int numCompElems= compareArray[0]; //the number of elements are stored in the 0th index
							archiveArray = (int*)calloc(numCompElems+1,sizeof(int));
							if (archiveArray == NULL){
								return 1;
							}
							archiveArray[0]= numCompElems;
							for(int i=1; i<(numCompElems+1); i++){
								archiveArray[i] = compareArray[i];
							}
							free(compareArray);
							compareArray = NULL; //reset the compareArray
						}	
					}

					else { //If the archiveArray has already been added to
						if(compareArray != NULL){
							//Figure out the number of new documents to be added to the archiveArray
							int numArchElems = archiveArray[0];
							int numCompElems = compareArray[0];
							int newDocs = 0;
							int oldDoc = 0;
							for(int i=1; i<(numCompElems+1); i++){
								for(int j=1; j<(numArchElems+1); j++){
									if(compareArray[i] == archiveArray[j]){
										oldDoc = 1;
										break;
									}
								}
								if (oldDoc == 0){
									newDocs++;
								}
								oldDoc= 0;
							}
							//Add those new documents into a temporary array
							int* tempArray = (int*)calloc(newDocs,sizeof(int));
							if (tempArray == NULL){
								return 1;
							}
							newDocs = 0;
							for(int i=1; i<(numCompElems+1); i++){
								for(int j=1; j<(numArchElems+1); j++){
									if(compareArray[i] == archiveArray[j]){
										oldDoc = 1;
										break;
									}
								}
								if (oldDoc == 0){
									tempArray[newDocs] = compareArray[i];
									newDocs++;
								}
								oldDoc= 0;
							}
							//Reallocate enough space in the archiveArray and add the new documents
							archiveArray = (int*)realloc(archiveArray,(sizeof(int)*(numArchElems+newDocs+1)));
							if (archiveArray == NULL){
								return 1;
							}
							archiveArray[0] = (numArchElems+newDocs);
							int counter= 0;
							for(int i=(numArchElems+1); i<(numArchElems+newDocs+1); i++){ 
								archiveArray[i] = tempArray[counter];
								counter++;
							}
							free(tempArray);
							free(compareArray);
							compareArray= NULL;
						}
					}
				}
				noMatches = 0;
				spaceToken = strtok(NULL, SPACE);
				continue;
			}
			//If the word is 'AND', simply continue, since two words right next to each other are treated the same way
			if (strcmp(word, AND) == 0){
				spaceToken = strtok(NULL, SPACE);
				continue;
			}
			//If it's a regular word, search for it in the index
			docArray= SearchIndex(word, index);
			if(docArray == NULL){
				if (compareArray != NULL){
					free(compareArray);
					compareArray= NULL;
				}
				if ((wordArray != NULL) && (archiveArray == NULL)){ 
					free(wordArray);
					wordArray = NULL;
				}
				noMatches = 1;
				spaceToken = strtok(NULL, SPACE);
				continue;
			}
			// If there's a word with no matches, and an 'OR' has not been reached yet, continue
			if(noMatches == 1){
				free(docArray);
				spaceToken = strtok(NULL, SPACE);
				continue;
			}

			//If the word does have matches, add it to the wordArray
			if (wordArray == NULL){
				wordArraySize = (strlen(word)+1);
				wordArray = (char*)calloc(1,(sizeof(char)*wordArraySize));
				if (wordArray == NULL){
					return 1;
				}
				strcpy(wordArray, word);
			}
			else {
				wordArraySize = wordArraySize+(strlen(word)+1);
				wordArray = (char*)realloc(wordArray,(sizeof(char)*wordArraySize));
				if (wordArray == NULL){
					return 1;
				}
				strcat(wordArray, " ");
				strcat(wordArray, word); //should only be one new word
			}
			
			//Then compare the word with the compareArray to see if there are matching documents
			if (compareArray == NULL) {
				int numDocElems= docArray[0];
				compareArray= (int*)calloc(numDocElems+1,sizeof(int));
				if (compareArray == NULL){
					return 1;
				}
				compareArray[0]= numDocElems;
				for(int i=1; i<(numDocElems+1); i++){ 
					compareArray[i]= docArray[i];
				}
			}

			else {
				int numDocElems= docArray[0];
				int numCompElems= compareArray[0];
				int numChecked= 0;

				for(int i=1; i<(numCompElems+1); i++) {
					for(int j=1; j<(numDocElems+1); j++) {
						if(docArray[j] != compareArray[i]) {
							numChecked++;
						}
						else {
							break;
						}

						if(numChecked == numDocElems){
							compareArray[i] = 0;
						}
					}
					numChecked= 0;
				}


			}
			free(docArray);
			spaceToken = strtok(NULL, SPACE);

		}
		//Print the results to stdout
		//No matches were found
		if ((noMatches == 1) && (archiveArray == NULL)){
			if (queryList != NULL){
			FreeQueryNodes(queryList);
			}
			if (compareArray != NULL){
				free(compareArray);
			}
			if (wordArray != NULL){
			free(wordArray);
			}	
			fprintf(stdout, "Sorry, there were no matches.\n");
			fprintf(stdout, "QUERY:>");
			fflush(stdout);
			continue;
		}

		//"OR" was never called, but there were word matches
		else if ((compareArray != NULL) && (archiveArray == NULL)){
			int returnCode = RankDocuments(compareArray, dirPath, wordArray, queryList, index);
			if (returnCode == 1){
				return 1;
			}
		}

		else if (archiveArray != NULL){
			if(compareArray != NULL){
				//Update the archiveArray with the latest compareArray
				int numArchElems = archiveArray[0];
				int numCompElems = compareArray[0];
				int newDocs = 0;
				int oldDoc = 0;
				for(int i=1; i<(numCompElems+1); i++){
					for(int j=1; j<(numArchElems+1); j++){
						if(compareArray[i] == archiveArray[j]){
							oldDoc = 1;
							break;
						}
					}
					if (oldDoc == 0){
						newDocs++;
					}
					oldDoc= 0;
				}
				int* tempArray = (int*)calloc(newDocs,sizeof(int));
				if (tempArray == NULL){
					return 1;
				}
				newDocs = 0;
				for(int i=1; i<(numCompElems+1); i++){
					for(int j=1; j<(numArchElems+1); j++){
						if(compareArray[i] == archiveArray[j]){
							oldDoc = 1;
							break;
						}
					}
					if (oldDoc == 0){
						tempArray[newDocs] = compareArray[i];
						newDocs++;
					}
					oldDoc= 0;
				}

				archiveArray = (int*)realloc(archiveArray,(sizeof(int)*(numArchElems+newDocs+1)));
				if (archiveArray == NULL){
					return 1;
				}
				archiveArray[0] = (numArchElems+newDocs);
				int counter = 0;
				for(int i=(numArchElems+1); i<(numArchElems+newDocs+1); i++){ //Not sure about this
					archiveArray[i] = tempArray[counter];
					counter++;
				}
				free(tempArray);

			}
			//rank the documents in the archiveArray
			int returnCode = RankDocuments(archiveArray, dirPath, wordArray, queryList, index);
			if (returnCode == 1){
				return 1;
			}
		}
		else {
			fprintf(stdout, "Sorry, there were no matches.\n");
			if (queryList != NULL){
				FreeQueryNodes(queryList);
			}
			if (wordArray != NULL){
				free(wordArray);
			}
			fprintf(stdout, "QUERY:>");
			fflush(stdout);
			continue;	
		}
		//Run through the singly-linked list and print out the docID and associated urls
		QueryResponse *currentQueryNode = queryList->head;
		if (currentQueryNode != NULL){
			while (currentQueryNode != NULL){
				fprintf(stdout, "Document ID:%d URL:%s\n", currentQueryNode->docID, currentQueryNode->url);
				fflush(stdout);
				currentQueryNode = currentQueryNode->next;
			}
		}
		else {
			fprintf(stdout, "Sorry, there were no matches.\n");
		}
		//free memory appropriately
		if (queryList != NULL){
			FreeQueryNodes(queryList);
		}
		if (compareArray != NULL){
			free(compareArray);
		}
		if (archiveArray != NULL){
			free(archiveArray);
		}
		if (wordArray != NULL){
			free(wordArray);
		}
		fprintf(stdout, "QUERY:>");
		fflush(stdout);	
	}
	return 0;
}

/*
 * CleanUpWord - takes a word and removes whitespace or newline characters and makes it lowercase
 * 
 * @token - the word to "clean-up"
 *
 * Assumptions - Input is a string
 * 
 * Pseudo Code:
 * Takes the word and
 * while there is whitespace at the front of it, it moves along and gets rid of it
 * If it was all whitespace, then return it
 * Find the end of the string
 * While you haven't reached the beginning of the string and there's still whitespace
 * move back a character
 * If the word is not 'OR' or 'AND', make all of the characters lowercase
 * return the words
 */
char* CleanUpWord(char* token) {
	char *end;

	while(isspace(*token)) {
		token++;
	}

	if(*token == 0) {
		return token;
	}

	end = token + strlen(token) - 1;
	while(end > token && isspace(*end)) {
		end--;
	}
	*(end+1) = 0;
	//Modified from NormalizeWord function in indexer
	if((strcmp(token, "OR") != 0) && (strcmp(token, "AND") != 0)) {
		for(char *ptr = token; *ptr; ++ptr) {
	        *ptr = tolower(*ptr);
		}
	}
	return token;
}

