/* symbolic expression evaluator
 *
 * needs to implement at least the 
 * following to run the LISP by McCarthy:
 *
 *  quote, atom, eq, cons, car, cdr, cond
 *
 * copyright (C) 2015 A. Carl Douglas
 */

OBJECT * _apply(OBJECT *expr, OBJECT *environ);
OBJECT * _eval(OBJECT *expr, OBJECT *environ);
OBJECT * _evlis(OBJECT *expr, OBJECT *environ);

OBJECT * _eval(OBJECT *expr, OBJECT *environ) {
  if (object_type(expr) == NUMBER) {
    return expr;
  } else if (object_type(expr) == SYMBOL) {

    if (strcmp(symbol_name(expr),"true")==0)
      return TRUE;
    else if (strcmp(symbol_name(expr),"false")==0)
      return FALSE;

    return _lookup(expr, environ);

  } else if (object_type(expr) == PAIR) {

    if (expr == NIL) /* NIL is a pair: '() */
      return NIL;

    if (object_type(_car(expr)) == PAIR) {
      if (strcmp(symbol_name(_car(_car(expr))),"lambda")==0) {
        OBJECT *tmpn = _car(_cdr(_car(expr)));
        OBJECT *tmpv = _cdr(expr);
        OBJECT *closure_env = environ;
        for ( ; tmpv != NIL && tmpn != NIL; 
                tmpv = _cdr(tmpv), tmpn = _cdr(tmpn)) {
          /* note: evaluates the arguments before evaluating the lambda */
          closure_env = _bind(_car(tmpn), _eval(_car(tmpv), closure_env), closure_env);
        }
        return _eval(_car(_cdr(_cdr(_car(expr)))), closure_env);
      }
    } else if (strcmp(symbol_name(_car(expr)), "quote")==0) {
      return _car(_cdr(expr));
    } else if (strcmp(symbol_name(_car(expr)), "atom")==0) {
      OBJECT *tmp = _eval(_car(_cdr(expr)), environ);
      return is_atom(tmp) ? TRUE : FALSE;      
    } else if (strcmp(symbol_name(_car(expr)), "eq")==0) {
      OBJECT *o1 = _eval(_car(_cdr(expr)), environ);
      OBJECT *o2 = _eval(_car(_cdr(_cdr(expr))), environ);
      if (  (object_type(o1)==NUMBER && object_type(o2)==NUMBER && integer(o1)==integer(o2))
         || (object_type(o1)==SYMBOL && object_type(o2)==SYMBOL && symbol_name(o1)==symbol_name(o2))
         || o1 == o2)
        return TRUE;
      return FALSE;
    } else if (strcmp(symbol_name(_car(expr)), "car")==0) {
      OBJECT *tmp = _eval(_car(_cdr(expr)), environ);  
      return _car(tmp);
    } else if (strcmp(symbol_name(_car(expr)), "cdr")==0) {
      OBJECT *tmp = _eval(_car(_cdr(expr)), environ);  
      return _cdr(tmp);
    } else if (strcmp(symbol_name(_car(expr)), "cons")==0) {
      OBJECT *o1 = _eval(_car(_cdr(expr)), environ);
      OBJECT *o2 = _eval(_car(_cdr(_cdr(expr))), environ);
      OBJECT *o3 = _cons(o1, o2);
      return o3;
    } else if (strcmp(symbol_name(_car(expr)), "cond")==0) {
      OBJECT *tmp ;
      for (tmp = _cdr(expr); tmp != NIL; tmp = _cdr(tmp)) {
        OBJECT *o1 = _eval(_car(_car(tmp)), environ);
        if (o1 == TRUE) {
          return _eval(_car(_cdr(_car(tmp))), environ);
        }
      }
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
    else  /* apply */
    {
      OBJECT *proc, *args;
      proc = _eval(_car(expr), environ);
      if (proc == NIL) {
        printf("Error - symbol not found '%s'\n", symbol_name(_car(expr)));
        exit(-1);
      }
      args = _cdr(expr);
      if (object_type(proc) == OPERATOR) {
        /* evaluate the arguments */
        args = _evlis(_cdr(expr), environ);
        return proc->value.primitive(args);
      } else if (object_type(proc) == PAIR) {
        return _eval( _cons(proc, args), environ);
      } else {
        ERR(expr); abort();
      }
    }

  } else {
    ERR(expr); abort();
  }
  return NIL;
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

