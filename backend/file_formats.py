"""
file_formats.py - Field widths and formatting helpers for every fixed-width file.

All of the system's files (user accounts, available games, games collection and the
daily transaction file) are fixed-width text. The widths live here, in one place, so
the readers and writers stay consistent and a change to a format is a one-line edit.
"""

# ---- Field widths (characters) ----
USERNAME_WIDTH = 15   # UUUUUUUUUUUUUUU
USER_TYPE_WIDTH = 2   # TT  (AA, FS, BS, SS)
CREDIT_WIDTH = 9      # CCCCCCCCC, e.g. 000500.00
GAME_NAME_WIDTH = 26  # IIIIIIIIIIIIIIIIIIIIIIIIII
PRICE_WIDTH = 6       # PPPPPP, e.g. 020.00

# ---- Line lengths of the data files (without the newline) ----
ACCOUNTS_LINE_LENGTH = USERNAME_WIDTH + 1 + USER_TYPE_WIDTH + 1 + CREDIT_WIDTH        # 28
AVAILABLE_GAMES_LINE_LENGTH = GAME_NAME_WIDTH + 1 + USERNAME_WIDTH + 1 + PRICE_WIDTH  # 49
COLLECTION_LINE_LENGTH = GAME_NAME_WIDTH + 1 + USERNAME_WIDTH                         # 42

# Name used on the last line of every data file
END_MARKER = "END"


def pad_text(text, width):
    """Return 'text' left-justified and padded with spaces to 'width' characters."""
    return text.ljust(width)


def format_money(amount, width):
    """Return a dollar amount with 2 decimals, zero-padded to 'width' characters
    (e.g. format_money(5, 9) -> '000005.00')."""
    return f"{amount:0{width}.2f}"


def read_text_field(line, start, width):
    """Return the text field of 'width' characters starting at 'start', without padding."""
    return line[start:start + width].strip()


def read_money_field(line, start, width):
    """Return the monetary field of 'width' characters starting at 'start' as a float.
    An empty field (e.g. on the final empty 00 line) counts as 0.00."""
    field = line[start:start + width].strip()
    return float(field) if field else 0.0
