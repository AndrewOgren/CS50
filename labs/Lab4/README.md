# README.md file for Lab4 Setup Files

Compiling:

*Simply use the 'make' command, and everything should compile correctly.
*There's also a 'make clean' command that removes .o files, executables, and unwanted files.

Executing:

My program compiles as crawler. So it can be executed with:

./crawler [SEED URL] [TARGET DIRECTORY FOR DATA] [MAX CRAWLING DEPTH]

To run my test script, I ran:

./crawlerTest.sh > crawlerTestlog.`date +"%a_%b_%d_%T_%Y"`

Assumptions:

There are several assumptions I made in this program.

1.) The seed url MUST be http://www.cs.dartmouth.edu/~cs50/tse/
	This is made extra-confusing, since we started out the lab using a
	different url, but if you try to execute the program with the other url
	it will fail. I made sure that it only works with this url, since we are 
	only supposed to crawl this dowmain.

2.) The directory passed to it already exists and is valid.

3.) It will overwrite files that were previously stored in that directory.

4.) It should terminate if there is no memory left (malloc fails).



