"""
models.py - The three kinds of records the Back End maintains.

Each class is one line of one of the data files: a user account, a game for sale,
or a game owned by a user.
"""


class UserAccount:
    """One user account: a line of the Current User Accounts File."""

    def __init__(self, username, user_type, credit):
        """Create an account with its username, two-letter type code (AA, FS, BS, SS)
        and available credit in dollars."""
        self.username = username
        self.user_type = user_type
        self.credit = credit


class GameForSale:
    """One game for sale: a line of the Available Games File."""

    def __init__(self, game_name, seller_username, price):
        """Create a game for sale with its name, the seller's username and its price."""
        self.game_name = game_name
        self.seller_username = seller_username
        self.price = price


class OwnedGame:
    """One game owned by a user: a line of the Games Collection File."""

    def __init__(self, game_name, owner_username):
        """Create an ownership record of 'game_name' by 'owner_username'."""
        self.game_name = game_name
        self.owner_username = owner_username
