/*	dog.c	Concatenates files or text from standard input.

	No copyright

	No license

	Project name: dog.c
	Component name: N/A
	
	Primary Author:	Andrew Ogren
	Date Created: 01/26/2016

	Special considerations:  None
	
======================================================================*/
// ---------------- Pseudocode
/* Create a long option for --help
  Go through the swtiches provided on the command line and set them to 1 if they
  were used.
  For -m and -M, set the following argument to the mapfile.
  If there is no following argument, issue an appropriate error message and exit.
  If the switch is not an option, give error message and exit.
  If no files were provided to read, read from standard input.
  If a '-' was used, read from standard input.
  If the mapfile was set, check that it's of the appropriate form.

  In mapfileCheck, make sure the mapfile can be opened.
  If it can be opened, scan it line by line.
  Get the first two character in each line.
  If there are only two characters per line, 
  Check that each character is lowercase in the english alphabet.
  If they are, convert each to an integer between 0 and 25
  Use this for an index in each of the encryption and decryption arrays, and
  set each to the other character.
  Make sure there were only 26 lines in the file.
  Make sure that there were no duplicate charactesr in each of the columns e.g. it's one-to-one mapping.

  Read through the input (whether from a file or standard input)
  if -n switch is on, use the counter to print out line number and increment
  if -m switch is on but -M is off, encrypt the input using the encryption array
  if -M switch is on but -m is off, decrypt hte input using the decryption array
  if no switches are on or both the -m and -M switches are on, simply print out the text as is.*/
//
// ---------------- Open Issues 

// ---------------- System includes
#include <stdio.h> //Needed for input and output
#include <stdlib.h> //standard c functions
#include <string.h> //Needed for string manipulation
#include <getopt.h> //Needed for switch parsing
#include <ctype.h> //Needed for checking contents of mapfile

// ---------------- Local includes  e.g., "file.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions
#define MAX 200 //Gets 200 chars at a time (somewhat arbitrary)

// ---------------- Structures/Types 

// ---------------- Private variables 
static int numLines, encrypt, decrypt;
char encryption[26], decryption[26];
int counter= 1;

// ---------------- Private prototypes 
void readInput(FILE *fp, char *mapfile, int *counter);
void encryptFunc(char *buffer);
void decryptFunc(char *buffer);
int mapfileCheck(char *mapfile);

/*====================================================================*/ 


int main(int argc, char *argv[]){
	/*Initially set all of the switches off */
	numLines=encrypt=decrypt=0;
	char *mapfile;
	
	static struct option longOptions[] =
	{
		{ "help", no_argument, 0, 'h'}
	};
	
	char opt;
	/* Runs through the arguments looking for switches
	   The ':'s within the getopt_long function indicate that the switch requires an argument*/
	while ((opt = getopt_long(argc, argv, ":nm:M:", longOptions, NULL)) != -1) {
		
		switch (opt) {

			case 'n':
				numLines=1;
				break;
			case 'm':
				encrypt=1;
				mapfile=optarg; /*optarg is the argument directly after the switch */
				break;
			case 'M':
				decrypt=1;
				mapfile=optarg;
				break;
			case 'h':
				fprintf(stdout, "Usage for dog.c: \nConcatenates files or standard input to standard output or another file.\n\nSwitches:\n"
	  			"'-n': numbers the nonempty output lines\n'-m' mapfile: uses a cipher to encrypt input\n'-M' mapfile: "
	  			"decrypts input back to plaintext (english alphabet)\n'--help': display this "
	  			"help message and terminate\n\nWith no file argument, or when first argument is '-', read from standard input.\n"
	  			"This is version 1.0");
	  			return 0;
	  		case ':':
	  			fprintf(stderr, "Option \'%c\' requires an argument!", optopt);
	  			return 1;

	  		case '?':
			default:
				fprintf(stderr, "Error: bad option \'%c\' \n", optopt);
				fprintf(stderr, "If you need more information, try --help");
				return 1;
			}		
	}

	/* If the only argument is the program name, it will read from standard input */
	if ( (argc == 1) ) {
		readInput(stdin, mapfile, &counter);
	}
	/* The mapfile has been set if either the m or M switch was used, so check the mapfile*/
	if ( mapfile != NULL) {
		if (mapfileCheck(mapfile) != 0) {
			return 1; /*if mapfileCheck returns 1, terminate */
		}
	}
	int i;
	FILE *fp;
	/* optind holds the place where getopt left off (right after the last switch) */
	for(i= optind; i<argc ; i++){
		
		char *arg= argv[i];
		if (strcmp(arg, "-") != 0){
			if ( (fp=fopen(arg, "r")) == NULL){
				fprintf(stderr, "There was an error opening the file: %s", arg);
				return 1;
			}
			else {
				readInput(fp, mapfile, &counter);
			}
		}
		/* If the '-' was used, read from standard input */
		else {
			readInput(stdin, mapfile, &counter);
		}
	}
	/* This is a check to see if only switches were called, so it should read from stdin invoking those switches */
	if (argc == optind) {
		readInput(stdin, mapfile, &counter);
	}

	return 0;
}

/* readInput takes a pointer to a file a mapfile and a counter */
void readInput(FILE *fp, char *mapfile, int *counter) {
	char buffer[MAX];

	int lineCount= *counter;

	while ((fgets(buffer, sizeof(buffer), fp)) != '\0') {
		/* number the lines */
		if (numLines == 1) {
			fprintf(stdout, "%d ", lineCount);
			*counter = ++lineCount;  
		}
		/* encrypt */
		if ((encrypt == 1) && (decrypt == 0)) {
			encryptFunc(buffer);
		}
		/* decrypt */
		else if ((encrypt == 0) && (decrypt == 1)) {
			decryptFunc(buffer);
		}
		/* If no switches were used, or if both encrypt and decrypt were used */
		else {
			fprintf(stdout, "%s", buffer);
		}

	}
	fclose(fp);
}

void encryptFunc(char *buffer) {
	char c1, c2;
	int i;
	for (i=0; i < strlen(buffer); i++) {
		c1= buffer[i];
		/* if it's a character that the mapfile can encrypt */
		if (islower(c1)) {
			int c1Int= c1 - 'a'; 
			c2= encryption[c1Int];
			fprintf(stdout, "%c", c2);
		}
		else {
			fprintf(stdout, "%c", c1);
		}
	}
}

void decryptFunc(char *buffer) {
	char c1, c2;
	int i;
	for (i=0; i < strlen(buffer); i++) {
		c2= buffer[i];

		if (islower(c2)) {
			int c2Int= c2 - 'a';
			c1= decryption[c2Int];
			fprintf(stdout, "%c", c1);
		}
		else {
			fprintf(stdout, "%c", c2);
		}
	}
}

int mapfileCheck(char *mapfile) {
	FILE *map;
	if ((map = fopen(mapfile, "r")) == NULL) {
		fprintf(stderr, "There was an error opening the map file: %s", mapfile);
		return 1;
	}

	char c1, c2;
	int rv;
	int size= 0;
	/* Reads the mapfile line by line, and gets the first two characters in that line */
	while ((rv = fscanf(map, "%1c %1c\n", &c1, &c2)) != EOF ) {
		/* fscanf returns the number of input items successfully matched, so if it's 2, it's in correct format*/
		if (rv == 2) {
			/* Make sure both characters are lowercase letters */
			if (!islower(c1)){
				fprintf(stderr, "Error, invalid character \'%c\'", c1);
				fclose(map);
				return 1;
			}
			if (!islower(c2)){
				fprintf(stderr, "Error, invalid character \'%c\'", c2);
				fclose(map);
				return 1;
			}
			
			else {
				/* To use the characters as indices in the arrays, subtract the
				'lowest value' that it could be (in ASCII character values) such that a->0, b->1, etc. */
				int i= c1 - 'a';  
				int j= c2 - 'a';
				encryption[i]=c2; 
				decryption[j]=c1;
				size++;
			}
		}

		else {
			fprintf(stderr, "Error, invalid format in map file.");
			fclose(map);
			return 1;
		}

	}
	/* If 26 characters were added to each array, size should be 26 */
	if (size != 26) { 
		fprintf(stderr, "Error, map file is %d lines, and it needs to be 26", size);
		fclose(map);
		return 1;
	}
	/* To make sure that each column has no repeated chars, run through each array checking for duplicates */
	int count = sizeof(encryption)/sizeof(encryption[0]);
	int i;
	int j;
	for (i = 0; i < count; i++) {
	    for (j = i + 1; j < count; j++) {
	        if (encryption[i] == encryption[j]) {
	            fprintf(stderr, "There was a duplicate in the mapfile.");
	            fclose(map);
	            return 1;
	        }
	    }
	}

	for (i = 0; i < count; i++) {
		int j;
	    for (j = i + 1; j < count; j++) {
	        if (decryption[i] == decryption[j]) {
	            fprintf(stderr, "There was a duplicate in the mapfile.");
	            fclose(map);
	            return 1;
	        }
	    }
	}



	fclose(map);
	return 0;

}

