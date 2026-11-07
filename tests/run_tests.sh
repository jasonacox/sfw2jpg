#!/bin/sh
#
# Regression tests for sfwjpg / pwpjpg. Builds sanitizer-instrumented
# binaries and checks that valid input still converts and that malformed
# input or bad filenames fail cleanly instead of overflowing buffers.
#
# Usage: make check   (or: sh tests/run_tests.sh)

set -u
CC=${CC:-cc}
SRC=$(cd "$(dirname "$0")/.." && pwd)
WORK=$(mktemp -d "${TMPDIR:-/tmp}/sfw2jpg-test.XXXXXX") || exit 1
trap 'rm -rf "$WORK"' EXIT
FAILS=0
SAN="-g -fsanitize=address,undefined -fno-sanitize-recover=all"

$CC $SAN -o "$WORK/sfwjpg" "$SRC/sfwjpg.c" || exit 1
$CC $SAN -o "$WORK/pwpjpg" "$SRC/pwpjpg.c" || exit 1
cp "$SRC/example.sfw" "$WORK/"
cd "$WORK" || exit 1

pass() { echo "ok:   $1"; }
fail() { echo "FAIL: $1"; FAILS=$((FAILS+1)); }

# expect_clean_failure <name> <cmd...>: must exit 1 with no sanitizer report
expect_clean_failure() {
	name=$1; shift
	"$@" > out.txt 2>&1
	rc=$?
	if grep -q -E 'Sanitizer|runtime error' out.txt; then
		fail "$name (memory error)"; sed -n '1,3p' out.txt
	elif [ $rc -ne 1 ]; then
		fail "$name (exit $rc, expected 1)"
	else
		pass "$name"
	fi
}

# 1. valid file converts to a 640x480 JFIF of the expected size
if ./sfwjpg example.sfw example.jpg > out.txt 2>&1 \
	&& [ "$(wc -c < example.jpg | tr -d ' ')" = "43330" ] \
	&& [ "$(od -An -tx1 -N4 example.jpg | tr -d ' \n')" = "ffd8ffe0" ]; then
	pass "example.sfw converts"
else
	fail "example.sfw converts"; cat out.txt
fi

# 2. malformed inputs
OFF=$(od -An -tx1 -v example.sfw | tr -s ' \n' '  ' | awk '{for(i=1;i<NF-3;i++) if($i=="ff"&&$(i+1)=="c8"&&$(i+2)=="ff"&&$(i+3)=="d0"){print i-1; exit}}')
head -c $((OFF + 30)) example.sfw > trunc_header.sfw
expect_clean_failure "truncated after header" ./sfwjpg trunc_header.sfw t.jpg
head -c $((OFF + 8)) example.sfw > trunc_soi.sfw
expect_clean_failure "truncated inside APP0" ./sfwjpg trunc_soi.sfw t.jpg
{ head -c $((OFF + 4)) example.sfw; printf '\377\377'; tail -c +$((OFF + 7)) example.sfw; } > badlen.sfw
expect_clean_failure "oversized marker length" ./sfwjpg badlen.sfw t.jpg

# 3. filenames
expect_clean_failure "short input name" ./sfwjpg ab
LONG=$(printf '%0300d' 0)
expect_clean_failure "300-char input name (sfwjpg)" ./sfwjpg "$LONG"
expect_clean_failure "300-char input name (pwpjpg)" ./pwpjpg "$LONG"
NEAR=$(printf '%0245d' 0)
expect_clean_failure "245-char missing input (sfwjpg)" ./sfwjpg "$NEAR"
expect_clean_failure "245-char missing input (pwpjpg)" ./pwpjpg "$NEAR"

if [ $FAILS -ne 0 ]; then
	echo "$FAILS failure(s)"; exit 1
fi
echo "PASS"
