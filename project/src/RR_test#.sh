#!/bin/bash
# Test all script for Assignment 1 & 2
# This script should be placed inside project/src
# Usage: ./test.sh (make sure you are in the src directory)

usage() {
  echo "Usage: $0 [-v] [filter_string]"
  echo "  -v    Use valgrind"
  echo "  -h    Show this help message"
}

VALGRIND=0

while getopts "vh" opt; do
  case $opt in
    v) VALGRIND=1 ;;
    h) usage; exit 0 ;;
    *) ;;
  esac
done

shift $((OPTIND - 1))
FILTER=$1

CWD="$(pwd)"
if [[ $CWD != */project/src ]]; then
    echo "Please run this script from the project/src directory"
    exit 1
fi

make style
make clean
make mysh
BINARY="$CWD/mysh"
TMP_DIR="$(mktemp -d)"
PASSED=0
FAILED=0
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
if [ ! -f "$BINARY" ]; then
    echo "Compilation failed!"
    exit 1
fi
ROOT=$(realpath "$CWD/../..")
for TEST_FILE in "$ROOT"/*/test-cases/T_RR30_2.txt "$ROOT"/*/test-cases/T_background.txt; do
    TEST_DIR=$(dirname "$TEST_FILE")
    TEST_FILE=$(basename "$TEST_FILE")
    # skip if "${TEST_FILE%.txt}_result.txt" does not exist
    if [ ! -f "$TEST_DIR/${TEST_FILE%.txt}_result.txt" ]; then
        continue
    fi
    # if filter is set, skip tests that do not contain the filter string
    if [ -n "$FILTER" ] && [[ $TEST_FILE != *"$FILTER"* ]]; then
        continue
    fi
    # setup temp directory for each test
    mkdir -p "$TMP_DIR"
    cp "$TEST_DIR"/* "$TMP_DIR"
    cd "$TMP_DIR" || exit
    if [ $VALGRIND -eq 1 ]; then
        OUTPUT=$(valgrind --leak-check=full -q "$BINARY" <"$TEST_FILE")
    else
        OUTPUT=$("$BINARY" <"$TEST_FILE")
    fi
    EXPECTED=$(<"${TEST_FILE%.txt}_result.txt")
    [[ -f "${TEST_FILE%.txt}_result2.txt" ]] && EXPECTED2=$(<"${TEST_FILE%.txt}_result2.txt") || EXPECTED2=""

    DIFF=$(diff -B -u --label "" --label "" <(echo "$OUTPUT") <(echo "$EXPECTED"))
    if [ -n "$EXPECTED2" ] && [ -n "$DIFF" ]; then
        # if EXPECTED2 exists and first diff failed, try second expected output
        DIFF=$(diff -B -u --label "" --label "" <(echo "$OUTPUT") <(echo "$EXPECTED2"))
    fi
    if [ -z "$DIFF" ]; then
        echo -e "${GREEN}PASS: $TEST_FILE${NC}"
        ((PASSED++))
    else
        echo -e "${RED}FAIL: $TEST_FILE${NC}"
        echo "$DIFF"
        ((FAILED++))
        echo "==============================="
    fi
    rm -rf "$TMP_DIR"
done
cd "$CWD" || exit
if [ $FAILED -eq 0 ]; then
    RED='\033[0;2m' # dim red if all tests pass
    echo 'G1swOzMybdmpKMuK4Zecy4sqKdmIIOKZoSBDb25ncmF0dWxhdGlvbnMhCg==' | base64 -d 
fi
echo -e "\nTest Summary:\n${GREEN}Passed: $PASSED${NC}\n${RED}Failed: $FAILED${NC}\nTotal: $((PASSED + FAILED))"
