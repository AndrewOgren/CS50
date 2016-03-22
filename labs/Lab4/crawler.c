/* ========================================================================== */
/* File: crawler.c - Tiny Search Engine web crawler
 *
 * Author: Andrew Ogren
 * Date: 02/09/16
 *
 * Input: 3 Arguments
 * Arg 1: The url that you want to crawl (for this project, can only be "http://www.cs.dartmouth.edu/~cs50/tse/"
 * Arg 2: The directory that you want to put the output file in. It must have already been created and must be writeable.
 * Arg 3: The depth that you wish to crawl to. This depth must non-negative and less than 5.
 *
 * Command line options: None
 *
 * Output: This program outputs file to the provided directory. These files, labeled 1,2,3,etc, contain the url of
 * the page crawled, the depth at which it was crawled, and the html curled from that url.
 *
 * Error Conditions: The program exits if the arguments provided do not meet the requirements, if file are not able to
 * be opened, or if memory is not allocated properly.
 *
 * Special Considerations: None.
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdio.h>                           // printf

#include <curl/curl.h>                       // curl functionality
#include <dirent.h>                          // checking directory
#include <unistd.h>                          // check for write permissions
#include <string.h>                          // string manipulation
#include <sys/stat.h>                        // checking directory

// ---------------- Local includes  e.g., "file.h"
#include "common.h"                          // common functionality
#include "web.h"                             // curl and html functionality
#include "list.h"                            // webpage list functionality
#include "hashtable.h"                       // hashtable functionality
#include "utils.h"                           // utility stuffs

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

/* ========================================================================== */

int main(int argc, char* argv[])
{

    // check command line arguments
    // Make sure that there are the right number of arguments
    if (argc != 4) {
        fprintf(stdout, "You must supply 3 arguments.\n");
        printf("The first argument is the seed URL. \n The second argument is the target directory. \n"
            "The third argument is the max depth which to crawl.");
        exit(1);
    }
    // Make sure that the provided directory exists
    const char *dirName= argv[2];
    struct stat dir;
    if (stat(dirName, &dir) == -1) {
        fprintf(stdout, "This directory cannot be found.\n");
        printf("The directory must already exist, and the directory must be your"
            " second argument. ");
        exit(1);
    }
    // Make sure that the provided directory has write permissions
    if (access(dirName, W_OK) != 0){
        fprintf(stdout, "This directory does not have write access.\n");
        printf("The directory must have write access to save files there.");
        exit(1);
    }
    //  Make sure that the given depth is non-negative and less than 5
    int userDepth= atoi(argv[3]);
    if ( (userDepth > MAX_DEPTH) || (userDepth < 0) ) {
        fprintf(stdout, "The depth of the crawler cannot exceed 4 or be less than 0.\n");
        printf("The crawl depth should be your third parameter. \nIt should"
            " indicate how many levels of links that the crawler explores.");
        exit(1);
    }
    // Make sure that given url is correct (only allowed to crawl this domain)
    if (strcmp(argv[1], "http://www.cs.dartmouth.edu/~cs50/tse/") != 0) {
        fprintf(stdout, "For this crawler, the url must be: \n"
        "http://www.cs.dartmouth.edu/~cs50/tse/ \n");
        printf("The url should be your first parameter");
        exit(1);
    }
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);

    // Initialize hashtable
    HashTable *table = initializeHashTable();
    
    // Make sure memory was allocated properly
    if (table == NULL){
        fprintf(stdout, "\nError: Out of memory.");
        exit(1);
    }

    // Initialize urllist
    List *urlList = initializeList();
    // Make sure memory was allocated properly
    if (urlList == NULL) {
        fprintf(stdout, "\nError: Out of memory.");
        free(table);
        exit(1);
    }
    // Initialize a file count for file-naming purposes
    int fileCount=0;
    int *fc;
    fc= &fileCount;

    // setup seed page
    // Allocate memory for the seed page
    WebPage *webpage= (struct WebPage*)malloc(sizeof(WebPage));
    // Make sure memory was allocated properly
    if (webpage == NULL) {
        fprintf(stdout, "\nError: Out of memory.");
        free(urlList);
        free(table);
        exit(1);
    }
    // Normalize the url to make sure it's formatted correctly
    char *seedURL = argv[1];
    if (!NormalizeURL(seedURL)){
        fprintf(stdout, "\nError: Could not parse seed url.");
        free(urlList);
        free(table);
        free(webpage);
        exit(1);
    }
    // Properly allocate space for seed url string so that it
    // can be freed properly later
    char *allocatedSeedURL = calloc(strlen(seedURL)+1, sizeof(char));
    if (webpage == NULL) {
        fprintf(stdout, "\nError: Out of memory.");
        free(urlList);
        free(table);
        free(webpage);
        exit(1);
    }
    strcpy(allocatedSeedURL, seedURL);

    // Initialize webpage's members
    webpage->url= allocatedSeedURL;
    webpage->html=NULL;
    webpage->depth=0;

    // get seed webpage
    // try 3 times, and if it fails on 3rd time, exit
    int returnCode;
    int i;
    for (i=0; i<MAX_TRY; i++){
        returnCode=GetWebPage(webpage);
        // sleep(INTERVAL_PER_FETCH);
        if (returnCode == 1){
            break;
        }
    }
    // Retrieving seed url page failed, so exit
    if (returnCode == 0) {
        fprintf(stderr, "\nThe webpage '%s' could not be found after 3 tries \n", webpage->url);
        free(webpage->url);
        free(webpage->html);
        free(webpage);
        free(urlList);
        free(table);
        exit(1);
    }
    else {
        (*fc)++;
    }
    
    // write seed file to given directory
    int saveReturnCode = SaveWebPage(webpage, fc, argv[2]);
    // A return code of 1 indicates that there was a memory allocation error
    if (saveReturnCode == 1) {
        fprintf(stdout, "\nError: Out of memory.");
        free(webpage->url);
        free(webpage->html);
        free(webpage);
        free(urlList);
        free(table);
        exit(1);
    }
    // A return code of 2 indicates that there was an issue opening the file
    // that is being written to.
    else if (saveReturnCode == 2) {
        fprintf(stdout, "\nError: Cannot open file.");
        free(webpage->url);
        free(webpage->html);
        free(webpage);
        free(urlList);
        free(table);
        exit(1);
    }
    // If the given depth is 0, the program is done, so free memory and return
    if (userDepth == 0) {
        free(webpage->url);
        free(webpage->html);
        free(webpage);
        free(urlList);
        free(table);
        return 0;
    }

    // add seed page to hashtable
    if (HashTableLookup(webpage->url, table) == 2) {
        fprintf(stdout, "\nError: Out of memory.");
        free(webpage->url);
        free(webpage->html);
        free(webpage);
        free(urlList);
        free(table);
        exit(1);
    }

    // extract urls from seed page
    char *html= webpage->html;
    int pos = 0;
    char *result=NULL;
    char *base_url = webpage->url;
    fprintf(stdout, "Crawling - %s \n", base_url);
    while(( pos = GetNextURL(html, pos, base_url, &result)) >0){
        // Make sure GetNextURL returned something
        if (result == NULL){
            continue;
        }
        // Normalize the returned url
        if (!NormalizeURL(result)){
            free(result);
            continue;
        }
        // Compare to base domain name to make sure it stays on old-dart-cs domain
        int baseLength = strlen(URL_PREFIX);
        if ((strncmp(result, URL_PREFIX, baseLength)) != 0) {
            free(result);
            continue;
        }
        else {
            WebPage *newPage;
            newPage = malloc(sizeof(WebPage));
            // Make sure memory was properly allocated
            if (newPage == NULL) {
                fprintf(stdout, "\nError: Out of memory.");
                free(webpage->url);
                free(webpage->html);
                free(webpage);
                free(urlList);
                FreeTableNodes(table);
                free(table);
                exit(1);
            }
            // Append the new page to the list
            newPage->url = result;
            fprintf(stdout, "Parser found link - %s \n", newPage->url);
            newPage->html = NULL;
            newPage->depth = 1;
            if (AppendList(newPage, urlList) == 1){
                fprintf(stdout, "\nError: Out of memory.");
                free(newPage->url);
                free(newPage->html);
                free(newPage);
                free(webpage->url);
                free(webpage->html);
                free(webpage);
                free(urlList);
                FreeTableNodes(table);
                free(table);
                exit(1);
            }        
        }
    }
    // The webpage that will be crawled next
    WebPage *nextPage;
    
    int depth;
    // while there are urls to crawl (the urlList is not empty)
    while((urlList->head) != NULL){
        // get next url from list
        nextPage = PopList(urlList);
        // make sure it's not null. If it is, move on to the next one.
        if (nextPage == NULL){
            continue;
        }
        // Get the depth of this page
        depth = nextPage->depth;
        // Make sure it's within correct domain. If it is, move on to the next one.
        char* url = nextPage->url;

        if (url == NULL){
            free(nextPage->html);
            free(nextPage);
        }
        int baseLength = strlen(URL_PREFIX);
        if ((strncmp(url, URL_PREFIX, baseLength)) != 0) {
            free(nextPage->url);
            free(nextPage->html);
            free(nextPage);
            continue;
        }
        // See if it's already been visited.
        int visited = HashTableLookup(url, table);
        // A return code of 2 indicates that there was a memory alloc failure within the function
        if (visited == 2){
            fprintf(stdout, "\nError: Out of memory.");
            free(webpage->url);
            free(webpage->html);
            free(webpage);
            free(nextPage->url);
            free(nextPage->html);
            free(nextPage);
            free(urlList);
            FreeTableNodes(table);
            free(table);
            exit(1);
        }
        // A return code of 1 indicates that the url has been visited, so skip it.
        if (visited == 1) {
            free(nextPage->url);
            free(nextPage->html);
            free(nextPage);
            continue;
        }

        // get webpage for url
        // try 3 times to retrieve the html
        int i;
        for (i=0; i<MAX_TRY; i++){
            returnCode=GetWebPage(nextPage);
            // sleep(INTERVAL_PER_FETCH);
            if (returnCode == 1){
                break;
            }
        }
        // If it fails after 3 tries, give up, and move on to the next url.
        if (returnCode == 0) {
            fprintf(stderr, "\nThe webpage '%s' could not be found after 3 tries \n", nextPage->url);
            free(nextPage);
            continue;
        }
        else {
            (*fc)++;
            // Increment the file number and save it.
            saveReturnCode= SaveWebPage(nextPage, fc, argv[2]);
            // Return code of 1 indicates a memory alloc failure.
            if (saveReturnCode == 1) {
                fprintf(stdout, "\nError: Out of memory.");
                free(webpage->url);
                free(webpage->html);
                free(webpage);
                free(nextPage->url);
                free(nextPage->html);
                free(nextPage);
                free(urlList);
                FreeTableNodes(table);
                free(table);
                exit(1);
            }
            // Return code of 2 indicates a file opening error.
            else if (saveReturnCode == 2) {
                fprintf(stdout, "\nError: Cannot open file.");
                free(webpage->url);
                free(webpage->html);
                free(webpage);
                free(nextPage->url);
                free(nextPage->html);
                free(nextPage);
                free(urlList);
                FreeTableNodes(table);
                free(table);
                exit(1);
            }
        }
        // Make sure that the depth is appropriate to get the urls on its page
        if (depth < userDepth) {
            //extract urls from webpage
            pos= 0;
            char *html= nextPage->html;
            result=NULL;
            base_url= url;
            fprintf(stdout, "Crawling - %s \n\n", base_url);

            while(( pos = GetNextURL(html, pos, base_url, &result)) >0){
                if(result == NULL){
                    continue;
                }
                if(!NormalizeURL(result)){
                    free(result);
                    continue;
                }
                int baseLength = strlen(URL_PREFIX);
                if ((strncmp(result, URL_PREFIX, baseLength)) != 0) {
                    free(result);
                    continue;
                }
                else {
                    WebPage *newPage= (struct WebPage*)malloc(sizeof(WebPage));
                    // Make sure there wasn't a memory allocation failure
                    if (newPage == NULL) {
                        fprintf(stdout, "\nError: Out of memory.");
                        free(webpage->url);
                        free(webpage->html);
                        free(webpage);
                        free(urlList);
                        FreeTableNodes(table);
                        free(table);
                        exit(1);
                    }
                    // Initialize members of newPage   
                    newPage->url = result;
                    fprintf(stdout, "Parser found link - %s \n", newPage->url);
                    newPage->html = NULL;
                    newPage->depth = (depth + 1);
                    // Add the page to the list
                    if (AppendList(newPage, urlList) == 1) {
                        fprintf(stdout, "\nError: Out of memory.");
                        free(newPage->url);
                        free(newPage->html);
                        free(newPage);
                        free(nextPage->url);
                        free(nextPage->html);
                        free(nextPage);
                        free(webpage->url);
                        free(webpage->html);
                        free(webpage);
                        free(urlList);
                        FreeTableNodes(table);
                        free(table);
                        exit(1);
                    }
                    
                }
            }
        }
        //Free what is no longer needed
        free(nextPage->html);
        free(nextPage);
    }
    //Free remaining allocated memory
    free(webpage->html);
    free(webpage);
    free(urlList);
    FreeTableNodes(table);
    free(table);

    // cleanup curl
    curl_global_cleanup();

    return 0;
}

