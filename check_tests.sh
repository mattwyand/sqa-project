#!/bin/bash
# =============================================================================
# check_tests.sh - compares the actual outputs from run_tests.sh with the expected outputs
#
# For every test NN_name it does two comparisons with diff:
#   (--strip-trailing-cr makes Windows CRLF line endings compare equal to Unix LF)
#   1. Daily transaction file: actual/NN_name_actual.atf  vs  expected/NN_name_expected.etf
#      (if a test has no expected .etf, e.g. a failed login with no logout, the program
#       must NOT have written a transaction file)
#   2. Terminal output:        actual/NN_name_actual.out  vs  expected/NN_name_expected.out
#
# A test PASSES only if both comparisons match. The differences for each failing test are
# printed, and a copy of the whole report is saved to TestResults/run_<date>_<time>.txt so
# every test-suite execution is kept for the failure log.
#
# Usage (from the repository root, after ./run_tests.sh):   ./check_tests.sh
# =============================================================================

OUTPUT_ROOT="TestCases/Outputs"
RESULTS_DIR="TestResults"
mkdir -p "$RESULTS_DIR"
report_file="$RESULTS_DIR/run_$(date +%Y-%m-%d_%H%M%S).txt"

pass_count=0
fail_count=0
failed_tests=""

# Everything inside this block is shown on screen AND saved to the report file
{
for expected_terminal in "$OUTPUT_ROOT"/*/expected/*_expected.out; do
    test_name=$(basename "$expected_terminal" _expected.out)
    category_dir=$(dirname "$(dirname "$expected_terminal")")
    expected_transactions="$category_dir/expected/${test_name}_expected.etf"
    actual_terminal="$category_dir/actual/${test_name}_actual.out"
    actual_transactions="$category_dir/actual/${test_name}_actual.atf"
    test_passed=true

    echo "checking outputs of test $test_name"

    # ---- Comparison 1: daily transaction file ----
    if [ -f "$expected_transactions" ]; then
        if [ ! -f "$actual_transactions" ]; then
            echo "  [transaction file] FAIL - no transaction file was written"
            test_passed=false
        elif ! diff --strip-trailing-cr "$actual_transactions" "$expected_transactions" > /tmp/tx_diff.txt; then
            echo "  [transaction file] FAIL - differences (< actual, > expected):"
            sed 's/^/      /' /tmp/tx_diff.txt
            test_passed=false
        fi
    elif [ -f "$actual_transactions" ]; then
        echo "  [transaction file] FAIL - a transaction file was written but none was expected"
        test_passed=false
    fi

    # ---- Comparison 2: terminal output ----
    if [ ! -f "$actual_terminal" ]; then
        echo "  [terminal output]  FAIL - no terminal output saved (did run_tests.sh run?)"
        test_passed=false
    elif ! diff --strip-trailing-cr "$actual_terminal" "$expected_terminal" > /tmp/term_diff.txt; then
        echo "  [terminal output]  FAIL - differences (< actual, > expected):"
        sed 's/^/      /' /tmp/term_diff.txt | head -40
        test_passed=false
    fi

    if $test_passed; then
        echo "  PASS"
        pass_count=$((pass_count + 1))
    else
        fail_count=$((fail_count + 1))
        failed_tests="$failed_tests $test_name"
    fi
done

echo ""
echo "================ SUMMARY ================"
echo "Passed: $pass_count   Failed: $fail_count"
for name in $failed_tests; do echo "  FAILED: $name"; done
} 2>&1 | tee "$report_file"

echo "Report saved to $report_file"
