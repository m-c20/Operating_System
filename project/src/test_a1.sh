#!/bin/bash
# Test all script for Assignment 1
# This script should be placed inside project/src
# Usage: ./test.sh (make sure you are in the src directory)
make style
make clean
make mysh
CWD="$(pwd)"
BINARY="$CWD/mysh"
TMP_DIR="$(mktemp -d)"
TEST_DIR="$(realpath ../../A1/test-cases)"
PASSED=0
FAILED=0
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
if [ ! -f "$BINARY" ]; then
    echo "Compilation failed!"
    exit 1
fi
for TEST_FILE in "$TEST_DIR"/*.txt; do
    # skip result files
    if [[ $TEST_FILE == *_result.txt ]]; then
        continue
    fi
    TEST_FILE=$(basename "$TEST_FILE")
    # setup temp directory for each test
    mkdir -p "$TMP_DIR"
    cp "$TEST_DIR"/*.txt "$TMP_DIR"
    cd "$TMP_DIR" || exit
    OUTPUT=$("$BINARY" <"$TEST_FILE")
    EXPECTED=$(<"${TEST_FILE%.txt}_result.txt")
    # maybe a better check that actually mirrors the autograder? (ignore whitespace errors)
    # could use diff for it maybe
    if [ "$OUTPUT" = "$EXPECTED" ]; then
        echo -e "${GREEN}PASS: $TEST_FILE${NC}"
        ((PASSED++))
    else
        echo -e "${RED}FAIL: $TEST_FILE${NC}"
        diff -u --label "" --label "" <(echo "$OUTPUT") <(echo "$EXPECTED")
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

