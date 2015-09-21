/* built-in functions that can be applied during evaluation of a symbolic expression
 * copyright (c) 2015 A. Carl Douglas
 */
#include<stdio.h>
#include"lisp.h"
#define DEFMATH(op,name) \
OBJECT * name (OBJECT *args) { \
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
OBJECT * name (OBJECT *args) { \
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

/* print needs a wrapper, it only prints it's first parameter */
OBJECT * prim_print(OBJECT *exp) {
  return _print(_car(exp));
}

OBJECT * prim_debug(OBJECT *exp) {
  return debug(_car(exp));
}

OBJECT * prim_read(OBJECT *exp) {
  return _read(_car(exp), 1);
}

OBJECT * prim_newline(OBJECT *exp) {
  printf("\n");
  return NIL;
}

OBJECT * prim_string_append(OBJECT *exp) {
  return string_cat(_car(exp), _car(_cdr(exp)));
}

OBJECT * prim_eval(OBJECT *exp) {
  return _eval(_car(exp), _car(_cdr(exp)));
}

