#!/bin/bash
# =============================================================================
# run_tests.sh - Phase 3 test runner for the Front End
#
# What it does:
#   1. Compiles the Front End into ./distribution-system
#   2. For every test input file   TestCases/Inputs/<category>/input/NN_name_input.txt
#      it runs the program once, feeding the input file as standard input, and saves:
#        - the terminal output        -> TestCases/Outputs/<category>/actual/NN_name_actual.out
#        - the daily transaction file -> TestCases/Outputs/<category>/actual/NN_name_actual.atf
#
# Every test starts from the same three data files in TestCases/data/. They are copied
# into a temporary folder before each test, so one test can never change the data used
# by the next test.
#
# Usage (from the repository root):   ./run_tests.sh
# Then check the results with:        ./check_tests.sh
# =============================================================================

PROGRAM="./distribution-system"          # compiled Front End
SOURCE_DIR="frontend/frontend"           # where the .cpp/.h files live
INPUT_ROOT="TestCases/Inputs"            # test inputs, one folder per category
OUTPUT_ROOT="TestCases/Outputs"          # expected/ and actual/ folders per category
DATA_DIR="TestCases/data"                # starting data files shared by every test
TIME_LIMIT=5                             # seconds before a hung test is stopped
MAX_OUTPUT_BYTES=20000                   # cap on saved terminal output (stops a runaway loop filling the disk)

# ---- Step 1: build the program ----
echo "Compiling Front End..."
if ! g++ -std=c++17 -Wall -o "$PROGRAM" "$SOURCE_DIR"/*.cpp; then
    echo "Build failed - no tests were run."
    exit 1
fi

# ---- Step 2: run every test ----
test_count=0
for input_file in "$INPUT_ROOT"/*/input/*_input.txt; do
    test_name=$(basename "$input_file" _input.txt)                 # e.g. 27_buy_valid_game
    category=$(basename "$(dirname "$(dirname "$input_file")")")  # e.g. buy
    actual_dir="$OUTPUT_ROOT/$category/actual"
    mkdir -p "$actual_dir"

    actual_terminal="$actual_dir/${test_name}_actual.out"
    actual_transactions="$actual_dir/${test_name}_actual.atf"
    rm -f "$actual_terminal" "$actual_transactions"   # clear results from the last run

    # Fresh copy of the starting data for this test only
    work_dir=$(mktemp -d)
    cp "$DATA_DIR"/*.txt "$work_dir"/

    echo "running test $test_name"
    timeout "$TIME_LIMIT" "$PROGRAM" \
        "$work_dir/currentaccounts.txt" \
        "$work_dir/availablegames.txt" \
        "$work_dir/gamescollection.txt" \
        "$actual_transactions" \
        < "$input_file" 2>&1 | head -c "$MAX_OUTPUT_BYTES" > "$actual_terminal"
    exit_status=${PIPESTATUS[0]}
    if [ "$exit_status" -eq 124 ]; then
        echo "*** TIMEOUT: program was still running after ${TIME_LIMIT}s ***" >> "$actual_terminal"
    elif [ "$exit_status" -ne 0 ]; then
        # 141 = output cap reached (program kept printing); anything else = crash / error exit
        echo "*** program ended abnormally (exit status $exit_status) ***" >> "$actual_terminal"
    fi

    rm -rf "$work_dir"
    test_count=$((test_count + 1))
done

echo "Ran $test_count tests. Now run ./check_tests.sh to compare against expected outputs."
