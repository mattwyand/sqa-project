# sqa-project

Game Distribution System (Steam-like) for CSCI 3060U. The Front End is in C++ (`frontend/frontend/`).

## Build and run the Front End

```bash
g++ -std=c++17 -Wall -o distribution-system frontend/frontend/*.cpp
./distribution-system currentaccounts.txt availablegames.txt gamescollection.txt transout.atf
```

Transactions are read from standard input (one per line). The prompts and messages go to standard output.
The daily transaction file (`transout.atf`) is written at every logout.

## Run the tests (Phase 3)

```bash
./run_tests.sh      # builds the program and runs all 48 tests
./check_tests.sh    # diffs actual vs expected terminal output and daily transaction file
```

- Inputs: `TestCases/Inputs/<type>/input/NN_name_input.txt`
- Expected: `TestCases/Outputs/<type>/expected/NN_name_expected.out` / `.etf`
- Actual (generated): `TestCases/Outputs/<type>/actual/NN_name_actual.out` / `.atf`
- Starting data for every test: `TestCases/data/`
- Saved report of each test-suite run: `TestResults/`
