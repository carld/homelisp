
OBJECT * _apply(OBJECT *expr, OBJECT *environ);
OBJECT * _eval(OBJECT *expr, OBJECT *environ);
OBJECT * _evlis(OBJECT *expr, OBJECT *environ);

OBJECT * _eval(OBJECT *expr, OBJECT *environ) {
  if (object_type(expr) == NUMBER) {
    return expr;
  } else if (object_type(expr) == SYMBOL) {
    return _lookup(expr, environ);
  } else if (object_type(expr) == PAIR) {
    return _apply(expr, environ);
  } else {
    ERR(expr);
    abort();
  }
  return NIL;
}

OBJECT * _apply(OBJECT *expr, OBJECT *environ) {
  OBJECT *proc = _eval(_car(expr), environ);
  if (object_type(proc) == OPERATOR) {
    return proc->value.primitive(_evlis(_cdr(expr), environ));
  } else {
    ERR(expr);
    abort();
  }
}

OBJECT * _evlis(OBJECT *expr, OBJECT *environ) {
  OBJECT *list = expr;
  for (  ; expr != NIL; expr = _cdr(expr) ) {
    OBJECT *tmp = _car(expr);
    _rplaca(expr, _eval(tmp, environ));
  }
  return list;
}

