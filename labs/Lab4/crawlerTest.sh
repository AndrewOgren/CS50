#!/bin/bash
#Script name: testArgs.sh
#Description: Runs through various possible
# arguments on the command line for crawler in order
# to test the outputs and error-catching
# Output: Into a file called crawlerTestlog.`date +"%a_%b_%d_%T_%Y"`

echo -e "This test should try to crawl a domain other than 'http://www.cs.dartmouth.edu/~cs50/tse/' and fail: \n"

./crawler https://en.wikipedia.org/wiki/Unistd.h ./data/ 2

echo -e "\n\nThis test should try to crawl a bad url and fail: \n"

./crawler jlafaofjfla.edu ./data/ 2

echo -e "\n\nThis test should have the wrong # of arguments (only 1) and fail: \n"

./crawler http://www.cs.dartmouth.edu/~cs50/tse/

echo -e "\n\nThis test should also have the wrong # of arguments (only 2) and fail: \n"

./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./data/

echo -e "\n\nThis test should have too many arguments (4) and fail: \n"

./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./data/ 3 7

echo -e "\n\nThis test should have a non-existent directory and fail: \n"
rm -rf ./foople/
./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./foople/ 2

echo -e "\n\nThis test should have a directory with wrong permissions and fail: \n"
mkdir -p ./noWrite/
chmod -w ./noWrite/
./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./noWrite/ 2

echo -e "\n\nThis test should have too great of a depth to crawl (5) and fail: \n"

./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./data/ 5

echo -e "\n\nThis test should have too small of a depth to crawl and fail: \n"

./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./data/ -1

echo -e "\n\nThis test should crawl only to depth 0 and thus crawl nothing (no output): \n"

./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./data/ 0 

echo -e "\n\nThis test should crawl to depth 3."

./crawler http://www.cs.dartmouth.edu/~cs50/tse/ ./data/ 3




