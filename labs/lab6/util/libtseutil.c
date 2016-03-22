/* ========================================================================== */
/* File: libtseutil.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * Author: Andrew Ogren
 * Date: 02/14/16
 *
 * You should include in this file your functionality for the hashtable as
 * described in the assignment and lecture.
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <string.h>                         // strlen
#include <stdlib.h>
#include <stdio.h>

// ---------------- Local includes  e.g., "file.h"
#include "libtseutil.h"                       // hashtable functionality


// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
int testing = 0;
// ---------------- Private prototypes

unsigned long JenkinsHash(const char *str, unsigned long mod)
{
    size_t len = strlen(str);
    unsigned long hash, i;

    for(hash = i = 0; i < len; ++i)
    {
        hash += str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % mod;
}
/*
 * TestingIndex - turn on testing
 * Assumptions - none
 * Psuedocode:
 * set the private variable 'testing' to 1
 * so that words are not freed that were not allocated
 */
void TestingIndex(){
    testing=1;
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
 * initializeHashTable - initializes a hash table type
 * 
 * Assumptions - none
 * Pseudocode:
 * Allocate memory for a HashTable type
 * Run through all of the slots of the table array, and set them to NULL initially
 * Return the Hashtable
 */

GenHashTable* InitializeHashTable() {
    GenHashTable *hashtable= (struct GenHashTable*)calloc(1, sizeof(GenHashTable));  
    int node;
    for (node=0; node<MAX_HASH_SLOT; node++){
        hashtable->table[node]= NULL;
    }
    return hashtable;
}

/*
 * LookupWord - Determines if the word has been added to the hashtable already
 * @key - the word
 * @tableptr - a pointer to the index table
 *
 * Assumptions: table is already allocated and the word is not null
 *
 * Pseudocode: 
 * Hash the key to get the index
 * if the table is empty at that index, the word has not been added
 * if the table is not empty at that index
 * while there are word nodes, compare the words to the given word
 * If there is a match, the word has already been added
 * When there are no nodes left, this means the word has not been added
 *
 */
int LookupWord(char *key, GenHashTable *tableptr) {
    unsigned long modulo= MAX_HASH_SLOT;
    unsigned long index= JenkinsHash(key, modulo);
    // If the table[index] is empty
    if ((tableptr->table[index]) == NULL) {
        return 0; //Not added yet
    }

    else {
        WordNode *current = tableptr->table[index];
        while(current != NULL) {
            if(strcmp(current->word, key) == 0) {
                return 1; //added already
            }
            current = current->next;
        }
    }
    return 0; //Not added yet
}

/*
 * NewNodeInsert - Adds a word node and a doc node 
 * @key - the word to be added
 * @docID - the id of the document in which the word is found
 * @tableptr - a pointer to the index table
 *
 * Assumptions: index has been allcoated
 *
 * Pseudocode:
 * Allocate memory for a wordnode
 * Allocate memory for a document node
 * Initialize the members of the document node and word node appropriately
 * Get the location in the index table
 * If it is empty, add it there
 * If it's not empty, go through the linked list until you hit a NULL pointer to next
 * Add the word node there
 */
int NewNodeInsert(char *key, int docID, GenHashTable *tableptr){
    //Allocate memory appropriately
    WordNode *newNode;
    newNode= (struct WordNode*)malloc(sizeof(WordNode));

    if (newNode == NULL) {
        return 1;
    }
    //Allocate memory appropriately
    DocNode *newDocNode;
    newDocNode= (struct DocNode*)malloc(sizeof(DocNode));

    if (newDocNode == NULL) {
        free(newNode);
        return 1;
    }
    //Initialize members of word node
    newNode->word= key;
    newNode->doc= newDocNode;
    newNode->next= NULL;
    //Initialize members of doc node
    newDocNode->docID= docID;
    newDocNode->occurrences= 1;
    newDocNode->next= NULL;

    unsigned long modulo= MAX_HASH_SLOT;
    unsigned long index= JenkinsHash(key, modulo);
    //If there is no word in the slot
    if (tableptr->table[index] == NULL) {
        tableptr->table[index] = newNode;
    }
    //Go through the linked list until there are no more word nodes
    else {
        WordNode *current= tableptr->table[index];
        while(current != NULL) {
            if ((current->next) == NULL) {
                break;
            }
            current = current->next;
        }
        current->next = newNode;
    }

    return 0;
}

/*
 * DocNodeInsert - When the word has already been added, add a doc node or increment doc node
 * @key - the word
 * @docID - the id of the document in which the word is found
 * @tableptr - a pointer to the index table
 *
 * Assumptions: the index and key have been allocated properly
 *
 * Pseudocode:
 * Hash the key to get the proper index
 * Find the point in the linked list at which the word node is saved
 * At this point, travel down the linked list of document nodes until
 * either it finds a document with the same docID as the current one
 * or it reaches the end of the document linked list
 * if the document already exists, increment its occurrences and return 0
 * if the document doesn't exist, create a document node, allocate its members
 * and save it at the end of the document linked list
 *
 */
int DocNodeInsert(char *key, int docID, GenHashTable *tableptr) {
    //hash the word
    unsigned long modulo= MAX_HASH_SLOT;
    unsigned long index= JenkinsHash(key, modulo);
    //Get the stop in teh table where the word node exists
    WordNode *current= tableptr->table[index];
    while (current != NULL) {
        if (strcmp(current->word, key) == 0) {
            break;
        }
        else {
            current = current->next;
        }

    } 
    DocNode *currentDoc = current->doc;
    //Go through all of the document nodes
    while (currentDoc != NULL) {
        //If one of the document nodes matches the docId, increment its occurrences
        if ((currentDoc->docID) == docID) {
            currentDoc->occurrences++;
            if (testing == 0){
                //Only free the key if you're not testing
                free(key);
            }
            return 0;
        }
        if ((currentDoc->next) == NULL) {
            break;
        }
        else {
            currentDoc = currentDoc->next;
        }
    }
    //Allocate memory for a new document node
    DocNode *newDocNode;
    newDocNode= (struct DocNode*)malloc(sizeof(DocNode));

    if (newDocNode == NULL) {
        return 1;
    }
    //Initialize its members
    newDocNode->docID= docID;
    newDocNode->occurrences= 1;
    newDocNode->next= NULL;
    //Append it to the linked list of document nodes for that word
    currentDoc->next = newDocNode;
    
    if (testing == 0){
        free(key);
    }

    return 0;
}

/*
 * CleanDynamicList - frees the remaining allocated memory in the index table
 * @tableptr - a pointer to the index table
 *
 * Assumptions - None
 * 
 * Pseudocode:
 * For each slot in the table
 * If the slot is NULL, just empty that slot
 * Else, go through the word nodes in that slot
 * For each word node, go through its document nodes
 * For each document node, free it
 * For each word node, free its word and the node itself
 */
void CleanDynamicList(GenHashTable *tableptr){
    int node;
    for(node=0; node<MAX_HASH_SLOT; node++){
        if(tableptr->table[node] == NULL){
            free(tableptr->table[node]);
        }

        else {
            WordNode *currentNode = tableptr->table[node];
            WordNode *nextNode;
            while(currentNode != NULL) {
                nextNode = currentNode->next;

                DocNode *currentDoc = currentNode->doc;
                DocNode *nextDoc;
                while(currentDoc != NULL){
                    nextDoc = currentDoc->next;
                    free(currentDoc);
                    currentDoc = nextDoc;
                }
                
                free(currentNode->word);
                free(currentNode);
                currentNode = nextNode;

            }
        }
    }
}

/*
 * SearchIndex - searches the index for the given word and returns a list of documents it's in
 *
 * @word - the word to search the index for
 * @index - the index to search for the word in 
 *
 * Assumptions - The index has already been rebuilt using ReadFile
 *
 * Pseudo Code:
 * Find if the word exists in the index
 * If the word does exist
 *  Find the word node in the index
 *  Figure out how many document nodes there are for that word node
 *  Fill an array with all of those document ids
 *  return that document array
 * If the word does not exist
 *  return NULL
 */
int* SearchIndex(char* word, GenHashTable *index) {
    //See if the word exists
    int wordExists = LookupWord(word, index);

    if (wordExists == 1){
        unsigned long modulo= MAX_HASH_SLOT;
        unsigned long hashNum= JenkinsHash(word, modulo);
        int numFiles=0;
        //Get the position of the word
        WordNode *currentWord= index->table[hashNum];
        while(currentWord != NULL) {
            if(strcmp(currentWord->word, word) == 0) {
                break;
            }
            currentWord = currentWord->next;
        } //Count the number of documents that there are for this word
        DocNode *currentDocNode = currentWord->doc;
        DocNode *counterDocNode = currentWord->doc;
        while( (counterDocNode) != NULL) {
            numFiles++;
            counterDocNode= counterDocNode->next;
        }

        int *docArray;
        docArray = (int*)calloc(numFiles+1,sizeof(int));
        
        docArray[0] = numFiles;
        //for each of these documents, add their docids to the docArray
        for(int i=1; i<(numFiles+1); i++){
            docArray[i]= currentDocNode->docID;
            if((currentDocNode->next) != NULL) {
                currentDocNode= currentDocNode->next;
            }
            else {
                break;
            }
        }

        return (docArray);
    
    }

    else {
        return NULL;
    }


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
        if (numChars == 0){
            fprintf(stdout, "Error: file is not properly formatted.");
            exit(1);
        }
        //Allocate memory for the word properly
        word = (char*)malloc(sizeof(char)*numChars+1);
        if (word == NULL){
            fprintf(stdout, "Error: Out of memory or word is NULL.");
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
                    fprintf(stdout, "Error: could not update index.");
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
