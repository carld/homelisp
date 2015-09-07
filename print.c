/* prints a symbolic expression
 *
 * the print function does not call itself recursively,
 * as the intention is to port it to assembly
 *
 * copyright (c) 2015 A. Carl Douglas
 */
OBJECT * _print(OBJECT *exp) {
  OBJECT *expr_stack  = NIL;
  int ldepth = 0, depth = 0;
next:
  if (exp == NIL) {
    printf("()");
    goto pop_frame;
  }

  if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack);
    exp = _car(exp);
    depth++;
    if (depth > ldepth) {
      printf("(");
      ldepth++;
    } else {
      printf(" ");
    }
    goto next;
  }
  
  if (object_type(exp) == SYMBOL) 
    printf("%s", exp->value.symbol);
  else if (object_type(exp) == NUMBER) 
    printf("%d", exp->value.number.integer);
  else if (object_type(exp) == OPERATOR)
    printf("[FUNCTION %p]", (void *)exp);

pop_frame:
  if (expr_stack != NIL) {
    exp = _cdr(_car(expr_stack));
    expr_stack = _cdr(expr_stack);
    depth--;
    if(exp == NIL) {
      printf(")");
      ldepth--;
      goto pop_frame;
    }
  } else {
    exp = NIL;
  }

  if (exp != NIL)
    goto next;

  return NIL;
}
