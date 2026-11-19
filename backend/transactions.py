"""
transactions.py - One class per daily transaction code.

Each class knows how to read its own line format from the Merged Daily Transaction
File (from_line) and how to apply itself to the SystemState (apply). The Back End
does not need to know the details of any transaction type.

Line formats (fields separated by one space, widths from file_formats.py):
    00, 01, 02, 06:  XX UUUUUUUUUUUUUUU TT CCCCCCCCC
    05 refund:       XX buyer(15) seller(15) CCCCCCCCC
    03 sell:         XX game(26) seller(15) PPPPPP
    04 buy:          XX game(26) seller(15) buyer(15) PPPPPP
"""

from file_formats import (USERNAME_WIDTH, USER_TYPE_WIDTH, CREDIT_WIDTH,
                          GAME_NAME_WIDTH, PRICE_WIDTH, read_text_field, read_money_field)

# Every field starts after the 2-digit code and one space
FIRST_FIELD_START = 3


class Transaction:
    """Base class for all transactions: keeps the original line for error messages."""

    def __init__(self, line):
        """Store the original transaction line."""
        self.line = line

    @classmethod
    def from_line(cls, line):
        """Create the transaction from one line of the transaction file."""
        raise NotImplementedError

    def apply(self, state, logger):
        """Apply this transaction to 'state', logging any failed constraint to 'logger'."""
        raise NotImplementedError


class UserTransaction(Transaction):
    """Shared parsing for codes 00, 01, 02 and 06:  XX username type credit."""

    def __init__(self, line, username, user_type, credit):
        """Store the username, user type and credit field of the line."""
        super().__init__(line)
        self.username = username
        self.user_type = user_type
        self.credit = credit

    @classmethod
    def from_line(cls, line):
        """Read the username, type and credit fields from the line."""
        type_start = FIRST_FIELD_START + USERNAME_WIDTH + 1
        credit_start = type_start + USER_TYPE_WIDTH + 1
        return cls(line,
                   read_text_field(line, FIRST_FIELD_START, USERNAME_WIDTH),
                   read_text_field(line, type_start, USER_TYPE_WIDTH),
                   read_money_field(line, credit_start, CREDIT_WIDTH))


class EndOfSessionTransaction(UserTransaction):
    """00 - end of a Front End session. Changes nothing."""

    def apply(self, state, logger):
        """Nothing to apply: this only marks the end of a session."""


class CreateUserTransaction(UserTransaction):
    """01 - create a new user account."""

    def apply(self, state, logger):
        """Add the account, unless the username is already taken."""
        if state.find_account(self.username) is not None:
            logger.log_constraint_error(
                f"user '{self.username}' already exists, so it cannot be created", self.line)
            return
        state.add_account(self.username, self.user_type, self.credit)


class DeleteUserTransaction(UserTransaction):
    """02 - delete a user account and cancel its games for sale."""

    def apply(self, state, logger):
        """Remove the account, if it exists."""
        if state.find_account(self.username) is None:
            logger.log_constraint_error(
                f"user '{self.username}' does not exist, so it cannot be deleted", self.line)
            return
        state.remove_account(self.username)


class AddCreditTransaction(UserTransaction):
    """06 - add credit to an account. The credit field is the amount added."""

    def apply(self, state, logger):
        """Add the credit to the account, if it exists."""
        account = state.find_account(self.username)
        if account is None:
            logger.log_constraint_error(
                f"user '{self.username}' does not exist, so credit cannot be added", self.line)
            return
        account.credit += self.credit


class RefundTransaction(Transaction):
    """05 - move credit from a seller back to a buyer."""

    def __init__(self, line, buyer_username, seller_username, amount):
        """Store the buyer, seller and refund amount."""
        super().__init__(line)
        self.buyer_username = buyer_username
        self.seller_username = seller_username
        self.amount = amount

    @classmethod
    def from_line(cls, line):
        """Read the buyer, seller and amount fields from the line."""
        seller_start = FIRST_FIELD_START + USERNAME_WIDTH + 1
        amount_start = seller_start + USERNAME_WIDTH + 1
        return cls(line,
                   read_text_field(line, FIRST_FIELD_START, USERNAME_WIDTH),
                   read_text_field(line, seller_start, USERNAME_WIDTH),
                   read_money_field(line, amount_start, CREDIT_WIDTH))

    def apply(self, state, logger):
        """Move the amount from the seller's credit to the buyer's credit."""
        buyer = state.find_account(self.buyer_username)
        seller = state.find_account(self.seller_username)
        if buyer is None or seller is None:
            logger.log_constraint_error("buyer or seller does not exist", self.line)
            return
        # TODO (Phase 5): enforce "no user should ever have a negative account balance"
        # for the seller here, as BuyGameTransaction does for the buyer.
        seller.credit -= self.amount
        buyer.credit += self.amount


class SellGameTransaction(Transaction):
    """03 - put a new game up for sale."""

    def __init__(self, line, game_name, seller_username, price):
        """Store the game name, seller and price."""
        super().__init__(line)
        self.game_name = game_name
        self.seller_username = seller_username
        self.price = price

    @classmethod
    def from_line(cls, line):
        """Read the game name, seller and price fields from the line."""
        seller_start = FIRST_FIELD_START + GAME_NAME_WIDTH + 1
        price_start = seller_start + USERNAME_WIDTH + 1
        return cls(line,
                   read_text_field(line, FIRST_FIELD_START, GAME_NAME_WIDTH),
                   read_text_field(line, seller_start, USERNAME_WIDTH),
                   read_money_field(line, price_start, PRICE_WIDTH))

    def apply(self, state, logger):
        """Add the game to the games for sale."""
        state.add_game_for_sale(self.game_name, self.seller_username, self.price)


class BuyGameTransaction(Transaction):
    """04 - a buyer purchases a game from a seller."""

    def __init__(self, line, game_name, seller_username, buyer_username, price):
        """Store the game name, seller, buyer and price."""
        super().__init__(line)
        self.game_name = game_name
        self.seller_username = seller_username
        self.buyer_username = buyer_username
        self.price = price

    @classmethod
    def from_line(cls, line):
        """Read the game name, seller, buyer and price fields from the line."""
        seller_start = FIRST_FIELD_START + GAME_NAME_WIDTH + 1
        buyer_start = seller_start + USERNAME_WIDTH + 1
        price_start = buyer_start + USERNAME_WIDTH + 1
        return cls(line,
                   read_text_field(line, FIRST_FIELD_START, GAME_NAME_WIDTH),
                   read_text_field(line, seller_start, USERNAME_WIDTH),
                   read_text_field(line, buyer_start, USERNAME_WIDTH),
                   read_money_field(line, price_start, PRICE_WIDTH))

    def apply(self, state, logger):
        """Move the price from buyer to seller and add the game to the buyer's collection."""
        buyer = state.find_account(self.buyer_username)
        seller = state.find_account(self.seller_username)
        if buyer is None or seller is None:
            logger.log_constraint_error("buyer or seller does not exist", self.line)
            return
        if buyer.credit < self.price:
            logger.log_constraint_error(
                f"buyer '{self.buyer_username}' would have a negative balance", self.line)
            return
        buyer.credit -= self.price
        seller.credit += self.price
        state.add_owned_game(self.game_name, self.buyer_username)


# Which class handles each 2-digit transaction code
TRANSACTION_TYPES = {
    "00": EndOfSessionTransaction,
    "01": CreateUserTransaction,
    "02": DeleteUserTransaction,
    "03": SellGameTransaction,
    "04": BuyGameTransaction,
    "05": RefundTransaction,
    "06": AddCreditTransaction,
}
