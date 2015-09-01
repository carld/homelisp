#!/bin/bash

LISP=./repl

COUNT=0

function run_test {
  COUNT=$((COUNT + 1))
  RETCODE=$(echo "$1" | $LISP | grep -c "$2")
  if [ $RETCODE = 1 ]; then 
    RESULT="PASS "
  else  
    RESULT="FAIL!"
  fi
  printf '%03d  %s  %s  ->  %s \n' "$COUNT" "$RESULT" "$1" "$2"
}

run_test '(+ 1 2)' '3'
run_test '(+ 1 2 (+ 3 2) 2)' '10'
run_test '(quote sym)' 'sym'
run_test '(reverse (quote (1 2 3)))' '3 2 1'
run_test '(reverse (quote (a b c def)))' 'def c b a'
run_test '(* 2 4)' '8'
run_test '(/ 10 2)' '5'
run_test '(- 16 8)' '8'
run_test '(car (quote (a b c)))' 'a'
run_test '(cdr (quote (a b c)))' 'b c'
run_test '(quote (a b c ( d e f ) ) )' '(a b c (d e f))'

