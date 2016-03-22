/* ========================================================================== */
/* File: hashtable.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * Author: Andrew Ogren
 * Date: 02/07/16
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
#include "common.h"                          // common functionality
#include "hashtable.h"                       // hashtable functionality
#include "utils.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

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
 * initializeHashTable - initializes a hash table type
 * 
 * Assumptions - none
 * Pseudocode:
 * Allcoate memory for a HashTable type
 * Run through all of the slots of the table array, and set them to NULL initially
 * Return the Hashtable
 */

HashTable* initializeHashTable() {
    HashTable *hashtable= (struct HashTable*)calloc(1, sizeof(HashTable));  
    int node;
    for (node=0; node<MAX_HASH_SLOT; node++){
        hashtable->table[node]= NULL;
    }
    return hashtable;
}

/*
 * HashTableLookup - Hash into the table and find if the url already has been visited
 * 
 * Assumptions: The URL is a valid url string
 * the tableptr points to the beginning of the hashtable in memory
 * 
 * Pseudocode:
 * Set the modulo to whatever the number of hash slots is
 * Use JenkinsHash to create an index
 * If the index in the array is still empty (NULL), simply insert the url 
 * because the url must be unique (not visited yet)
 * If the index has a url, keep going until the node's next pointer is null, and
 * compare the urls each time to see if the url has already been visited.
 * If it hasn't, insert it.
 */

int HashTableLookup(char *URL, HashTable *tableptr) {
    unsigned long modulo= MAX_HASH_SLOT;
    unsigned long index= JenkinsHash(URL, modulo);
    // If the table[index] is empty
    if ((tableptr->table[index]) == NULL) {
        if (HashTableInsert(URL, index, tableptr) == 2) {
            return 2;
        }
        return 0;
    }
    // If there is a node already there, travel down the list
    else {
        HashTableNode *currentNode = tableptr->table[index];
        while ((currentNode) != NULL) {
            // Compare each node's url with provided url
            if (strcmp(currentNode->url, URL) == 0) {
                return 1;
            }
            else {
                currentNode= currentNode->next; 
            }
        }
    }
    if (HashTableInsert(URL, index, tableptr) == 2) {
        return 2;
    }
    return 0;
}

/*
 * HashTableInsert - Inserts a HashTableNode into the hashtable
 *
 * Assumptions - URL and tableptr are allocated and initialized
 * index is an integer that was found from Jenkin's hash
 * 
 * Pseudocode:
 * Create and allocate memory for a new hashtablenode
 * If the allocation fails, return 2
 * Set the members of the hashtablenode
 * If the table is empty at that index, insert it right into the table
 * If there is already a node at that index, travel down the linked list
 * until the next pointer is null, and insert it there.
 * return 0 upon success
 */
int HashTableInsert(char *URL, int index, HashTable *tableptr) {
    HashTableNode *newNode;
    newNode= (struct HashTableNode*)malloc(sizeof(HashTableNode));
    // Make sure memory was allocated properly
    if (newNode == NULL){
        return 2;
    }
    // set members
    newNode->url= URL;
    newNode->next = NULL;
    // if index is empty in table
    if ((tableptr->table[index]) == NULL) {
        tableptr->table[index] = newNode;
    }

    else {
        HashTableNode *currentNode = tableptr->table[index];
        while ((currentNode) != NULL){
            
            if ((currentNode->next) == NULL){
                currentNode->next = newNode;
                break;
            }
            else {
                currentNode = currentNode->next;
            }
        }
    }
    return 0;
}

/*
 * FreeTableNodes - Used to free the allocated memory of all of the HashTableNodes that were allocated
 * 
 * Assumptions - tableptr is allocated and initialized
 * 
 * Pseudocode: 
 * Go through all of the nodes in the table
 * If the index in the table is still null, simply free that index
 * If there is a HashTableNode at that index, free the node
 * and then move along the list until the next pointer is null
 */
void FreeTableNodes(HashTable *tableptr){
    
    int node;
    for(node=0; node<MAX_HASH_SLOT; node++){
        
        if (tableptr->table[node] == NULL) {
            free(tableptr->table[node]);
        }
        else {
            HashTableNode *currentNode = tableptr->table[node];
            HashTableNode *nextNode;
            while(currentNode != NULL){
                nextNode = currentNode->next;
                free(currentNode->url);
                free(currentNode);
                currentNode = nextNode;
            }

        }
    }
}



