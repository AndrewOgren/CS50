Assumptions and considerations for birthday_match:
-I assumed that if the date was before 09/14/1752, it could not be used, because of the difference between
-the date and the calendar.

-You must put in two valid birthdate and only two.

Assumptions and considerations for count_files.sh:
-I assumed that all hidden files started wiht a '.' and should be included in the noext category
-I assumed that files that end with a '.' and have no extension after should be included in the noext category

Assumptions and considerations for url_search.sh:

-Nothing unusual
-Assumed that the first parameter has to be a .txt file
-Assumed that all of the urls have to be valid for it to run properly

Assumptions and considerations for spy.sh:

-I assumed that the user should supply an accurate name instead of something
-like "Andrew", where this could be any number of people, and you don't want to spy on the wrong person

-To be as accurate as possible, I got the user's actual log-in time from the who command instead of
-the time that I started running spy.

-I decided not to count the session if the user was still logged in when the spy script was killed,
-since this isn't a true session on Wildcat (or whatever computer)

-Warning: On my computer at least, if I try to cat the 'spy.log' file immediately after killing the script
-it reports that it doesn't exist. It seems it takes a few seconds to a minute to compile correctly.

-While running it in the background, it will report that the finalLog.txt doesn't exist; this is fine. This is supposed to 
-happen. It should still run correctly.

-Compared to the example spy.log, I include all of the same info, but I decided it worked best if I added the summaries of
each of the people first (their number of logins and total time). Then, after, I reported specific sessions for each of the POIs