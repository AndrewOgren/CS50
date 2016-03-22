/*	chill.c	- A program to calculate the wind chill based on the temperature and windspeed

	No copyright

	No license

	Project name: chill
	Component name: none
	
	Primary Author:	Andrew Ogren
	Date Created: 01/20/2016

	Special considerations:  
	Compiles with -lm
	
======================================================================*/
// ---------------- Pseudocode
/*Check to make sure that an argument was supplied
  If no argument was supplied, print the standard temps and windspeeds list
  If one argument was supplied, assume that it's temperature
  Check to make sure temperature is less than 50 degrees
  If it is less than 50 degrees, calculate the windchill with a standard list of windspeeds
  If it is 50 degrees or above, exit with error message
  If two arguments were supplied, assume first is temp and second is windspeed
  If the temp is less than 50 degrees and windspeed is greater than or equal to .5
  Calculate the wind chill based on that temp and wind
  Else, give an appropriate error message
  If there are more than two arguments, exit and give error message */
//
// ---------------- Open Issues 

// ---------------- System includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ---------------- Local includes 

// ---------------- Constant definitions 

// ---------------- Macro definitions
#define MAXTEMP 50.0
#define MINVELOC 0.5
#define NUMTEMPELS 6
#define NUMVELOCELS 3

// ---------------- Structures/Types 

// ---------------- Private variables 
static float standardTemps[NUMTEMPELS]= {-10.0, 0.0, 10.0, 20.0, 30.0, 40.0};
static float standardVeloc[NUMVELOCELS]= {5.0, 10.0, 15.0};
static char standardDisplay[]="Temp 	Wind	Chill \n-----	----	----- \n";

// ---------------- Private prototypes 
float calcTemp(float, float);

/*====================================================================*/

int main(int argc, char *argv[]){
	float floatTemp;
	float floatWindSpeed;
	float windChill;

	if (argc == 1) { //No arguments were supplied
		printf("%s", standardDisplay);
		int i;
		for(i=0; i < NUMTEMPELS; i++) { //for each standard temperature found in standardTemps array
			printf("\n");
			int j;
			for(j=0; j < NUMVELOCELS; j++){ //for each standard wind speed found in standardVeloc array
				floatTemp=standardTemps[i]; //get the temp
				floatWindSpeed=standardVeloc[j]; //get the wind speed
				windChill=calcTemp(floatTemp, floatWindSpeed); //calculate the wind chill
				printf("%3.1f 	%3.1f 	%3.1f \n", floatTemp, floatWindSpeed, windChill);
				//The %3.1f notation means that it will print 3 digits going to one decimal place
			}
		}
	}

	else if (argc == 2) { //if the temperature was supplied as an argument
		char* temp=argv[1];
		floatTemp=atof(temp); //convert the argument to a floating point number
		//same process as before, but check to make sure temp is less than 50 degrees
		if ( floatTemp < MAXTEMP ) {
			printf("%s", standardDisplay);
			int i;
			for(i=0; i < NUMVELOCELS; i++) {
				floatWindSpeed=standardVeloc[i];
				windChill=calcTemp(floatTemp, floatWindSpeed);
				printf("%3.1f 	%3.1f 	%3.1f \n", floatTemp, floatWindSpeed, windChill);
			}
		}
		else {
			printf("The temperature must be less than 50.0 degrees Fahrenheit.");
		}
	}
	else if (argc == 3) { //assume arguments are temp and wind speed
		char* temp=argv[1];
		char* windSpeed=argv[2];
		floatTemp=atof(temp);
		floatWindSpeed=atof(windSpeed);
		//make sure that temp is less than 50 degrees and wind speed is >= .5
		if ((floatTemp < MAXTEMP) && (floatWindSpeed >= MINVELOC)) {
			printf("%s", standardDisplay);
			windChill=calcTemp(floatTemp, floatWindSpeed);
			printf("%3.1f 	%3.1f 	%3.1f \n", floatTemp, floatWindSpeed, windChill);
		}
		else {
			if (floatTemp >= MAXTEMP) {
				printf("The temperature must be less than 50.0 degrees Fahrenheit."); }
			if (floatWindSpeed < MINVELOC) {
				printf("Wind velocity must be greater than 0.5 MPH."); }
		}
	}
	else {
		printf("There are too many arguments."); //there can't be more than two supplied arguments
	}

}

float calcTemp(float temp, float windSpeed){
	float chillTemp=(35.74 + (0.6215*temp) - (35.75*powf(windSpeed, 0.16)) + (0.4275*temp*powf(windSpeed, 0.16)));
	//Utilizes the powf function to raise windspeed to the power of .16
	return chillTemp;
}

