/* prints a symbolic expression
 *
 * the print function does not call itself recursively,
 * as the intention is to port it to assembly
 *
 * copyright (c) 2015 A. Carl Douglas
 */
OBJECT * _print(OBJECT *exp) {
  OBJECT *expr_stack  = NIL;
  OBJECT *result = make_string("",0);
  int ldepth = 0, depth = 0;
next:
  if (exp == NIL) {
    result = string_cat(result, "()");
    goto pop_frame;
  }

  if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack);
    exp = _car(exp);
    depth++;
    if (depth > ldepth) {
      result = string_cat(result, "(");
      ldepth++;
    } else {
      result = string_cat(result, " ");
    }
    goto next;
  }
  
  if (object_type(exp) == SYMBOL) {
    result = string_cat(result, exp->value.symbol);
  } else if (object_type(exp) == NUMBER)  {
    char buffer[128];
    snprintf(buffer, 128, "%d", exp->value.number.integer);
    result = string_cat(result, buffer);
  } else if (object_type(exp) == OPERATOR) {
    char buffer[128];
    snprintf(buffer, 128, "[FUNCTION %p]", (void *)exp);
    result = string_cat(result, buffer);
  } else if (object_type(exp) == STRING) {
    result = string_cat(result, string(exp));
  }

pop_frame:
  if (expr_stack != NIL) {
    exp = _cdr(_car(expr_stack));
    expr_stack = _cdr(expr_stack);
    depth--;
    if(exp == NIL) {
      result = string_cat(result, ")");
      ldepth--;
      goto pop_frame;
    }
  } else {
    exp = NIL;
  }

  if (exp != NIL)
    goto next;
  
  printf("%s", string(result));

  return result;
}
