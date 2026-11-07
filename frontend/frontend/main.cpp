/*
=============================================================================
 Game Distribution System - Front End  (CSCI 3060U course project)
=============================================================================

 Overall intention:
   A point-of-purchase terminal for a Steam-like game store. It reads the current
   user accounts and the games for sale, processes transactions typed one per line
   (login, logout, create, delete, sell, buy, refund, addcredit, list), and writes
   every accepted transaction to the daily transaction file for the Back End.

 Input files (read at each login):
   currentaccounts.txt   - current user accounts      (28-character lines)
   availablegames.txt    - games available for sale   (49-character lines)
   gamescollection.txt   - games owned by each user   (42-character lines)

 Output:
   transout.atf          - daily transaction file, written at every logout
   standard output       - prompts and messages (the terminal log)

 Input:
   standard input        - one transaction code per line, followed by the lines
                           that transaction asks for (e.g. login / buyer)

 How to run:
   distribution-system currentaccounts.txt availablegames.txt gamescollection.txt transout.atf
   e.g. with a test file:  ./distribution-system ... transout.atf < test_input.txt

 The program ends when standard input ends.
=============================================================================
*/

#include "program.h"

#include <iostream>

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		std::cout << "usage: distribution-system currentaccounts.txt availablegames.txt "
					 "gamescollection.txt transout.atf\n";
		return 1;
	}

	Program program;
	program.run(argv[1], argv[2], argv[3], argv[4]);
	return 0;
}
