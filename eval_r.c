
OBJECT * _apply(OBJECT *expr, OBJECT *environ);
OBJECT * _eval(OBJECT *expr, OBJECT *environ);
OBJECT * _evlis(OBJECT *expr, OBJECT *environ);

OBJECT * _eval(OBJECT *expr, OBJECT *environ) {
  if (object_type(expr) == NUMBER) {
    return expr;
  } else if (object_type(expr) == SYMBOL) {
    return _lookup(expr, environ);
  } else if (object_type(expr) == PAIR) {

    if (strcmp(symbol_name(_car(expr)), "quote")==0) {
      return _car(_cdr(expr));
    } else if (strcmp(symbol_name(_car(expr)), "lambda")==0) {

    } else {
      return _apply( _eval(_car(expr), environ), _evlis(_cdr(expr), environ) );
    }
  } else {
    ERR(expr);
    abort();
  }
  return NIL;
}

OBJECT * _apply(OBJECT *proc, OBJECT *args) {
  if (object_type(proc) == OPERATOR) {
    return proc->value.primitive(args);
  } else {
    printf("Expected primitive, got %d\n", object_type(proc));
    ERR(proc); ERR(args); abort();
  }
}

OBJECT * _evlis(OBJECT *expr, OBJECT *environ) {
  OBJECT *list = expr;
  for (  ; expr != NIL; expr = _cdr(expr) ) {
    OBJECT *tmp = _car(expr);
    tmp = _eval(tmp, environ);
    _rplaca(expr, tmp);
  }
  return list;
}

