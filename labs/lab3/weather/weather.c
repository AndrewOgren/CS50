/*  weather.c - Requests a weather report from NOAA's weather.gov site for a specified location

  No copyright

  No license

  Project name: weather.c
  Component name: none
  
  Primary Author: Andrew Ogren
  Date Created: 01/24/16

  Special considerations:  
  This code was adapted from libcurl's website for cs50 

  
======================================================================*/
//Pseudocode:
/* Get the provided location code supplied as an argument
   Setup curl stuff
   If there is more than one argument provided, give error message and exit
   If the argument provided is not 4 characters, give error message and exit
   Get the url that you will curl from
   Initialize the curl
   Curl the given url
   If it is not successful, print error message and exit
   If it is successful, for each keyword or 'tag', call processText
   ProcessText gets the appropriate information from in between the tags and prints it out
   return */
//
// ---------------- Open Issues 

// ---------------- System includes
#include "mycurl.c"
// ---------------- Local includes  e.g., "file.h"

// ---------------- Constant definitions 

// ---------------- Macro definitions

// ---------------- Structures/Types 

// ---------------- Private variables 
char *startWords[]={"<visibility_mi>", "<wind_string>", "<relative_humidity>", "<temperature_string>", "<weather>", "<observation_time>",
                  "<station_id>", "<location>", "<credit>"};
char *info[]={"Visibility in miles","Wind","Relative humidity", "Temperature", "Weather", "Time of observation",
              "Station ID", "Location", "Credit"};

// ---------------- Private prototypes 
void processText(char *text, char *keyword);
/*====================================================================*/

int main(int argc, char *argv[]) {
	/* Get the provided location code */
  char *code=argv[1];

	CURL *curl;
	CURLcode res;
	struct curlResponse s;

  /* Make sure that only one argument was provided */
	if (argc != 2 ) {
		fprintf(stderr, "./weather needs one arg, a location code like KMWN which would be "
			"Mt. Washington\n To lookup location codes, see \n"
			 "http://w1.weather.gov/xml/current_obs/seek.php?state=ak&Find=Find");
		return 1;
	}
  /* Make sure that the argument provided is 4 characters long */
	else if (strlen(code) != 4){
		fprintf(stderr, "Location code must be 4 characters.");
		return 1;
	}
  /* setup curl stuff */
	init_curlResponse(&s);
  /* set-up what the url shoudl be (without the location code) */
  char *APIurl = "http://w1.weather.gov/xml/current_obs/%s.xml";
  char url[50];
	sprintf(url, APIurl, code);

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
    /*use a useragent, so that it thinks request is from a valid source */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "CS50/1.0");
		/* set the function curl should call with the result */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    /* set the buffer into which curl should place the data */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* make sure it worked */
    if (res != CURLE_OK ){
    	fprintf(stderr, "Curl failed: %s \n", curl_easy_strerror(res));
    	return 1;
    }
    /* check: if the weather station code wasn't found, we get back HTML instead of XML  */
    char typeText[5];
   	char *response=s.ptr;
    strncpy(typeText, response, 5);
    if (strcmp(typeText, "<?xml") != 0){
    	fprintf(stderr, "The station code %s couldn't be found.", code);
    }
    /* otherwise, select the desired output from the results */
    else {
      int i;
      int length= sizeof(startWords)/sizeof(startWords[0]);
      /* run through the 'tags' in the array */
      for (i=0; i<length; i++){
         char *key=startWords[i];
         fprintf(stdout, "%s:", info[i]);
    	   processText(response, key);
      }
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
	return 0;
}


void processText(char *text, char *keyword){
  
    char *key=keyword;
    char *firstLocation;
    /* Finds where the tag is first located in the xml string */
    firstLocation= strstr(text, key);

    char delim1[2]=">";
    char delim2[2]="<";

    /* successive strtok calls use delimiters to get the info between the tags */
    strtok(firstLocation, delim1);
    char *parse2= strtok(NULL, delim2);
    fprintf(stdout, " %s\n", parse2);
}







