"""
data_files.py - Reading and writing the three fixed-width data files and reading
the Merged Daily Transaction File.

DataFile holds the read/write loop shared by all three data files; each subclass
only says how one line maps to one record. TransactionFile reads the merged daily
transactions into Transaction objects.
"""

from file_formats import (USERNAME_WIDTH, USER_TYPE_WIDTH, CREDIT_WIDTH, GAME_NAME_WIDTH,
                          PRICE_WIDTH, ACCOUNTS_LINE_LENGTH, AVAILABLE_GAMES_LINE_LENGTH,
                          COLLECTION_LINE_LENGTH, END_MARKER, pad_text, format_money,
                          read_text_field, read_money_field)
from models import UserAccount, GameForSale, OwnedGame
from transactions import TRANSACTION_TYPES


class DataFile:
    """Shared reading/writing for a fixed-width data file that ends with an END line."""

    # Set by each subclass
    LINE_LENGTH = 0

    def __init__(self, logger):
        """Create the file handler; fatal errors are reported through 'logger'."""
        self.logger = logger

    def read(self, path):
        """Read every record in the file at 'path' up to the END line.
        A line of the wrong length is a fatal error."""
        records = []
        with open(path, "r") as file:
            for line in file:
                line = line.rstrip("\r\n")
                if len(line) != self.LINE_LENGTH:
                    self.logger.log_fatal_error(
                        f"line is {len(line)} characters, expected {self.LINE_LENGTH}", path)
                if read_text_field(line, 0, self.name_width()) == END_MARKER:
                    break
                records.append(self.parse_line(line))
        # TODO (Phase 5): treat a file with no END line as a fatal error.
        return records

    def write(self, path, records):
        """Write every record to the file at 'path', followed by the END line."""
        with open(path, "w") as file:
            for record in records:
                file.write(self.format_line(record) + "\n")
            file.write(self.end_line() + "\n")

    def name_width(self):
        """Width of the first field, which holds END on the last line."""
        raise NotImplementedError

    def parse_line(self, line):
        """Turn one line into one record."""
        raise NotImplementedError

    def format_line(self, record):
        """Turn one record into one line."""
        raise NotImplementedError

    def end_line(self):
        """Return the END line that closes the file."""
        raise NotImplementedError


class UserAccountsFile(DataFile):
    """Current User Accounts File:  UUUUUUUUUUUUUUU TT CCCCCCCCC  (28 characters)."""

    LINE_LENGTH = ACCOUNTS_LINE_LENGTH
    TYPE_START = USERNAME_WIDTH + 1
    CREDIT_START = TYPE_START + USER_TYPE_WIDTH + 1

    def name_width(self):
        """The username field holds END on the last line."""
        return USERNAME_WIDTH

    def parse_line(self, line):
        """Return the UserAccount on this line."""
        # TODO (Phase 5): a non-numeric credit field currently crashes instead of
        # being logged as a fatal error.
        return UserAccount(read_text_field(line, 0, USERNAME_WIDTH),
                           read_text_field(line, self.TYPE_START, USER_TYPE_WIDTH),
                           read_money_field(line, self.CREDIT_START, CREDIT_WIDTH))

    def format_line(self, account):
        """Return the 28-character line for this UserAccount."""
        return (pad_text(account.username, USERNAME_WIDTH) + " " + account.user_type + " " +
                format_money(account.credit, CREDIT_WIDTH))

    def end_line(self):
        """END with an empty type and credit field."""
        return pad_text(END_MARKER, ACCOUNTS_LINE_LENGTH)


class AvailableGamesFile(DataFile):
    """Available Games File:  game(26) seller(15) PPPPPP  (49 characters)."""

    LINE_LENGTH = AVAILABLE_GAMES_LINE_LENGTH
    SELLER_START = GAME_NAME_WIDTH + 1
    PRICE_START = SELLER_START + USERNAME_WIDTH + 1

    def name_width(self):
        """The game name field holds END on the last line."""
        return GAME_NAME_WIDTH

    def parse_line(self, line):
        """Return the GameForSale on this line."""
        return GameForSale(read_text_field(line, 0, GAME_NAME_WIDTH),
                           read_text_field(line, self.SELLER_START, USERNAME_WIDTH),
                           read_money_field(line, self.PRICE_START, PRICE_WIDTH))

    def format_line(self, game):
        """Return the 49-character line for this GameForSale."""
        return (pad_text(game.game_name, GAME_NAME_WIDTH) + " " +
                pad_text(game.seller_username, USERNAME_WIDTH) + " " +
                format_money(game.price, PRICE_WIDTH))

    def end_line(self):
        """END with all other fields empty."""
        return pad_text(END_MARKER, AVAILABLE_GAMES_LINE_LENGTH)


class GamesCollectionFile(DataFile):
    """Games Collection File:  game(26) owner(15)  (42 characters)."""

    LINE_LENGTH = COLLECTION_LINE_LENGTH
    OWNER_START = GAME_NAME_WIDTH + 1

    def name_width(self):
        """The game name field holds END on the last line."""
        return GAME_NAME_WIDTH

    def parse_line(self, line):
        """Return the OwnedGame on this line."""
        return OwnedGame(read_text_field(line, 0, GAME_NAME_WIDTH),
                         read_text_field(line, self.OWNER_START, USERNAME_WIDTH))

    def format_line(self, owned_game):
        """Return the 42-character line for this OwnedGame."""
        return (pad_text(owned_game.game_name, GAME_NAME_WIDTH) + " " +
                pad_text(owned_game.owner_username, USERNAME_WIDTH))

    def end_line(self):
        """END with all other fields empty."""
        return pad_text(END_MARKER, COLLECTION_LINE_LENGTH)


class TransactionFile:
    """Merged Daily Transaction File: one transaction per line, in the order made."""

    def __init__(self, logger):
        """Create the reader; fatal errors are reported through 'logger'."""
        self.logger = logger

    def read(self, path):
        """Return a Transaction object for every line of the file at 'path'.
        An unknown transaction code is a fatal error."""
        transactions = []
        with open(path, "r") as file:
            for line in file:
                line = line.rstrip("\r\n")
                if not line:
                    continue  # ignore blank lines
                code = line[:2]
                transaction_class = TRANSACTION_TYPES.get(code)
                if transaction_class is None:
                    self.logger.log_fatal_error(f"unknown transaction code '{code}'", path)
                transactions.append(transaction_class.from_line(line))
        return transactions
