#!/bin/bash

LISP=./repl

COUNT=0
FAIL=0
PASS=0

function run_test {
  COUNT=$((COUNT + 1))
  RETCODE=$(echo "$1" | $LISP -i init.lsp | grep -c "^$2$")
  if [ $RETCODE = 1 ]; then 
    RESULT="Pass "
    PASS=$((PASS + 1))
  else  
    RESULT="Fail!"
    FAIL=$((FAIL + 1))
  fi
  printf "%03d  %s  %s  ->  %s \n" "$COUNT" "$RESULT" "$1" "$2"
}

# Built-in evaluator tests
run_test "(+ 1 2)" "3"
run_test "(+ 5 (+ 3 2))" "10"
run_test "(quote sym)" "sym"
run_test "(reverse (quote (1 2 3)))" "(3 2 1)"
run_test "(reverse (quote (a b c def)))" "(def c b a)"
run_test "(* 2 4)" "8"
run_test "(/ 10 2)" "5"
run_test "(- 16 8)" "8"
run_test "(car (quote (a b c)))" "a"
run_test "(cdr (quote (a b c)))" "(b c)"
run_test "(quote (a b c(d e f)))" "(a b c (d e f))"
run_test "(quote (a b c(d e f)h i))" "(a b c (d e f) h i)"
run_test "(reverse (quote (a b c(d e f)h i)))" "(i h (d e f) c b a)"
run_test "(* 10 10)" "100"
run_test "(quote (hello world))" "(hello world)"
run_test "(eq 1 2)" "#f"
run_test "(eq 1 1)" "#t"
run_test "(eq (quote hello) (quote world))" "#f"
run_test "(eq (quote hello) (quote hello))" "#t"
run_test "(atom 1)"  "#t"
run_test "(atom (quote (1 2)))" "#f"
run_test "(atom (quote symbol))" "#t"
run_test "(cond ('t 'hello))" "hello"
run_test "(cons 1 (quote (2 3)))" "(1 2 3)"
run_test "(cond ((eq 1 2) (quote first)) ((eq 1 1) (quote second)))" "second"
run_test "(cond ((eq (quote a) (quote b)) (quote first)) ((eq (quote b) (quote b)) (quote second)))" "second"
run_test "#t" "#t"
run_test "#f" "#f"
run_test "(quote ())" "()"
run_test "(eq () ())" "#t"
run_test "(def hello (quote world))" "()"
run_test "(quote (a b c (d e f) g h (i j k)))"  "(a b c (d e f) g h (i j k))"
run_test "((lambda (x y) (* x y) ) 2 3)"  "6"
run_test "((lambda (x)   (cond ((eq x 1)  (quote one)) (#t (quote not_one)))) 1)" "one"
run_test "(append (quote (1 2 3)) (quote (4 5 6)))" "(1 2 3 4 5 6)"
run_test "(pair (quote (1)) (quote (2)))"  "((1 2))"
run_test "(factorial 3)" "6"
run_test "(factorial 4)" "24"
run_test "(factorial 10)" "3628800"
run_test "(list 1 2)" "(1 2)"
run_test "'a" "a"
run_test "'(a b c)" "(a b c)"
run_test "\"hello\"" "hello"

# McCarthy Lisp tests
run_test "(eval (quote (quote 1)) (env))" "1"
run_test "(eval (quote (atom (quote 1))) (env))" "#t"
run_test "(eval (quote (car (quote (a b c)))) (env))" "a"
run_test "(eval (quote (cdr (quote (a b c)))) (env))" "(b c)"
run_test "(eval (quote (assoc (quote a) (quote ((a b))))) (env))" "b"
run_test "(eval (quote test) (quote ((test #t))) )" "#t"
run_test "(eval (quote (cond ('t 'hello))) (env))" "hello"
run_test "(eval '(null? '1) (env))" "#f"
run_test "(eval (quote (reverse (quote (1 2 3)))) (env))" "(3 2 1)"
run_test "(eval 'null? (env))" "(lambda (x) (eq x (quote ())))"

# helper tests
run_test "(map  (lambda (x) (atom x))   (quote (1 2 (quote ()) 3 4)))" "(#t #t #f #t #t)"
run_test "(fibonacci 10)" "55"
run_test "(append (quote (1 2 3)) (quote (4 5 6)))" "(1 2 3 4 5 6)"
run_test "(reverse (quote (1 2 3)))" "(3 2 1)"

printf "\nPassed: %d / %d \nFailed: %d / %d\n" "$PASS" "$COUNT" "$FAIL" "$COUNT"
