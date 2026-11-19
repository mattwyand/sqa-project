# sqa-project

Game Distribution System (Steam-like) for CSCI 3060U.

- **Front End** (C++): `frontend/` holds **Front End A**, the front end chosen for our team in Phase 4.
  It replaced our own Front End, which is still in the git history (up to the Phase 3 merge).
- **Back End** (Python): `backend/` is the overnight batch processor.

## Front End A

```bash
cd frontend/phase2
make                    # builds ./frontend
./frontend data/users.txt data/games.txt data/collection.txt transout.atf
```

Its 147 requirements tests:

```bash
cd frontend/phase3
./run_tests.sh          # builds the Front End, runs every test, then checks the results
```

## Back End

```bash
cd backend
python main.py merged_transactions.txt old_accounts.txt old_games.txt old_collection.txt \
               new_accounts.txt new_games.txt new_collection.txt
```

Failed constraints and fatal errors are printed on the terminal as `ERROR: <msg>` lines.
