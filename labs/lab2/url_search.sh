#!/bin/bash
# Script name: url_search.sh
#
# Description: Takes in a txt file with urls listed on each line
# as well as words as arguments to search for within the given urls.
# As a check, it makes sure that there are at least two parameters, that the
# first parameter is a .txt file, and that the actual file exists.
# Then it curls those urls within the .txt file to get the html files and counts how many occurences
# of each word there are in each url. It prints out the word(s) and their counts for the
# respective URLs.
# If there is an error with one of the urls (it is broken), the error is reported.
#
# Command line options: None
#
# Input: A .txt file that countains urls on each line and at least one word to search for
#
# Output: A list of the urls and their counts for the words that were searched
# 
# Special Considerations and assumptions: None
#
# Pseudocode
# Check if there are the correct number of parameters (at least two)
# If not, tell the user, and exit
# Check if the first parameter is a .txt file
# If not, let the user know and exit
# Check that the file exists
# If not, tell the user, and exit
# Check that all of the urls are valid
# If not, tell the suer, and exit
# For each search-word, curl each of the urls and get the count of the number of 
# occurrences of each word for the respective url
#



#Make sure there are at least two parameters
if [ $# -lt 2 ]
	then
		printf "There are too few parameters. You need a .txt file with urls and at least one word to search for. \n"
		exit 1
fi
#Make sure the first parameter is a .txt file
ext=".txt"
if [[ "$1" != *"$ext" ]]
	then
		printf "The first parameter must be a .txt file."
		exit 1
fi
#find the path to the file, and if the file does not exist, exit with an error
file=`find ~/ -type f -name $1`
if [ ! $file ]
	then
	printf "This file does not exist on this machine."
	exit 1
fi

#Makes sure all of the urls in the .txt file are valid
while IFS='' read -r line || [[ -n "$line" ]]; do
	urlTest=`curl $line > /dev/null 2>&1; echo $?` #dumps output of curl into /dev/null and gets the exit code of previous command
	if [ $urlTest != 0 ]
		then
		printf "There is an invalid url in your file."
		exit 1
	fi
done < $file

#Curl for each parameters with the given urls
num=1
for param in ${@:2}
do
	printf "$param \n"
	while IFS='' read -r line || [[ -n "$line" ]]; do  #This format works even if there is no EOF tag in .txt file 
		count=`curl -s $line > $num.html; grep -o "$param" $num.html | wc -l`
		printf "$line $count \n"
		rm $num.html
	done < $file
	num=$((num+1))
done


exit 0