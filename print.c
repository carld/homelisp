/* prints a symbolic expression
 * copyright (c) 2015 A. Carl Douglas
 */
#include <stdio.h>
#include "lisp.h"
OBJECT * _print(OBJECT *exp, OBJECT *env) {
  char buffer[128];
  OBJECT *expr_stack  = NIL;
  OBJECT *result = make_string("",0);
  int ldepth = 0, depth = 0; /* ldepth is the current list depth */
print_object:
  if (exp == NIL) {
    result = string_cat(_cons(result, _cons(make_string("()",0), NIL)), env);
  } else if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack); /* push onto stack, come back here */
    exp = _car(exp);
    depth++;
    if (depth > ldepth) {
      result = string_cat(_cons(result, _cons(make_string("(",0), NIL)), env);
      ldepth++;
    } else {
      result = string_cat(_cons(result, _cons(make_string(" ",0), NIL)), env);
    }
    goto print_object;
  } else if (object_type(exp) == SYMBOL) {
    result = string_cat(_cons(result, _cons(make_string(symbol_name(exp),0), NIL)), env);
  } else if (object_type(exp) == NUMBER)  {
    snprintf(buffer, 128, "%d", exp->value.number.integer);
    result = string_cat(_cons(result, _cons(make_string(buffer,0), NIL)), env);
  } else if (object_type(exp) == OPERATOR) {
    snprintf(buffer, 128, "[FUNCTION %p]", (void *)exp);
    result = string_cat(_cons(result, _cons(make_string(buffer,0), NIL)), env);
  } else if (object_type(exp) == STRING) {
    result = string_cat(_cons(result, _cons(exp, NIL)), env);
  } else {
    printf("cannot print object (type %d)\n", exp->type);
    exit(1);
  }
next_object:
  if (expr_stack != NIL) {
    exp = _cdr(_car(expr_stack));
    expr_stack = _cdr(expr_stack); /* pop from stack */
    depth--;
    if(exp == NIL) {
      result = string_cat(_cons(result, _cons(make_string(")",0), NIL)), env);
      ldepth--;
      goto next_object;
    } else {
      goto print_object;
    }
  }
  printf("%s", string(result));
  return result;
}
