/* ========================================================================== */
/* File: file.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Indexer
 *
 * Author: Ira Ray Jenkins - jenkins@cs.dartmouth.edu
 * Date: Tue Apr 29, 2014
 *
 * These functions are provided for the students to simplify the manipulation
 * of files and directories.
 */
/* ========================================================================== */
#define _DEFAULT_SOURCE                         // scandir, 
                                             // students shouldn't take advantage
                                             // of the bsd extensions, but
                                             // it makes it easier to test file
                                             // types

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <stdlib.h>                          // free, calloc
#include <string.h>                          // strlen, strcpy
#include <dirent.h>                          // scandir, alphasort

#include <sys/stat.h>                        // stat functionality
#include <stdio.h>

// ---------------- Local includes  e.g., "file.h"
#include "file.h"                             // file/dir functionality

// ---------------- Constant definitions

// ---------------- Macro definitions
#define MAX 200

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes
static int SelectFile(const struct dirent *entry);

// ---------------- Public functions

int IsDir(const char *path)
{
    struct stat statbuf;                     // stat buffer

    // make sure we have a path
    if(!path) return 0;

    // if we can stat it, and it's a directory
    return (!stat(path, &statbuf) && S_ISDIR(statbuf.st_mode));
}

int IsFile(const char *path)
{
    struct stat statbuf;                     // stat buffer

    // make sure we have a path
    if(!path) return 0;

    // if we can stat it, and it's a regular file
    return (!stat(path, &statbuf) && S_ISREG(statbuf.st_mode));
}

int GetFilenamesInDir(const char *dir, char ***filenames)
{
    struct dirent **namelist;                // list of dir entries from scandir
    int num_ents;                            // number of regular files found
    int status;                              // return status

    // make sure we have a directory path to work with
    if(!dir) { return -1; }

    // scan the directory for regular files
    status = num_ents = scandir(dir, &namelist, SelectFile, alphasort);
    if(num_ents < 0) {                       // something went wrong scanning
        return -1;
    }

    // allocate the filenames array
    *filenames = calloc(num_ents, sizeof(char*));
    if(!*filenames) {
        status = -1;
        goto cleanup;                        // sorry Dijkstra
    }

    // copy all the filenames and free the dirents
    for(int i = 0; i < num_ents; i++) {
        // allocate a filename large enough to contain the name
        (*filenames)[i] = calloc(strlen(namelist[i]->d_name) + 1, sizeof(char));
        if(!(*filenames)[i]) {
            status = -1;
            goto cleanup;                    // sorry Dijkstra
        }

        // copy the filename
        strcpy((*filenames)[i], namelist[i]->d_name);
    }

cleanup:
    if(namelist) {
        for(int i = 0; i < num_ents; i++) {
            if(namelist[i])
                free(namelist[i]);
        }
        free(namelist);
    }

    return status;
}

/*
 * SelectFile - test if a directory entry is a file
 * @entry: directory entry to test
 *
 * Returns non-zero if the directory entry is a regular file; otherwise, 0.
 *
 * Known issues:
 * This function uses the BSD dirent extensions: d_type, DT_REG, and DT_UNKNOWN.
 * Posix doesn't promise that dirent will contain a type, but BSD does. Using
 * these extensions avoids a separate call to stat.
 * Example:
 *    return(entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN);
 *
 * If this function is not working properly, you can replace the return
 * statement with a simpler test, for example to filter out "." and ".." dirs:
 * Example:
 *    return(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."));
 *
 * If you do not want to filter out *any* entries, then simply use:
 * Example:
 *    return(1);
 *
 * Should have no use outside of this file, thus declared static.
 */
static int SelectFile(const struct dirent *entry)
{
    return(entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN);
}

/*
 * LoadDocument - Gets a file from the directory and loads the html
 * @directory - the directory that holds the html files
 * @fileName - the name of the html file to read
 *
 * Assumptions: The directory and filename are both valid
 *
 * Psuedocode: 
 * Append the filename to the directory to form the complete path to the file
 * Open that file for reading using the full path name
 * Make sure it's not null
 * Use fseek and ftell to record length of the html file
 * allocate memory for the html string appropriately
 * Get rid of the first two lines that were added to the html in crawler
 * Read the rest of the html into the htmlstring
 * Close the file and return the htmlstring
 */
char *LoadDocument(const char *directory, char *fileName) {
    //Create the full path name
    char *pathName = (char*)malloc(sizeof(char)*(strlen(directory)+strlen(fileName))+1);
    sprintf(pathName, "%s%s", directory, fileName);
    // Open the file
    FILE *fp;
    fp = fopen(pathName, "r");

    if (fp == NULL) {
        fprintf(stdout, "The file could not be opened for reading.");
        exit(1);
    }
    //Get the length of the file
    int fileLength;
    fseek(fp, 0, SEEK_END);
    fileLength = ftell(fp);
    rewind(fp);
    //Allocate memory for the string based on that length
    char *htmlString;
    htmlString = (char*)calloc(fileLength+1, sizeof(char));
    
    if (htmlString == NULL) {
        fclose(fp);
        free(pathName);
        return NULL;
    }
    //Read past the first two lines of the html file
    int line = 0;
    char c;
    while (line < 2) {
        c = fgetc(fp);
        if (c == '\n') {
            line++;
        }
        if (c == EOF) {
            free(pathName);
            free(htmlString);
            fclose(fp);
            return NULL;
        }
    }
    //Read the read into the htmlstring
    fread(htmlString, sizeof(char), fileLength+1, fp);

    free(pathName);
    fclose(fp);
    //Return the htmlString
    return htmlString;
}

/*
 * GetDocumentId - Creates a document id for the file based on its name
 *
 * @fileName - the name of the file to create the id for
 *
 * Assumptions - that file exists
 *
 * Pseudocode: 
 * Convert the filename to an integer
 * If it returns 0, then the conversion failed, so return a negative integer to indicate this
 * return the docId
 */
int GetDocumentId(char *fileName) {

    int docID = atoi(fileName);

    if (docID == 0) {
        return -1;
    }
    
    return docID;
}
