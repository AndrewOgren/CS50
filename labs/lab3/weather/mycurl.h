/*  mycurl.h  utility functons for use with libcurl
 *
 * Copyright : None - code extracted/modified from libcurl examples
 *
 * Project name:    CS50 Lab assignment 3
 * Component name:  weather
 *
 * Usage:       defines the string struct
 * 				defines two functions:
 *	              init_string allocates and initializes a curl string structure.
 *  	          writefunc is a callback used by curl to store the information returned into a specified buffer.
 *
 * Date Created:    March 2014
 *
 * Special considerations:
 *  Links with calling application and libcurl.a
 */
#ifndef _mycurl_h_
#define _mycurl_h_

#define   MAXTAG        50
#define   KEYNOTFOUND   "<!DOCTYPE"

struct curlResponse {
  char *ptr;
  size_t len;
};

void init_curlResponse(struct curlResponse *s) ;

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct curlResponse *s);

#endif
