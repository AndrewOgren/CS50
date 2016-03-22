Primary Authors: Andrew Ogren '18, Amos Cariati '18, James Thompson III '17

Date Created: March 2nd, 2016

General directory structure:
`` `
.
├── bin
│   └── placeholder
├── design_spec.pdf
├── logs
│   ├── Amazing_aogren_2_1.log
│   ├── Amazing_aogren_3_2.log
│   ├── Amazing_aogren_4_3.log
│   ├── Amazing_aogren_5_5.log
│   └── Amazing_aogren_6_7.log
├── makefile
├── README.md
├── src
│   ├── amazing.h
│   ├── AMStartup.c
│   ├── avatar.c
│   ├── avatar.h
│   └── unit_test.c
└── test
    └── placeholder
` ``

Compiling
_________
'Startup' and 'avatar' can both be compiled simultaneously using the Makefile file contained in
	the upper level of the Joubertin fork of the Amazing Maze project. The Makefile also 
	supports a 'make clean' functionality as well as a 'make test' functionality for unit testing
	in the test directory.

The 'Startup' and 'avatar' executables are both made in the 'bin' directory. As a result, to 
	start the program, the user must change to the 'bin' directory before issuing any command 
	line inputs. If one is trying to unit test, change to the 'test' direcory, and call ./unit_test.

Executing
_________

The program requires an initial command line input of:
	./Startup [nAvatars] [Difficulty] [Hostname]

	[nAvatars] must be an integer value greater than 0 and equal to or less than 10. It 
	represents the number of avatar characters the user wishes to place into the maze.

	[Difficulty] must be an integer value greater than 0 and equal to or less than 9. It 
	represents the level of difficulty of the computer generated maze. 9 represents the hardest
	available level.

	[Hostname] is the Hostname of the server used for this program. As specified, the program 
	should be exclusively run on 'flume.cs.dartmouth.edu'. As a result this is the only 
	accepted input for this parameter.

	An example commandline input would be:
	./Startup 2 2 flume.cs.dartmouth.edu

Startup will proceed to initiate communication with the server designated by the [Hostname] 
	parameter by sending an AM_INIT message containing the desired number of avatars and 
	difficulty level of the maze specified by the [nAvatars] and [Difficult] parameters 
	respectively. The server proceeds to generate a maze of specified difficulty and place an 
	appropriate number of avatars into it. The server then responds to Startup with an 
	AM_INIT_OK message that contains a unique integer for the Mazeport identification number 
	along with the maze's Width and Height. 

Startup takes this information and creates a thread for each of the 'n' specified number of 
	avatars. Program control then transfers to 'avatar'. Each thread uses the passed Mazeport 
	identification number to contact the server with an AM_AVATAR_READY message containing its 
	unique avatar identification number. After all AM_AVATAR_READY messages have been recieved, 
	the server responds by sending an AM_AVATAR_TURN message to each subsequent avatar thread 
	containing that avatar's (x,y) location in the maze and its turn identification number.

Each avatar thread then, on its turn, will send an AM_AVATAR_MOVE message to the server 
	specifying which direction the avatar's algorithm suggests it attempt to move in order to 
	find the other avatars in the maze. If the move is feasible in the context of the maze 
	(there is not a wall in the way) the server will update the avatar's location. Regardless of 
	if the location changed, however, the server will respond to each thread with an updated
	AM_AVATAR_TURN message with their current location in the maze and an incremented turn 
	number.

This process will repeat until:
 	- A thread's conection to the server is broken. 
 	- The maximum number of avatar moves for that designated difficulty level is reached
 	- The server's AM_WAIT_TIME timer expires
 	- Or all avatars are located at the same (x,y) coordinate in the maze (solved)

Once one of these exit conditions is met, the server will prompt the avatars to log their 
	success/progress in an output log, free any allocated memory, close the mazeport and then 
	exit the program. If the program is exited as a result of the maze being solved, the server 
	will send an AM_MAZE_SOLVED message to each avatar thread who will collectively write the 
	message to a single output log file.

The output log file will be named 'Amazing_$USERID_$nAvatars_$Diffculty.log'. The file will be 
	written to the whatever directory the 'Startup' executable is called from.
	Example LogFile Name:
	Amazing_acariati_2_2.log


Assumptions
___________


1.) This program will not be run by someone who doesn't know what they're doing. This means that
	the argument checking is not as extensive or thorough as it should be if this program were going
	to be used by people who did not write it.

2.) We assumed that all of the generated mazes were perfect mazes.

3.) We assumed that the server would handle order of the threads properly without our intervention.

4.) We assumed that each avatar can know the location of all of the other avatars in the maze.

5.) We assumed that each avatar can leave a "trail of breadcrumbs" of where it has been.

6.) We assumed that the avatars have no previous knowledge of the maze.

7.) The user is expected to adjust the font to a reasonable size for the graphics
to function properly. The user should adjust font size of terminal prior to running Startup script.
	
	Recommended Font Sizes (May vary depending on size of monitor):

		Difficulty    				Font Size
		    0							12
		    1 							12
			2 							12
			3 							10
			4 							7.5
			5 							5.5
			6 							4.5
			7 							4
		    8							2.6
		    9 							.8

