/* built-in functions that can be applied during evaluation of a symbolic expression
 * copyright (c) 2015 A. Carl Douglas
 */
#include<stdio.h>
#include"lisp.h"
#define DEFMATH(op,name) \
OBJECT * name (OBJECT *args, OBJECT *env) { \
  int result = integer(_car(args)); \
  for ( args = _cdr(args); args != NIL; args = _cdr(args)) \
    result = result op integer(_car(args)); \
  return make_number_i(result); \
}

DEFMATH(+,prim_add)
DEFMATH(-,prim_subtract)
DEFMATH(*,prim_multiply)
DEFMATH(/,prim_divide)

#undef DEFMATH

#define DEFLOGIC(op,name) \
OBJECT * name (OBJECT *args, OBJECT *env) { \
  int result = integer(_car(args)) op integer(_car(_cdr(args)));\
  return result == 0 ? FALSE : TRUE; \
}

DEFLOGIC(=,prim_equals)
DEFLOGIC(!=,prim_not_equals)
DEFLOGIC(<,prim_less_than)
DEFLOGIC(>,prim_greater_than)
DEFLOGIC(>=,prim_greater_than_equals)
DEFLOGIC(<=,prim_less_than_equals)

#undef DEFLOGIC

OBJECT * prim_read(OBJECT *exp, OBJECT *env) {
  return _read(_car(exp), 1);
}

OBJECT * prim_newline(OBJECT *exp, OBJECT *env) {
  printf("\n");
  return NIL;
}

