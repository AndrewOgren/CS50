#!/bin/bash
# Script name: birthday_match.sh
#
# Description: This program takes two birthdates as arguments. With these two birthdates,
# it first validates them, and then it finds the day of the week that each of the birthdates
# is on. If they are the same day, it lets the user know. If they are not on the same day,
# it also informs the user of this.
#
#Command line options: None
#
#Input: Two birthdates in the form of mm-dd-yyyy as parameters.
#
#Output: Writes the day of the week of the birthdays of person1 and person2 to
# standard output. It also writes whether they are the same day of the week or not.
#
#Special Considerations and assumptions: 
# The dates must be in the mm-dd-yyyy format, or else it will not work.
# The year can be past the current year (it can be in the future)
# The dates prior to 09/14/1752 are not allowed due to inconsistencies b/w the date and calendar
#
#Pseudocode:
# Determines if there are the correct number of parameters.
# If there are not the correct number, it lets the user know and exits.
# For each of the arguments, it determines if the date is of the correct format and valid.
# If either of the dates is not valid, it lets the user know which date(s) is invalid and exits.
# Determines if either of the dates is not permitted and lets the user know if they are not.
# It gets the day of the week of each of the birthday and writes them to standard output.
# It checks to see if the days are the same and lets the user know if they are or are not.

#checks to make sure that two arguments were supplied
if [ $# -ne 2 ]
	then
		printf "Usage: Two valid birthdates must be added as arguments. \n" 1>&2
		exit 1
fi

for arg in $@
do
	#The date function does not check format correctly as it will accept '9/02/1995' when it should be
	# 09/02/1995. Therefore, I check this manually.
	if [[ $arg != [0-3][0-9]/[0-1][0-9]/[0-9][0-9][0-9][0-9] ]] #Got format for checking date format from Stack Overflow user KonsoleBox
		then
			printf "Usage: Dates must be of the mm/dd/yyyy format and valid! \n"
			exit 1
	fi

	year=`date -d"$arg" +%Y`
	month=`date -d"$arg" +%m`
	day=`date -d"$arg" +%d`
	#Checks to see if date is before 09/14/1752
	if [ $year -le 1752 ] && [ $month -le 9 ] && [ $day -lt 14 ]
		then
			printf "Usage: Dates before 09/14/1752 are not allowed due to an error in the calendar. \n"
			exit 1
	fi

	#Sets the date format, takes the given date, send standard out and error to null
	#and echos the exit status, which is 1 if there is an error (date doesn't exist)
	birthday=`date "+%m/%d/%Y" -d $arg > /dev/null 2>&1; echo $?`
	
	if [ $birthday == 1 ]
	then
		printf "Usage: $arg Not a valid date. Remember to use the mm/dd/yyyy format! \n"
		exit 1
	fi
done

# get the days of the week of each of the birthdates by adding "%A"
birthday1=`date -d $1 +"%A"`
birthday2=`date -d $2 +"%A"`

printf "The first person was born on: $birthday1 \n"
printf "The second person was born on: $birthday2 \n"

if [ $birthday1 == $birthday2 ]
	then
		printf "Jackpot! You were both born on the same day of the week! \n"
else
	printf "Therefore, you are not born on the same day of the week. \n"
fi

exit 0