Compiled:
I compiled dog.c with the following:
mygcc -0 dogtest dog.c

where mygcc=gcc -Wall -pedantic -std=c11 -ggdb

Execute:
Execute by calling dogtest (or whatever name you compile it as)
along with any desired switches including -n, -m, -M, and --help.

Assumptions:
1.) If dog is called with no arguments or switches or just '-', read from 
standard input.

2.) If dog is called with no arguments or '-', along with switches, read from standard
input while invoking those switches.

3.) If dog is called with -mn, -mM, or Mn...essentially anything where the next text right after
either -m or -M is not a mapfile, it will crash and report an error message. Both -m and -M NEED to 
have their mapfiles listed directly after themselves. 

4.) If dog is given a file that is an executable, it trys to read it, but fails and simply outputs nothing.

5.) Since the directions say, "you need only encipher the 26 letters of the alphabet", I assumed this meant
the lowercase letters, since enciphering both lowercase and uppercase letters would be 52.

6.) Since I used functions like islower() from ctype.h, I'm assuming that it is using ASCII encoding for the characters,  or else it will not encipher correctly.

7.) Since I only encrypted/decrypted lowercase letters, this means that capital letters or any other character will stay the same. This also means that the mapfile must only have 26 lines that encrypt lowercase letters, or else it will give an error message.
