/* ========================================================================== */
/* File: list.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * This file contains the definitions for a doubly-linked list of WebPages.
 *
 */
/* ========================================================================== */
#ifndef LIST_H
#define LIST_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"                          // common functionality
#include "hashtable.h"
#include "utils.h"

// ---------------- Constants

// ---------------- Structures/Types

typedef struct ListNode {
    WebPage *page;                           // the data for a given page
    struct ListNode *prev;                   // pointer to previous node
    struct ListNode *next;                   // pointer to next node
} ListNode;

typedef struct List {
    ListNode *head;                          // "beginning" of the list
    ListNode *tail;                          // "end" of the list
} List;

// ---------------- Public Variables

// ---------------- Prototypes/Macros

/*
 * initializeList - initializes the doubly linked list
 * 
 * returns the initialized doubly linked list
 */
List* initializeList();

/*
 * AppendList - appends a ListNode to the doubly linked list
 * @page - a pointer to a webpage that has been allocated and initialized
 * @urlList - a pointer to a  urllist that has has been allocated and initialized
 * 
 * Returns 0 upon successful completion
 * Returns 1 upon memory allocation failure
 */
int AppendList(WebPage *page, List *urlList);

/*
 * PopList - Removes a ListNode from the front of the list
 * @urlList - a pointer to a urllist that has been allocated and initialized
 * 
 * Returns a WebPage type
 */
WebPage *PopList(List *urlList);

#endif // LIST_H
