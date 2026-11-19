"""
back_end.py - The overnight batch run: load the old files, apply every transaction,
write the new files.
"""

from data_files import UserAccountsFile, AvailableGamesFile, GamesCollectionFile, TransactionFile
from system_state import SystemState


class BackEnd:
    """Runs one overnight update from the old data files to the new ones."""

    def __init__(self, logger):
        """Create the Back End; all errors are reported through 'logger'."""
        self.logger = logger
        self.accounts_file = UserAccountsFile(logger)
        self.games_file = AvailableGamesFile(logger)
        self.collection_file = GamesCollectionFile(logger)
        self.transaction_file = TransactionFile(logger)

    def run(self, file_paths):
        """Do the whole update. 'file_paths' is a BackEndFilePaths naming every
        input and output file."""
        state = self.load_old_files(file_paths)
        transactions = self.transaction_file.read(file_paths.merged_transactions)
        self.apply_transactions(state, transactions)
        self.write_new_files(state, file_paths)

    def load_old_files(self, file_paths):
        """Read the old accounts, games and collection files into a SystemState."""
        return SystemState(self.accounts_file.read(file_paths.old_accounts),
                           self.games_file.read(file_paths.old_games),
                           self.collection_file.read(file_paths.old_collection))

    def apply_transactions(self, state, transactions):
        """Apply each transaction to 'state', in the order they were made."""
        for transaction in transactions:
            transaction.apply(state, self.logger)

    def write_new_files(self, state, file_paths):
        """Write the updated SystemState out as the new accounts, games and collection files."""
        self.accounts_file.write(file_paths.new_accounts, state.accounts.values())
        self.games_file.write(file_paths.new_games, state.games_for_sale)
        self.collection_file.write(file_paths.new_collection, state.owned_games)


class BackEndFilePaths:
    """The seven file names the Back End is given on the command line."""

    def __init__(self, merged_transactions, old_accounts, old_games, old_collection,
                 new_accounts, new_games, new_collection):
        """Store the path of each input and output file."""
        self.merged_transactions = merged_transactions
        self.old_accounts = old_accounts
        self.old_games = old_games
        self.old_collection = old_collection
        self.new_accounts = new_accounts
        self.new_games = new_games
        self.new_collection = new_collection
