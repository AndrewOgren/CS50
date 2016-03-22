#!/bin/bash
#Script name: testArgs.sh
#Description: Runs through various possible
# arguments on the command line for crawler in order
# to test the outputs and error-catching


echo -e "This test should have the wrong # of arguments: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/

echo -e "\n\nThis test should also have the wrong # of arguments: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./data/

echo -e "\n\nThis test should have too many arguments: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./data/ 3 7

echo -e "\n\nThis test should have a non-existent directory: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./foople/ 2

echo -e "\n\nThis test should have a directory with wrong permissions: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./noWrite/ 2

echo -e "\n\nThis test should have too great of a depth to crawl: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./data/ 5

echo -e "\n\nThis test should have too small of a depth to crawl: \n"

./crawler http://old-www.cs.dartmouth.edu/~cs50/tse/ ./data/ 0

echo -e "\n\nThis test should exit normally"

./crawler "http://www.cs.dartmouth.edu/~cs50/tse/" ./data/ 2




