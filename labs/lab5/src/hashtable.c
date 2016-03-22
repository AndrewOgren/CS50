/* ========================================================================== */
/* File: hashtable.c
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

// ---------------- Local includes  e.g., "file.h"
#include "hashtable.h"                       // hashtable functionality


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

