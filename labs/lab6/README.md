README for Query
------------------

Compiling:

To correctly compile the query, make sure that you're in the lab6 directory.

Then, issue the commands:
make clean
make
make testing

The command "make clean" gets rid of any executables or any files in the bin directory, any object files
in the src, test, or util directory, and any other extraneous files.

The command "make" first goes into the util directory and recompiles the library, libtseutil.a.
Then it goes into the src directory and creates all of the object files that are necessary to create
the query executable, which is placed in the bin directory.

The command "make testing" goes into the test directory and calls the makefile in the test directory.
This creates the object file for queryengine_test.c and the executable, queryengine_test.

If one calls the ./QEBATS.sh file in the lab6 directory, the previous three commands will be issued
automatically. 

It is important that the directory structure is the same as the following in order for the compiling to
work properly.

Here is the directory structure:

`` `
.
├── bin
│   └── placeholder
├── design.pdf
├── logfile.dat
├── makefile
├── QEBATS.sh
├── README.md
├── src
│   ├── query.c
│   ├── rank.c
│   └── rank.h
├── test
│   ├── makefile
│   └── queryengine_test.c
└── util
    ├── libtseutil.c
    ├── libtseutil.h
    └── makefile
` ``

Executing:

In order to execute this program, you must be in the bin directory, where the executable was compiled.

Too run query, do the following:

./query [INDEX FILE PATH] [CRAWLER DIRECTORY PATH]

The [INDEX FILE PATH] is the path to the index file that was created in Indexer.

The [CRAWLER DIRECTORY PATH] is the path to the directory that holds the html files from crawler.

Then, once query is running, and you see the prompt to query (QUERY:>):

Type any words to search for them in the html of the urls.

However, AND and OR are special keywords.

If you want both words to appear in the document, use 'AND' in between them.

If you want either word to appear in the document, use 'OR' in between them.

When you are done, type 'q' and then return to exit.

Additionally, having two words next to each other is counted as an 'AND' for both of the words.

Additionally, there is a --help option, which will explain how to properly execute the query as well.


Assumptions:

1.) The crawled html files and the index files are either from executing my crawler and indexer programs, or
	the index file that I used is found at /net/class/cs50/web/tse/indexer/cs_lvl3.dat and the crawler directory
	that I tested with is found at /net/class/cs50/web/tse/crawler/lvl2/

2.) My ranking scheme essentially builds a linked list such that the document with the highest number of total occurrences
	is at the head. The order or documents that have the same number of total occurrences will vary depending on what order they are in the document array. Those that come later in the array will be earlier in the list, because it is inserted if its totalOccurrences are GREATER THAN OR EQUAL TO the current node's total occurrences.

3.) It is assumed that the file-naming scheme is the one that was implemented in crawler, because the docIDs of documents
	where one of the words was found but not all are set to 0 to prevent them from being included in the query's linked list.

4.) I made it so that typing 'q' as a query would exit the program. Thus, one can't query 'q', but this shouldn't be a 		
	problem, since it wouldn't be found anyway, because only words of 3 characters or more were included in the index.

5.) The --help option can be used to get information on how to properly execute the program. However, whenever this
	option is used, the program does not run, but gives information on how to properly run the program.

6.) The target directory must end in a '/', so that the filename can be properly appended to create the
	file path. If it does not end in a '/', an error message will be given, and the program will terminate.



























