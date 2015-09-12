/* prints a symbolic expression
 * copyright (c) 2015 A. Carl Douglas
 */
OBJECT * _print(OBJECT *exp) {
  char buffer[128];
  OBJECT *expr_stack  = NIL;
  OBJECT *result = make_string("",0);
  int ldepth = 0, depth = 0; /* ldepth is the current list depth */
print_object:
  if (exp == NIL) {
    result = string_cat(result, make_string("()",0));
  } else if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack); /* push onto stack */
    exp = _car(exp);
    depth++;
    if (depth > ldepth) {
      result = string_cat(result, make_string("(",0));
      ldepth++;
    } else {
      result = string_cat(result, make_string(" ",0));
    }
    goto print_object;
  } else if (object_type(exp) == SYMBOL) {
    result = string_cat(result, make_string(symbol_name(exp),0));
  } else if (object_type(exp) == NUMBER)  {
    snprintf(buffer, 128, "%d", exp->value.number.integer);
    result = string_cat(result, make_string(buffer,0));
  } else if (object_type(exp) == OPERATOR) {
    snprintf(buffer, 128, "[FUNCTION %p]", (void *)exp);
    result = string_cat(result, make_string(buffer,0));
  } else if (object_type(exp) == STRING) {
    result = string_cat(result, exp);
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
      result = string_cat(result, make_string(")",0));
      ldepth--;
      goto next_object;
    } else {
      goto print_object;
    }
  }
  printf("%s", string(result));
  return result;
}
