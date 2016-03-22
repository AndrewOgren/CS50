/*	indexer.c	Takes html files and counts the occurrences of words in them.

	No Copyright

	No license

	Project name: Tiny Search Engine
	Component name: Indexer
	
	Primary Author:	Andrew Ogren
	Date Created: 02/14/2016

	Special considerations: None
	
======================================================================*/
// ---------------- Open Issues 

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
// ---------------- Local includes  e.g., "file.h"
#include "file.h"
#include "web.h"
#include "hashtable.h"
// ---------------- Constant definitions 

// ---------------- Macro definitions
#define TESTING 5
#define STANDARD 3
// ---------------- Structures/Types 

// ---------------- Private variables 

// ---------------- Private prototypes 
GenHashTable* buildIndexFromDirectory(const char* directory, GenHashTable* index, char *outputFile);

int UpdateIndex(GenHashTable *index, char *word, int documentId);

int SaveIndexToFile(GenHashTable *index, char* fileName);

GenHashTable *ReadFile(char *file);
/*====================================================================*/


int main(int argc, char* argv[]) {
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
				fprintf(stdout, "Usage for indexer: For each html file, get the number of occurrences of each word.\n"
				"Arguments: Input should either be: ./indexer  [TARGET_DIRECTORY] [RESULTS FILE NAME]\n"
				"Where [TARGET_DIRECTORY] is the directory that holds the html files to be indexed.\n"
				"And [RESULTS FILE NAME] is the name of the file to write the index to.\n\n"
				"Else, input should be: ./indexer  [TARGET_DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITTEN FILENAME]\n"
				"Where there should be two additional arguments\n"
				"[RESULTS FILENAME] is the same filename that the index was written to.\n"
				"[REWRITTEN FILENAME] is the new file name that the index will be written to to compare with the old index.");
				return 1;

			case '?':
			default:
				fprintf(stdout, "Error: bad option \'%c\' \n", optopt);
				fprintf(stdout, "If you need more information, try --help");
				return 1;
		}
	}
	// parameter processing
	// Makes sure that either 2 or 4 arguments were given (in addition to the name of the program)
	if (argc != STANDARD && argc != TESTING) {
		fprintf(stdout, "There are an invalid number of arguments.\n"
		"Input should either be: ./indexer  [TARGET_DIRECTORY] [RESULTS FILE NAME]\n"
		"Where [TARGET_DIRECTORY] is the directory that holds the html files to be indexed.\n"
		"And [RESULTS FILE NAME] is the name of the file to write the index to.\n\n"
		"Else, input should be: ./indexer  [TARGET_DIRECTORY] [RESULTS FILENAME] [RESULTS FILENAME] [REWRITEN FILENAME]\n"
		"Where there should be two additional arguments\n"
		"[RESULTS FILENAME] is the same filename that the index was written to.\n"
		"[REWRITTEN FILENAME] is the new file name that the index will be written to to compare with the old index.");
		exit(1);
	}
	// Makes sure the given directory name is valid
	const char *dirName= argv[1];
    if (IsDir(dirName) == 0) {
        fprintf(stdout, "This directory cannot be found: %s\n", dirName);
        fprintf(stdout, "The directory must already exist, and the path to the directory must be your"
            " first argument.\n For further information, try: ./indexer --help");
        exit(1);
    }
	
	// Makes sure the directory path ends in a '/', because if not, the file opening will fail
	const char *endPath = "/";
	if (strcmp(endPath, &dirName[strlen(dirName)-1]) != 0) {
		fprintf(stdout, "The directory path must end with a '/'.");
		exit(1);
	}
	// Checks that the given index file is valid
	if ((argc == STANDARD) || (argc == TESTING)) {
		const char *indexFile = argv[2];
		if (IsFile(indexFile) == 0) {
			fprintf(stdout, "This file cannot be found: %s\n", indexFile);
			fprintf(stdout, "The file must already exist.\n"
			"For further information, try: ./indexer --help");
			exit(1);
		}
	}
	// If it is in testing mode, the additional arguments should also be valid files
	// Additionally, make sure args 2 and 3 are the same, or else the file will not contain the correct information
	if (argc == TESTING) {
		const char *indexFile1 = argv[2];
		const char *indexFile2 = argv[3];
		const char *newFile = argv[4];
		if (IsFile(indexFile2) == 0){
			fprintf(stdout, "This file cannot be found: %s\n", indexFile2);
			fprintf(stdout, "The file must already exist.\n"
			"For further information, try: ./indexer --help");
			exit(1);
		}
		if (IsFile(newFile) == 0) {
			fprintf(stdout, "This file cannout be found: %s\n", newFile);
			fprintf(stdout, "The file must already exist.\n"
			"For further information, try: ./indexer --help");
			exit(1);
		}
		if(strcmp(indexFile1, indexFile2) != 0){
			fprintf(stdout, "The second and third arguments must be the same"
			" to allow proper testing.\n For further information, try: ./indexer --help");
			exit(1);
		}
	}


	// initialize data structures
	GenHashTable *hashtable = InitializeHashTable();
	
	fprintf(stdout, "Building the index...");
	fflush(stdout);

	GenHashTable *index = buildIndexFromDirectory(dirName, hashtable, argv[2]);
	// Clean up
	CleanDynamicList(index);
	free(index);
	fprintf(stdout, "done!\n");
	fflush(stdout);
	// Only do this in testing mode
	if (argc == TESTING){
		fprintf(stdout, "Testing index...");
		fflush(stdout);
		TestingIndex();
		// Create a new index
		GenHashTable *newIndex = ReadFile(argv[3]);
		// Save the new index to the provided file
		int saveCode = SaveIndexToFile(newIndex, argv[4]);
		if (saveCode == 1){
			fprintf(stdout, "There was an error opening the file: %s", argv[4]);
			CleanDynamicList(newIndex);
			free(index);
			exit(1);
		}
		fprintf(stdout, "done!");
		fflush(stdout);
		//Clean up
		CleanDynamicList(newIndex);
		free(newIndex);
	}


	return 0;
}

/*
 * buildIndexFromDirectory - gets all of the html file from the directory and builds an index with the words found
 *
 * @directory - the directory in which all of the html files are stored
 * @index - the index that will be built
 * @outputFile - the file to which the index will be written
 * 
 * Assumptions: The directory is valid and contains the html files
 * The index has been initialized and allocated
 * The output file already exists
 *
 * Pseudocode: 
 * For each file
 * Load the html
 * Create a document id for the file
 * While there are remaining words in the html
 * If the word is greater than two characters
 * Make the word all lowercase
 * Update the index accordingly
 * If updating the index fails, exit gracefully
 * Once the index is complete, save it to a file
 * Clean up
 * Return the newly created index
 */
GenHashTable* buildIndexFromDirectory(const char* directory, GenHashTable* index, char *outputFile) {
	
	char **fileNames= NULL;
	int numFiles= 0;
	numFiles = GetFilenamesInDir(directory, &fileNames);
	if (numFiles == 0){
		fprintf(stdout, "There were no files in the directory.");
		int i;
		for(i=0; i<numFiles; i++){
			free(fileNames[i]);
		}
		free(fileNames);
		free(index);
		exit(1);
	} 

	int docID;
	int pos;
	char *word;
	char *doc;

	int name;
	//For each file
	for(name=0; name < numFiles; name++) {
		doc = LoadDocument(directory, fileNames[name]); 
		if (doc == NULL) {
			fprintf(stdout, "\nThe document, %s, is empty.\n", fileNames[name]);
			fflush(stdout);
			continue;
		}
		docID = GetDocumentId(fileNames[name]); 
		if (docID == -1) {
			free(doc);
			fprintf(stdout, "\nCould not create a document id for: %s\n", fileNames[name]);
			fflush(stdout);
			continue;
		}
		pos = 0;
		while ((pos = GetNextWord(doc, pos, &word)) > 0) {
			//Only add words that are greater than length of 2 to the index
			if (strlen(word) > 2){
				if (pos == -1){
					free(word);
					continue;
				}
				// Makes all of the words lowercase, so they can properly be compared
				NormalizeWord(word);
				// Adds the word to the index
				int returnCode = UpdateIndex(index, word, docID);

				if (returnCode == 1){
					fprintf(stdout, "Error: Out of memory.");
					int i;
					for(i=0; i<numFiles; i++){
						free(fileNames[i]);
					}
					free(fileNames);
					free(fileNames);
					free(doc);
					free(word);
					CleanDynamicList(index);
					free(index);
					exit(1);
				}
			}
			else {
				free(word);
			}
		}
		free(doc);
	}

	int saveCode = SaveIndexToFile(index, outputFile);
	if (saveCode == 1){
		fprintf(stdout, "There was an error opening the file: %s", outputFile);
		free(fileNames);
		CleanDynamicList(index);
		free(index);
		exit(1);
	}
	// Frees the array of filenames and all of its contents
	int i;
	for(i=0; i<numFiles; i++){
		free(fileNames[i]);
	}
	free(fileNames);
	
	return index;

}

/*
 * UpdateIndex - updates the index accordingly
 *
 * @index - the index
 * @word - the word that will be saved in the index
 * @documentId - the unique id of the file
 * 
 * Assumptions - The index has been initialized properly
 * The word and documentId are not NULL
 * 
 * Psuedocode:
 * Lookup the provided word to see if it already exists in the index
 * If if doesn't exist, call NewNodeInsert to insert a word node and a document node
 * If it does exist, call DocNodeInsert to either increment the occurrences of a doc Node or add a doc node
 * If either method fails, it will return 1
 * If successful, return 0
 */
int UpdateIndex(GenHashTable *index, char *word, int documentId) {
	// Returns 0 if it hasn't been added, 1 if it has
	int visited = LookupWord(word, index);
	
	if (visited == 0){
		//Insert a new word node, because it hasn't been added yet
		int returnCode = NewNodeInsert(word, documentId, index);
		if (returnCode == 1){
			return 1;
		}
	}

	else {
		//Insert a doc node, because the word is already in the hash table
		int returnCode= DocNodeInsert(word, documentId, index);
		if (returnCode == 1){
			return 1;
		}
	}

	return 0;
}

/*
 * SaveIndexToFile - Take the newly created index and formats the information to save in a file
 * @index - the index
 * @fileName - the file to which the index will be saved
 * 
 * Assumptions: The index has been filled in fully
 * The file that it is writing to already exists
 *
 * Pseudocode:
 * Open the file for writing
 * If the file cannot be opened, return 1, indicating failure
 * For each node in the index (hashtable array)
 * get the word from the word node
 * Make sure it's not null
 * For each of the word node's in the linked list at that hash slot
 * Go through the word node's linked list of doc nodes to count the number of docs
 * For each of its doc nodes, get the doc id and occurrences
 * Send this info to the file
 * Move to the next word node
 * Close the file
 * Return 0 upon success
 */
int SaveIndexToFile(GenHashTable *index, char* fileName){
	// Open the file
	FILE *fp;
	fp = fopen(fileName, "w");
	// Make sure it was opened properly
	if (fp == NULL){
		return 1;
	}
	// For each hash slot
	int node;
	for(node=0; node< MAX_HASH_SLOT; node++){
		//Get the word node
		WordNode *currentWord = index->table[node];
		if (currentWord == NULL){
			continue;
		}
		// for each word node in the linked list at that hash slot
		while(currentWord != NULL) {
			// get the actual word and send it to the file
			fprintf(fp, "%s ", currentWord->word);
			// get the count of docs that that word appears in and send to the file
			int docCount = 0;
			DocNode *currentDocNode = currentWord->doc;
			if (currentDocNode == NULL){
				continue;
			}
			while(currentDocNode != NULL) {
				++docCount;
				currentDocNode = currentDocNode->next;
			}
			fprintf(fp, "%d ", docCount);
			// for each of the word node's doc nodes
			currentDocNode = currentWord->doc;
			while(currentDocNode != NULL){
				//get its id and occurrrences and send them to the file
				fprintf(fp, "%d %d ", currentDocNode->docID, currentDocNode->occurrences);
				// Move to the next doc node
				currentDocNode = currentDocNode->next;
			}
			fprintf(fp, "\n");
			
			currentWord = currentWord->next;
		}

	}
	//Close the file
	fclose(fp);

	return 0;
}

/*
 * ReadFile - Recreates the index from the given file
 *
 * @file - the file that contains the inverted index
 *
 * Assumptions: the provided file contains the previously created inverted index
 * 
 * Pseudocode: 
 * Open the file for reading
 * If the file can't be opened for reading, exit accordingly
 * Initialize the new index
 * While it hasn't reached the end of the file
 * Record the current position in the file
 * Scan the first word for the number of characters and save it in numChars
 * Allocate memory for that word accordingly
 * Move the current position back to the recorded position in the file
 * Scan and save the word and the numfiles that it was seen in 
 * For each file that the word was seen in 
 * Scan and save the docid of the file and the number of occurrences in that file
 * For each occurrence of the word in that file, update the index accordingly
 * Close the file
 * Return the new index
 */
GenHashTable *ReadFile(char *file){
	//Open the file for reading
	FILE *fp;
	fp = fopen(file, "r");

	if (fp == NULL){
		fprintf(stdout, "This file could not be opened for reading or is empty: %s", file);
		exit(1);
	}
	// Initialize new index
	GenHashTable *newIndex = InitializeHashTable();
	if (newIndex == NULL){
		fprintf(stdout, "Error: Out of memory.");
		fclose(fp);
		exit(1);
	}

	char *word = NULL;
	int docID;
	int occurrences;
	int numFiles;
	
	int start;
	int numChars;
	//While you haven't reached the end of the file
	while (!feof(fp)){

		//Get the starting position
		start = ftell(fp);
		//Check how long that word is
		fscanf(fp, "%*s%n", &numChars); 
		//Allocate memory for the word properly
		word = (char*)malloc(sizeof(char)*numChars+1);
		if (word == NULL){
			fprintf(stdout, "Error: Out of memory.");
			fclose(fp);
			free(newIndex);
			exit(1);	
		}
		//Get the previously recorded position
		fseek(fp, start - ftell(fp), SEEK_CUR);
		//Get the word and the number of file it appears in
		int n = fscanf(fp, "%s %d", word, &numFiles);
		if (n != 2){
			free(word);
			break;
		}
		//For each of these files
		int i;
		for(i=0; i<numFiles; i++){
			// Get the document id and the number of occurrences in that document
			int n = fscanf(fp, "%d %d", &docID, &occurrences);
			if (n != 2){
				free(word);
				break;
			}
			// For each of these occurrences, update the index
			int j;
			for (j=0; j < occurrences; j++){
				int returnCode = UpdateIndex(newIndex, word, docID);
				if (returnCode == 1){
					fclose(fp);
					fprintf(stdout, "Error: Out of memory");
					free(word);
					CleanDynamicList(newIndex);
					free(newIndex);
					exit(1);
				}
			}
		}
	}
	fclose(fp);

	return newIndex;
}




