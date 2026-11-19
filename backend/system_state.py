"""
system_state.py - Holds all accounts, games for sale and owned games in memory.

The Back End loads the old data files into a SystemState, applies every transaction
to it, and then writes the SystemState out as the new data files.
"""

from models import UserAccount, GameForSale, OwnedGame


class SystemState:
    """All data the Back End is updating: accounts, games for sale and owned games."""

    def __init__(self, accounts, games_for_sale, owned_games):
        """Create the state from lists read from the old data files.
        Accounts are kept in a dict by username; games keep their file order."""
        self.accounts = {account.username: account for account in accounts}
        self.games_for_sale = list(games_for_sale)
        self.owned_games = list(owned_games)

    # ---- Accounts ----

    def find_account(self, username):
        """Return the UserAccount named 'username', or None if there isn't one."""
        return self.accounts.get(username)

    def add_account(self, username, user_type, credit):
        """Add a new UserAccount."""
        self.accounts[username] = UserAccount(username, user_type, credit)

    def remove_account(self, username):
        """Remove the account named 'username' and cancel its games for sale."""
        del self.accounts[username]
        self.games_for_sale = [game for game in self.games_for_sale
                               if game.seller_username != username]
        # TODO (Phase 5): decide whether a deleted user's owned games should also be
        # removed from the games collection; for now they are kept.

    # ---- Games for sale ----

    def add_game_for_sale(self, game_name, seller_username, price):
        """Put a new game up for sale."""
        self.games_for_sale.append(GameForSale(game_name, seller_username, price))

    # ---- Owned games ----

    def add_owned_game(self, game_name, owner_username):
        """Record that 'owner_username' now owns 'game_name'."""
        self.owned_games.append(OwnedGame(game_name, owner_username))
