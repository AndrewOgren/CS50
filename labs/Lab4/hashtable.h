/* ========================================================================== */
/* File: hashtable.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * This file contains the definitions for a hashtable of urls.
 *
 */
/* ========================================================================== */
#ifndef HASHTABLE_H
#define HASHTABLE_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"                          // common functionality

// ---------------- Constants
#define MAX_HASH_SLOT 5000                 // number of "buckets"

// ---------------- Structures/Types

typedef struct HashTableNode {
    char *url;                               // url previously seen
    struct HashTableNode *next;              // pointer to next node
} HashTableNode;

typedef struct HashTable {
    HashTableNode *table[MAX_HASH_SLOT];     // actual hashtable
} HashTable;

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/*
 * jenkins_hash - Bob Jenkins' one_at_a_time hash function
 * @str: char buffer to hash
 * @mod: desired hash modulus
 *
 * Returns hash(str) % mod. Depends on str being null terminated.
 * Implementation details can be found at:
 *     http://www.burtleburtle.net/bob/hash/doobs.html
 */
unsigned long JenkinsHash(const char *str, unsigned long mod);

/*
 * HashTableLookup - Hash into the table and find if the url already has been visited
 * 
 * @URL - url to compare
 * @tableptr - the pointer the hashtable
 * 
 * Returns 0 if successful
 * Returns 1 if url is already in the hashtable
 * Returns 2 if memory allocation failed
 */
int HashTableLookup(char *URL, HashTable *tableptr);

/*
 * @URL - url to add to HashTableNode
 * @index - the place to hash into the table
 * @tableptr - the pointer to the hashtable
 *
 * Returns 0 if successful
 * Returns 2 if memory allocation failed
 */
int HashTableInsert(char *URL, int index, HashTable *tableptr);

/*
 * initializeHashTable - initializes a hash table type
 *
 * Returns a pointer to a hashtable type
 */

HashTable* initializeHashTable();

/*
 * FreeTableNodes - Used to free the allocated memory of all of the HashTableNodes that were allocated
 * 
 * @hashtable - the pointer to a hashtable
 * returns nothing
 */
void FreeTableNodes(HashTable *hashtable);

#endif // HASHTABLE_H
