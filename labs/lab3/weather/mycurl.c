/*  mycurl.c  utility functons for use with libcurl
 *
 * Copyright : None - code extracted/modified from libcurl examples
 *
 * Project name:    CS50 Lab assignment 3
 * Component name:  weather
 *
 * Usage:       
 *              init_string allocates and initializes a curl string structure.
 *              writefunc is a callback used by curl to store the information returned into a specified buffer.
 *
 * Date Created:    March 2014
 *
 * Special considerations:
 *  Links with calling application and libcurl.a
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "mycurl.h"


/*
 * init_curlResponse - initializes a curlResponse struct, including memory allocation
 * 
 * Returns:  If s->ptr is NULL, the operation failed.  
 *          Otherwise, s->ptr points to new memory and the first byte is NULL.
 */

void init_curlResponse(struct curlResponse *s)
{
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

/*
 * writefunc -  is called by libcurl to receive data from the curl call.
 *              this is typically HTML or XML.
 * Returns:     If successful it returns the size in bytes of the received data
 *              Otherwise, it returns zero;
 *
 */

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct curlResponse *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    return 0;
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}
