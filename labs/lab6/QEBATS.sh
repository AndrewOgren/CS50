#!/bin/bash
# This is the Lab6 QEBATS.sh file
#
# Script name: QEBATS.sh
#
# Description: This shell script is solely used for testing purposes. It tests various aspects of the query program. 
# However, this may not test all possible cases.
#
# Command line options: none
#
# Input: None
#
# Output: Into a log file containing
# date & time stamp of the beginning and end of the build
# the hostname and Operating System where the build was run,
# the results of each step of the build, and
# the results of running all the tests (name or purpose of test, success/fail, etc.)
#

if [ $# -ne 0 ]; then
	echo -e "This script should take no arguments."
	exit 1
fi

log="logfile.dat"

#Create a header for the logfile
echo -e "Query test log \nStarted the log on `date +"%a, %b %d %Y at %T"`\n\
Running on hostname: `hostname`\nOn operating system: `uname -o`\n" > $log

#remove any prior executables or object files
make clean > /dev/null
#Build the appropriate targets
make > /dev/null
make testing > /dev/null

echo -e "\nChanging to the testing directory.\n" >> $log
cd ./test/
log="../logfile.dat"

echo -e "\nRunning queryengine_test.c\n" >> $log

./queryengine_test >> $log 2>&1

echo -e "\n\nNow testing command line arguments.\n" >> $log

cd ../bin/

echo -e "\nTesting with too few arguments. Query should report an error.\n" >> $log

./query /net/class/cs50/web/tse/indexer/cs_lvl3.dat >> $log 2>&1

echo -e "\n\nTesting with too many arguments. Query should report an error.\n" >> $log

./query /net/class/cs50/web/tse/indexer/cs_lvl3.dat /net/class/cs50/web/tse/crawler/lvl2/ oops >> $log 2>&1

echo -e "\n\nTesting with a directory path that doesn't exist. Query should report an error.\n" >> $log

if [ -d "./fakedir" ]; then
	rm ./fakedir/*
	rm ./fakedir
fi

./query  ./fakedir /net/class/cs50/web/tse/crawler/lvl2/ >> $log 2>&1

#Test with a directory path that doesn't end in a '\'
echo -e "\n\nTesting with a crawler directory path that doesn't end in a '\'. Query should report an error.\n" >> $log
if [ ! -d "./badDirPath" ]; then
	mkdir ./badDirPath
fi
./query /net/class/cs50/web/tse/indexer/cs_lvl3.dat ./badDirPath >> $log 2>&1

rmdir ./badDirPath

echo -e "\n\nTesting with an index file that doesn't exist. Query should report an error.\n" >> $log
if [ -d "./fakeFile" ]; then
	rm -f ./fakeFile
fi
./query  ./fakeFile /net/class/cs50/web/tse/crawler/lvl2/ >> $log 2>&1

echo -e "\n\nTesting with an index file that can't be read. Query should report an error.\n" >> $log
if [ ! -d "./cantRead" ]; then
	touch ./cantRead
fi
chmod -r ./cantRead
./query ./cantRead /net/class/cs50/web/tse/crawler/lvl2/ >> $log 2>&1

rm -f ./cantRead

echo -e "\n\nTesting the help option with nothing else.\n" >> $log
./query --help >> $log 2>&1

echo -e "\n\nTesting the help option with other arguments. Should give help and quit.\n" >> $log
./query /net/class/cs50/web/tse/indexer/cs_lvl3.dat /net/class/cs50/web/tse/crawler/lvl2/ --help >> $log 2>&1

echo -e "\n\nDone testing the command line arguments. Now testing the functionality of query.\n" >> $log 2>&1

queryInput="queryInput.txt"

echo -e "The following tests will be run in the order that are listed:\n\
One word that has results: 'chicken'\nOne word that doesn't have results: 'fanefnla'\n\
Two words, both with matches: 'chicken AND pizza'\nTwo words, only one with matches:\
'chicken AND afnkakew'\nTwo words, only one with matches, in reverse order: 'afnkakew AND chicken'\n\
Two words, both with matches, with an OR: 'computer OR chicken'\nTwo words, only one with a match, with an OR: \
'pizza OR anflfl'\nThree words, all match, with one OR: 'computer chicken OR pizza'\n\
Three words, should only get results of third: 'computer anflenal OR pizza'\n\n\
Now some strange input to try to break query:\n\
computer AND chicken OR\ncomputer AND nflawnfe OR\nOR computer\nOR computer anflaenfla\ncomputer alfnalefma OR chicken pizza OR science\n\
OR OR OR pizza\npizza OR OR AND chicken\n" >> $log

echo -e "\n\nThe main purpose of these tests is to show that query avoids a segmentation fault.\n\
The verification of the results is more aptly suited for comparison with another query.\n" >> $log

#Put this into $queryInput so that it can be put into query's input on the commadn line, line by line
echo -e "chicken\nfanefnla\nchicken AND pizza\nchicken AND afnkakew\nafnkakew AND chicken\ncomputer OR chicken\n\
pizza OR anflfl\ncomputer chicken OR pizza\ncomputer anflenal OR pizza\ncomputer AND chicken OR\ncomputer AND nflawnfe OR\n\
OR computer\nOR computer anflaenfla\ncomputer alfnalefma OR chicken pizza OR science\nOR OR OR pizza\npizza OR OR AND chicken\nq" > $queryInput

(./query /net/class/cs50/web/tse/indexer/cs_lvl3.dat /net/class/cs50/web/tse/crawler/lvl2/) < $queryInput >> $log


rm $queryInput

cd ..
make clean > /dev/null
log="logfile.dat"
echo -e "\nThe test finished on `date +"%a, %b %d %Y at %T"`" >> $log


