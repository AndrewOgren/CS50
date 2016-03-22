#!/bin/bash
# Script name: spy.sh
#
# Description: Checks if any of a list of users are logged onto a machine.
# Runs continuously (with 60s sleep) until killed with a trap signal. When
# the script detects a POI has logged in/out, it
# remembers that session.
#
# Command line options: None
#
# Input: The full names of users to monitor
#
# Output: A summary report in spy.log
# 
# Special Considerations and assumptions:
# I assumed that the user should supply an accurate name instead of something
# like "Andrew", where this could be any number of people, and you don't want to spy on the wrong person
#
# To be as accurate as possible, I got the user's actual log-in time from the who command instead of
# the time that I started running spy.
#
# I decided not to count the session if the user was still logged in when the spy script was killed,
# since this isn't a true session on Wildcat (or whatever computer)
#
# Warning: On my computer at least, if I try to cat the 'spy.log' file immediately after killing the script
# it reports that it doesn't exist. It seems it takes a few seconds to compile correctly.
#
# Pseudocode:
# Check whether the names are valid
# If they're valid, put the names into an arg array and the usernames into a different array
# If the name is not valid, alert the user
# If none of the names are valid, exit and tell the user
# If any supplied valid name has multiple associated usernames, follow them all
# Then enter the while-true loop that sleeps for 60 seconds
# In the while loop, get the list of people that are current logged on and store that in a logHistory.txt file
# For each user that you're tracking, get a list of their current logins (could be mutiple)
# and store that in currentLog.txt
# For each of these logins, check to see if they've been added to a user history array and add them if not
# Then check if they've logged of in the last minute. If they have, add them to a finalLog.txt file that holds the list of sessions
# 
# When the trap command is killed via "trap -10 PID", start the log with the kill-date and list of arguments
# Then for each name in the username list, if they've logged in at least once get the length of each of the sessions
# and find the shortest session, longest session, and the most time spent logged in 
# If they never had a session, set the numLogins and totalTime accordingly (to 0)
# Report how many times and for how long the POIs were on
# Then for each POI, give a further break down of their session (when they logged on and when they logged off)
# If no POI ever had a session, report that
# Finally, send the compiled report to a spy.log file and delete the temporary files that were created


#Checks to see if there is at least one argument
if [ $# -eq 0 ]; then
	echo -e "You need to watch at least one person! \n" 1>&2
	exit 1
fi

#Need to create a list of usernames to track and arguments (the actual names)
usernamesList=()
argumentsList=()
param=1
for arg in "$@"
	do
		#The supplied name
		actualName="$arg"

		#User needs to supply accurate name of user. 
		#There's no room for guessing what name the user meant.
		nameExists=`cat /etc/passwd | cut -d':' -f5 | grep -q "$actualName*" > /dev/null 2>&1; echo $?`
		#This returns 0 if the given name exists in /etc/passwd file
		if [ $nameExists == 0 ]
			then
			multipleUsernames=`grep "$actualName" /etc/passwd | cut -d':' -f1 | wc -l`
			
			if [ $multipleUsernames -eq 1 ]
				then
				userName=`grep "$actualName" /etc/passwd | cut -d':' -f1`
				usernamesList[$param]=$userName
				argumentsList[$param]=$actualName
				let param++
			fi

			if [ $multipleUsernames -gt 1 ]
				then
				num=1
				while [ $num -le $multipleUsernames ]; do
					#If name has multiple usernames, add all of them to list
					userName=`grep "$actualName" /etc/passwd | cut -d':' -f1 | awk NR==$num`
					usernamesList[$param]=$userName
					argumentsList[$param]=$actualName
					let param++
					let num++
				done

			fi

		else 
			echo -e "The name $actualName does not exist in this list of users. \n" 1>&2
		fi
done

if [ ${#usernamesList[@]} -eq 0 ]
	then
	echo -e "There were no valid names. Please enter some valid names." 1>&2
	exit 1
fi

numberUser=1
#Start the spy report before entering the trap function
logString="spy.sh Report \nstarted at `date +%H:%M` on `date +%Y-%m-%d`"

trapfunc() {

	killTime=`date +%R`
	killDate=`date +%Y-%m-%d`
	logString="${logString}\nstopped at $killTime on $killDate \n\
	arguments:  "
	for name in "${argumentsList[@]}"; do
		logString="${logString} $name"
	done
	totalLogins=0
	numLogins=()
	totalPeriod=()
	mostTotalTime=-100
	shortestSession=1000000000
	longestSession=-100
	#Sets these statistics to unreasonable numbers so that they are reset in the for-loop

	for name in "${usernamesList[@]}"; do
		#Set it to 0 initially for all POIs
		totalPeriod[$name]=0
		#Create an  individual log of their sessions
		grep "$name" finalLog.txt > userLog.txt

		numLogins[$name]=`wc -l userLog.txt | awk '{print $1}'`
		totalLogins=`expr $totalLogins + ${numLogins[$name]}`
		if [ ${numLogins[$name]} -gt 0 ]
			then

			userLogFile=userLog.txt
			#Format for "while read loop" received from linux.by.examples.com
			while IFS='' read -r line || [[ -n "$line" ]]; do #This formatting works even if there is no EOF tag in .txt file
				#parsing the dates out by delimiter of # and converting them to seconds since 1970-01-01 00:00:00 UTC for easier time diff calc
				date1=`echo $line | awk -F"#" '{print $3" "$4}'`
				convertedDate1=`date -d"$date1" +%s`
				date2=`echo $line | awk -F"#" '{print $5" "$6}'`
				convertedDate2=`date -d"$date2" +%s`
				timeDiffInSec=`expr $convertedDate2 - $convertedDate1`

				#updating statistics
				if [ $timeDiffInSec -lt $shortestSession ]
					then
					shortestSession=$timeDiffInSec
					shortestSessionUser=$name
				fi

				if [ $timeDiffInSec -gt $longestSession ]
					then
					longestSession=$timeDiffInSec
					longestSessionUser=$name
				fi

				totalPeriod[$name]=`expr ${totalPeriod[$name]} + $timeDiffInSec`

				if [ ${totalPeriod[$name]} -gt $mostTotalTime ]
					then
					mostTotalTime=${totalPeriod[$name]}
					mostTotalTimeUser=$name
				fi

			done < $userLogFile
		
		else
			totalPeriod[$name]=0
			numLogins[$name]=0
		fi
		totalPeriodInSecs=${totalPeriod[$name]}
		totalPeriodInMins=$((totalPeriodInSecs/60))

		logString="${logString}\n\n$name logged on ${numLogins[$name]} times for a total period of \
		$totalPeriodInMins minutes.\n"

	done

	#converting statistics to minutes
	shortestSession=$((shortestSession/60))
	longestSession=$((longestSession/60))
	mostTotalTime=$((mostTotalTime/60))

	#This is a catch for making sure that at least one person had a complete session during the time that spy was running
	if [ $totalLogins -gt 0 ]
		then

		for name in "${usernamesList[@]}"; do
			grep "$name" finalLog.txt > userLog2.txt
			logString="${logString}\n\nHere is the breakdown for $name :"

			lineCounter=1
			userLogFile=userLog2.txt
			while IFS='' read -r line || [[ -n "$line" ]]; do
				logOnDate=`echo $line | awk -F"#" '{print $3" "$4}'`
				logOffDate=`echo $line | awk -F"#" '{print $5" "$6}'`

				logString="${logString}\n$lineCounter) logged on $logOnDate; logged off $logOffDate"
				let lineCounter++

			done < $userLogFile
		done
		#Add the statistics to the logString
		logString="${logString}\n\n$mostTotalTimeUser spent the most time on wildcat today: \n \
		$mostTotalTime mins in total for all his/her sessions \n"
		logString="${logString}\n$shortestSessionUser was on for the shortest session and therefore \
		the most sneaky: \n $shortestSession mins \n"
		logString="${logString}\n$longestSessionUser was on for the longest session: \n \
		$longestSession mins"
	else
		logString="${logString}\n\n Oh my! Nobody logged in!"
	fi

	#Finally add everything to the spy log and remove the temporary txt files that stored important info
	echo -e $logString > spy.log
	rm finalLog.txt
	rm currentLog.txt
	rm logHistory.txt
	rm userLog.txt
	rm userLog2.txt

	exit 0
}
#This will trigger the trap function
trap "trapfunc" 10

userHistory=()

while [ true ];do
	#Gets everyone that's currently on logged in and formats in a way that can be parsed easily
	#This info is stored in a logHistory file that is overwritten each time through the while loop
	who | awk '{print $1"#"$2"#"$3"#"$4}' > logHistory.txt

	for name in "${usernamesList[@]}"; do
		#Gets the user's info if they are currently logged in and stores it temporarily in a file
		who | grep "$name" | awk '{print $1"#"$2"#"$3"#"$4}' > currentLog.txt

			currentLogLength=`wc -l currentLog.txt | awk '{print $1}'`
			#Makes sure that they are logged on in at least one terminal
			if [ $currentLogLength -gt 0 ]
				then
				
				currentLogFile=currentLog.txt
				while IFS='' read -r line || [[ -n "$line" ]]; do
					alreadyLogged=0

					for u in "${userHistory[@]}"; do
						#Checks to see if they've already been added to the user history array, which tracks logins of 
						#the people we are spying on
						if [ "$u" == "$line" ]
							then
							alreadyLogged=1
						fi

					done

					if [ $alreadyLogged -ne 1 ]
						then
							#Since their login has not been recorded yet, add it and increment their user number
							userHistory[$numberUser]="$line"
							numberUser=$(( numberUser + 1 ))
					fi
					

				done < $currentLogFile
			fi
	done

	for user in "${userHistory[@]}"; do
		#Checks to see if the user is still logged in
		active=`grep $user logHistory.txt`

		if [ "$active" == "" ] && [ "$user" != "null" ]
			then
			#They must have logged out since last checked, so get the logout date and time and complete the session
			logOutDateNow=`date +%Y-%m-%d`
			logOutTimeNow=`date +%R`
			finalInput=$user"#""$logOutDateNow""#""$logOutTimeNow"
			alreadyExited=`grep $user finalLog.txt`
			if [ "$alreadyExited" == "" ] #if this session has not already been added to the finalLog.txt, add it
			then
				echo "$finalInput" >> finalLog.txt
			fi
		fi
	done
	sleep 60
done


exit 0 #This should never actually be called, but just as a safe guard, it's here
