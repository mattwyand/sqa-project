"""
=============================================================================
 Game Distribution System - Back End  (CSCI 3060U course project)
=============================================================================

 Overall intention:
   The overnight batch processor. It reads the previous day's data files, applies
   every transaction from the Merged Daily Transaction File (all of the day's Front
   End sessions joined together), and writes the new data files that tomorrow's
   Front End runs will read.

 Input files:
   merged transaction file   - every daily transaction line from all Front Ends,
                               ending with an empty session (a single 00 line)
   old user accounts file    - yesterday's Current User Accounts File (28-char lines)
   old available games file  - yesterday's Available Games File (49-char lines)
   old games collection file - yesterday's Games Collection File (42-char lines)

 Output files:
   new user accounts file, new available games file, new games collection file
   (same formats as the old ones)

 Terminal output (log):
   ERROR: <msg> lines - a failed business constraint names the transaction that
   caused it and processing continues; a fatal error names the file that caused it
   and the Back End stops without writing the new files.

 How to run:
   python main.py merged_transactions.txt old_accounts.txt old_games.txt old_collection.txt \
                  new_accounts.txt new_games.txt new_collection.txt
=============================================================================
"""

import sys

from back_end import BackEnd, BackEndFilePaths
from error_logger import ErrorLogger, FatalError

USAGE = ("usage: python main.py merged_transactions old_accounts old_games old_collection "
         "new_accounts new_games new_collection")
NUMBER_OF_FILE_ARGUMENTS = 7


def main(arguments):
    """Check the command-line arguments, then run the Back End.
    Returns the program's exit status (0 = success, 1 = error)."""
    logger = ErrorLogger()
    if len(arguments) != NUMBER_OF_FILE_ARGUMENTS:
        logger.log_usage_error(USAGE)
        return 1

    try:
        BackEnd(logger).run(BackEndFilePaths(*arguments))
    except FatalError:
        return 1  # already logged; stop without writing the new files
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
