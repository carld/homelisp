/* symbolic expression evaluator
 * needs to implement at least the following to run the LISP by McCarthy:
 *  quote, atom, eq, cons, car, cdr, cond
 *
 * copyright (C) 2015 A. Carl Douglas
 */
OBJECT * _evlis(OBJECT *expr, OBJECT *environ);

OBJECT * _eval(OBJECT *expr, OBJECT *environ) {
  if (object_type(expr) == NUMBER) {
    return expr;
  } else if (object_type(expr) == STRING) {
    return expr;
  } else if (object_type(expr) == SYMBOL) {
    /*  ((atom e) (assoc e a))  */
    /* this evaluator has built-in true and false, McCarthy's uses nil or not nil
     */
    if (strcmp(symbol_name(expr),"#t")==0) return TRUE;
    else if (strcmp(symbol_name(expr),"#f")==0) return FALSE;
    return _lookup(expr, environ);
  } else if (object_type(expr) == PAIR) {
    if (expr == NIL) /* NIL is a pair: '() */
      return NIL;
    else if (object_type(_car(expr)) == PAIR) {
      if (strcmp(symbol_name(_car(_car(expr))),"lambda")==0) {
        OBJECT *tmpn = _car(_cdr(_car(expr)));
        OBJECT *tmpv = _cdr(expr);
        OBJECT *closure_env = environ;
        for ( ; tmpv != NIL && tmpn != NIL; 
                tmpv = _cdr(tmpv), tmpn = _cdr(tmpn)) {
          /* note: evaluates the arguments before evaluating the lambda */
          /* #define _bind(ex,va,en)    _cons(_cons(ex, _cons(va, NIL)), en) */
          OBJECT *val = _eval(_car(tmpv), environ);
          closure_env = _bind(_car(tmpn), val, closure_env);
        }
        return _eval(_car(_cdr(_cdr(_car(expr)))), closure_env);
      }
    } else if (strcmp(symbol_name(_car(expr)), "quote")==0) {
      /* ((eq (car e) (quote quote)) (cadr e)) */
      return _car(_cdr(expr));
    } else if (strcmp(symbol_name(_car(expr)), "atom")==0) {
      OBJECT *tmp = _eval(_car(_cdr(expr)), environ);
      return is_atom(tmp) ? TRUE : FALSE;      
    } else if (strcmp(symbol_name(_car(expr)), "eq")==0) {
    /*
     *  ((eq (car e) 'eq)    (eq     (eval. (cadr e) a)
     *                               (eval. (caddr e) a)))
     */
      OBJECT *o1 = _eval(_car(_cdr(expr)), environ);
      OBJECT *o2 = _eval(_car(_cdr(_cdr(expr))), environ);
      if (  (object_type(o1)==NUMBER && object_type(o2)==NUMBER && integer(o1)==integer(o2))
         || (object_type(o1)==SYMBOL && object_type(o2)==SYMBOL && symbol_name(o1)==symbol_name(o2))
         || o1 == o2  )
        return TRUE;
      return FALSE;
    } else if (strcmp(symbol_name(_car(expr)), "car")==0) {
      return _car(_eval(_car(_cdr(expr)), environ));
    } else if (strcmp(symbol_name(_car(expr)), "cdr")==0) {
      return _cdr(_eval(_car(_cdr(expr)), environ));
    } else if (strcmp(symbol_name(_car(expr)), "cons")==0) {
      OBJECT *o1 = _eval(_car(_cdr(expr)), environ);
      OBJECT *o2 = _eval(_car(_cdr(_cdr(expr))), environ);
      return _cons(o1, o2);
    } else if (strcmp(symbol_name(_car(expr)), "cond")==0) {
      /* ((eq (car e) 'cond)  (evcon (cdr e) a))
       *
       * (def evcon (c a)
       *   (cond ((eval (caar c) a) (eval (cadar c) a))
       *           ('t (evcon (cdr c) a))))
       */
      OBJECT *tmp ;
      for (tmp = _cdr(expr); tmp != NIL; tmp = _cdr(tmp)) {
        OBJECT *o1 = _eval(_car(_car(tmp)), environ);
        /* an atom (sym or number) is true to be compatible with McCarthy Lisp */
        if (o1 != NIL && o1 != FALSE) o1 = TRUE;
        if (o1 == TRUE) {
          return _eval(_car(_cdr(_car(tmp))), environ);
        }
      }
      return NIL;
    } else if (strcmp(symbol_name(_car(expr)), "def")==0) {
      OBJECT *lambda = _cons(make_symbol("lambda"), _cdr(_cdr(expr)));
      _append(environ, _cons( _cons(_car(_cdr(expr)), _cons(lambda, NIL)), NIL));
      return NIL; /* what should binding to the environment return? */
    } else if (strcmp(symbol_name(_car(expr)), "env")==0) {
      return environ;
    } else if (strcmp(symbol_name(_car(expr)), "lambda")==0) {
      /* does not evaluate here */
      return expr;
    }
    else  /* apply? */
    {
      OBJECT *proc = _eval(_car(expr), environ);
      if (object_type(proc) == OPERATOR) {
        OBJECT *args = _evlis(_cdr(expr), environ);
        return proc->value.primitive(args);
      } else if (object_type(proc) == PAIR) {
        return _eval( _cons(proc, _cdr(expr)), environ);
      }
    }
  } /* type == PAIR */
  return NIL; /* this is an error condition */
}

OBJECT * _evlis(OBJECT *expr, OBJECT *environ) {
  OBJECT *list = NIL;
  for (  ; expr != NIL; expr = _cdr(expr) ) {
    OBJECT *tmp = _car(expr);
    tmp = _eval(tmp, environ);
    list = _cons(tmp, list);
  }
  return _reverse_in_place(list);
}

