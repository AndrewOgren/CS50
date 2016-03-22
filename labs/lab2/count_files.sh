#!/bin/bash
# Script name: count_files.sh
#
# Description: Find the number of extensions of each type 
# that are in the current directory and subdirectories
#
# Command line options: None
#
# Input: None
#
# Output: the number of files of each type
# including a no-extension category
# 
# Special Considerations and assumptions:
# Files that begin with a . (hidden files), files that have no extension
# and files that end with a . but have noextension after were grouped into the
# no extension category
#
# Pseudocode
# Gets the count of hidden files
# Gets the count of files that have no extension
# Gets the count of files that end with a  but have no extension
# Adds the previous three category counts up to get noext category count
# Gets the files that have an extension and lists them by type with a count next to them
# Prints the number in the noext category after the previous command completes
#

#Gets the hidden files that begin with a '.'
hiddenFiles=`find . -type f | awk -F'/' '{print $(NF)}' | grep '^\.' | wc -l`

#Gets the files that have no extension
none=`find . -type f | awk -F'/' '{print $(NF)}' | grep -v '\.' | wc -l`

#Gets the files that end with a '.', but have no extension after it
endDot=`find . -type f | awk -F'/' '{print $(NF)}' | grep '\.$' | wc -l`

noExt=`expr "$hiddenFiles" + "$endDot" + "$none"`

#Gets the files with extension and sorts them and counts the number of duplicate extensions
find . -type f | awk -F'/' '{print $(NF)}' | grep -v '^\.' | grep -v '\.$' | grep '\.' | awk -F'.' '{print $(NF)}'| sort | uniq -c

printf "      $noExt noext"

exit 0
