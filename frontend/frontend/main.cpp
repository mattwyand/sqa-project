#include "program.h"


/*
 * Digital Games Distribution System - Front End
 *
 * This program is a command-line front end for managing accounts and game
 * information in a digital game distribution system. It loads stored user
 * accounts, available games, and game ownership records from text files,
 * then displays a menu of commands based on the current login session.
 *
 * Users can log in, view the games in their collection, log out, and exit
 * the program. Administrator sessions also allow creating and deleting
 * user accounts and adding credit to user wallets. The program supports
 * different account types (administrator, full-standard, buy-standard,
 * and sell-standard), which describe the intended user permissions.
 *
 * Changes to account information are saved to files so they can be loaded
 * again when the program starts. Supported actions are recorded as
 * transactions, and the transaction log is written when a user logs out.
 *
 * Buying, selling, and refunding games are planned features but are not
 * yet available through the current command menu.
 *
 * The main() function creates a Program object and calls run(), which
 * initializes the stored data and handles user input until the user exits.
 */


int main()
{
	Program program;
	program.run();

	return 0;
}