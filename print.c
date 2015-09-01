
void _print(OBJECT *exp) {
  OBJECT *expr_stack  = NIL;
  int indent = 0;
next:
  if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack);
    exp = _car(exp);
    indent++;
  } else if (object_type(exp) == SYMBOL || object_type(exp) == NUMBER || object_type(exp) == OPERATOR) {

    if (object_type(exp) == SYMBOL) 
      printf("%s ", exp->value.symbol);

    if (object_type(exp) == NUMBER) 
      printf("%d ", exp->value.number.integer);

    if (expr_stack != NIL) {
      exp = _cdr(_car(expr_stack));
      expr_stack = _cdr(expr_stack);
      indent--;
    } else {
      exp = NIL;
    }
  }
  if (exp != NIL) 
    goto next;
  printf("\n");
}

