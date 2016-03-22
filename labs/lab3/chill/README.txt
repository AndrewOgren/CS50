Compile:
I compiled chill.c with 
"mygcc -o chilltest chill.c -lm"
where mygcc=gcc -Wall -pedantic -std=c11 -ggdb

I couldn't call it chill, because it was in the chill subdirectory.

Execute:
Follows the directions. 
No arguments calculates a list of wind chills based on standard temps and wind speeds.
One argument is assumed to be temperature, and calculates wind chill with this temp and a standard set of wind speeds.
Two arguments are assumed to be temperature and wind respectively. They are used to calculate wind chill.
More than two arguments is not allowed.

Assumptions:
If the user enters anything other than a number (some other character), it is converted by atof() to be 0.0, so
the program still works if it's within the correct range.
