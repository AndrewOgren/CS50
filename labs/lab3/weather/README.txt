Compiled:
I compiled with the following:
mygcc -o weathertest weather.c -lcurl

**Make sure to have mycurl.c and mycurl.h in the same directory when compiling

where mygcc=gcc -Wall -pedantic -std=c11 -ggdb
I called it weathertest, because I was in the weather directory

Executed:
I followed the expectations of the directions.
I debugged using gdb.

Assumptions:
1.) I assumed, as per the directions, that the user could only supply one 
location code.
1.) I also assumed that every location code is 4 characters long.
3.) I assumed that the curl could fail for any number of reasons.
4.) I assumed that if xml wasn't returned (it was html), then that location could not be found at the moment.

