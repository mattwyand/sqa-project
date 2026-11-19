"""
error_logger.py - Writes error messages to the terminal in the required form.

Every recorded error has the form   ERROR: <msg>
  - failed constraint: <msg> has the type and description of the error and the
    transaction that caused it; processing continues with the next transaction.
  - fatal error: <msg> has the type and description of the error and the file that
    caused it; the Back End stops immediately.
"""

import sys


class FatalError(Exception):
    """Raised to stop the Back End after a fatal error has been logged."""


class ErrorLogger:
    """Prints failed-constraint and fatal error messages to standard output."""

    def log_constraint_error(self, description, transaction_line):
        """Log a failed business constraint and the transaction line that caused it."""
        print(f"ERROR: Failed constraint: {description} | transaction: {transaction_line}")

    def log_fatal_error(self, description, file_name):
        """Log a fatal error with the file that caused it, then stop processing by
        raising FatalError."""
        print(f"ERROR: Fatal error: {description} | file: {file_name}")
        raise FatalError(description)

    def log_usage_error(self, usage_text):
        """Print how to run the program when the command-line arguments are wrong."""
        print(usage_text, file=sys.stdout)
