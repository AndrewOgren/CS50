#!/bin/bash
# This is the Lab5 BATS.sh file
#
# Script name: BATS.sh
#
# Description: This shell script is solely used for testing purposes. It tests various aspects of the indexer program. 
# However, this may not test all possible cases.
#
#Command line options: none
#
# Input: None
#
# Output: Into a log file containing
# date & time stamp of the beginning and end of the build
# the hostname and Operating System where the build was run,
# the results of each step of the build, and
# the results of running all the tests (name or purpose of test, success/fail, etc.)
#
# IMPT: These tests will not work unless the directories are structured as indicated in the lab requirements
# It is most important to note that the 1746 html files are stored in a path from the current directory of: ../data/crawler/


if [ $# -ne 0 ]; then
	echo -e "This script should take no arguments."
	exit 1
fi

log="logfile.dat"

#Create a header for the logfile
echo -e "Indexer test log \nStarted the log on `date +"%a, %b %d %Y at %T"`\n\
Running on hostname: `hostname`\nOn operating system: `uname -o`\n" > $log

#First remove any previous .o file and executables
echo -e "\nCalling 'make clean'\n" >> $log
make clean > /dev/null
#Compile the indexer from scratch
echo -e "\nCalling 'make'\n" >>$log
make > /dev/null
echo -e "\nCompile was successful\n" >> $log

cd ./bin
#Change the path to the logfile
log="../logfile.dat"
echo -e "\nChanging to the 'bin' directory, where the executables are.\n" >> $log

#Test the command line arguments
echo -e "\n\nTesting the --help option with no other arguments.\n\n" >> $log
./indexer --help >> $log 2>&1

echo -e "\n\nTesting the --help option with other arguments. Should just give help info and quit.\n\n" >> $log
./indexer ../data/crawler/ foople.dat --help >> $log 2>&1

echo -e "\n\nTesting indexer with invalid arguments.\nAll of the following tests should report errors.\n\n" >> $log
#Test with no arguments; it should fail
echo -e "\n\nTesting with no arguments.\n\n" >> $log
./indexer >> $log 2>&1
#Test with 1 argument only.
echo -e "\n\nTesting with one argument only.\n\n" >> $log
./indexer ../data/crawler/ >> $log 2>&1

#Test with too many arguments.
echo -e "\n\nTesting with too many arguments.\n\n" >> $log
./indexer ../data/crawler/ index.dat index.dat new_index.dat index.dat >> $log 2>&1

echo -e "\n\nTesting with a directory that doesn't exist.\n\n" >> $log
#Test with invalid directory (doesn't exist)
if [ -d "./fakedir" ]; then
	rm ./fakedir/*
	rm ./fakedir
fi
./indexer ./fakedir/ index.dat >> $log 2>&1

#Test with a directory path that doesn't end in a '\'
echo -e "\n\nTesting with a directory path that doesn't end in a '\'\n\n" >> $log
if [ ! -d "./badDirPath" ]; then
	mkdir ./badDirPath
fi
./indexer ./badDirPath index.dat >> $log 2>&1

rmdir ./badDirPath


#Make a real directory for testing
if [ ! -d "./testing" ]; then
	mkdir ./testing
fi

echo -e "\n\nMade a directory for testing, called 'testing'\n\n" >> $log

#Test with file that doesn't exist
echo -e "\n\nTesting with a file that doesn't exist.\n\n" >> $log
FILE="foo"
if [ -f $FILE ]; then
	rm $FILE
fi
./indexer ./testing/ foo >> $log 2>&1

if [ ! -f "index.dat" ]; then
	touch index.dat
fi
echo -e "\n\nMade a file called 'index.dat' for testing.\n\n" >> $log

if [ ! -f "./testing/777" ]; then
	touch ./testing/777
fi
echo -e "\n\nMade a file called '777' and moved it into the testing directory.\n\n" >> $log

#Test with a non-numeric file in the data directory
echo -e "\n\nTesting the empty file, '777' in a directory. \n\n" >> $log
./indexer ./testing/ index.dat >> $log 2>&1

rm ./testing/777

if [ ! -f "./testing/clown" ]; then
	touch ./testing/clown
fi
echo -e "Here is some text\nLine 2\nLine3...\nLine4" > ./testing/clown
echo -e "\n\nAdded a non-numeric file, clown, that is not empty.\n\n" >> $log
echo -e "\n\nTesting the non-numeric file, clown, that is not empty.\n\n" >> $log
./indexer ./testing/ index.dat >> $log 2>&1

rm ./testing/*

echo -e "\n\nTesting with an empty directory\n\n" >> $log
./indexer ./testing/ index.dat >> $log 2>&1

rmdir ./testing/

echo -e "\n\nTesting writing to a file that does not have write permissions.\n\n" >> $log
chmod -w index.dat
./indexer ../data/crawler/ index.dat >> $log 2>&1

echo -e "\n\nTesting having args 2 and 3 be different file names.\n\n" >> $log
if [ ! -f "new_index.dat" ]; then
	touch new_index.dat
fi
if [ ! -f "whoops.dat" ]; then
	touch whoops.dat
fi

./indexer ../data/crawler/ index.dat whoops.dat new_index.dat >> $log 2>&1

rm -f index.dat
rm -f new_index.dat

echo -e "\n\nNow Testing using valid inputs with the 1746 html files found at depth 2 of crawler.\n\
These files are found in the directory path ../data/crawler/ \n\n" >> $log

echo -e "\n\nThis will be done in testing mode, so that both index files can be compared.\n\n" >> $log

if [ ! -f "index.dat" ]; then
	touch index.dat
fi

if [ ! -f "new_index.dat" ]; then
	touch new_index.dat
fi

./indexer ../data/crawler/ index.dat index.dat new_index.dat >> $log
echo -e "\n\nHere are the contents of index.dat:\n\n" >> $log

cat index.dat >> $log

echo -e "\n\nNow, let's see if there's a difference between the index.dat and new_index.dat.\n\
There should be no difference.\n\n" >> $log
diff -s index.dat new_index.dat >> $log

cd ..
make clean > /dev/null

log="logfile.dat"
echo -e "\nThe test finished on `date +"%a, %b %d %Y at %T"`" >> $log



