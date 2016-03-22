/* ========================================================================== */
/* File: libtseutil.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * 
 *
 */
/* ========================================================================== */
#ifndef LIBTSEUTIL_H
#define LIBTSEUTIL_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants
#define MAX_HASH_SLOT 10000                 // number of "buckets"

// ---------------- Structures/Types
typedef struct WordNode {             // A word node for storing the word
	struct WordNode *next;
	char *word;
	struct DocNode *doc;
} WordNode;

typedef struct DocNode {			// A doc node for storing the doc id and # of occurrences
	struct DocNode *next;
	int docID;
	int occurrences;
} DocNode;

typedef struct GenHashTable {
    WordNode *table[MAX_HASH_SLOT];     // actual hashtable
} GenHashTable;

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
 * LookupWord - Determines if the word has been added to the hashtable already
 * @key - the word
 * @tableptr - a pointer to the index table
 *
 * Returns 0 if the word has not been added, returns 1 if it has been added already
 */
int LookupWord(char *key, GenHashTable *tableptr);
/*
 * NewNodeInsert - Adds a word node and a doc node
 * @key - the word to be added
 * @docID - the id of the document in which the word is found
 * @tableptr - a pointer to the index table
 *
 * Returns 0 upon success, or 1 upon a memory allocation failure
 */
int NewNodeInsert(char *key, int docID, GenHashTable *tableptr);

/*
 * DocNodeInsert - When the word has already been added, add a doc node or increment doc node
 * @key - the word
 * @docID - the id of the document in which the word is found
 * @tableptr - a pointer to the index table
 *
 * Returns 0 upon success, or 1 upon a memory allocation failure
 */
int DocNodeInsert(char *key, int docID, GenHashTable *tableptr);

/*
 * CleanDynamicList - frees the remaining allocated memory in the index table
 * @tableptr - a pointer to the index table
 *
 * Returns void
 */
void CleanDynamicList(GenHashTable *tableptr);

/*
 * TestingIndex - turn on testing
 *
 * Returns void
 */
void TestingIndex();

/*
 * initializeHashTable - initializes a hash table type
 *
 * Returns a pointer to a hashtable type
 */
GenHashTable* InitializeHashTable();

/*
 * UpdateIndex - updates the index accordingly
 *
 * @index - the index
 * @word - the word that will be saved in the index
 * @documentId - the unique id of the file
 *
 * Returns 0 if successful, but 1 if there is a memory allocation failure
 */
int UpdateIndex(GenHashTable *index, char *word, int documentId);

/*
 * ReadFile - Recreates the index from the given file
 *
 * @file - the file that contains the inverted index
 *
 * Returns the rebuilt index
 */
GenHashTable *ReadFile(char *file);

/*
 * SearchIndex - searches the index for the given word and returns a list of documents it's in
 *
 * @word - the word to search the index for
 * @index - the index to search for the word in 
 *
 * Returns NULL if the word does not exist in the index; if it does, it returns all of docIDs of the docs that it appears in
 */
int* SearchIndex(char* word, GenHashTable *index);


#endif // LIBTSEUTIL_H