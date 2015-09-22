/* symbolic expression evaluator
 * needs to implement at least the following to run the LISP by McCarthy:
 *   quote, atom, eq, cons, car, cdr, cond
 *
 * copyright (C) 2015 A. Carl Douglas
 */
#include <string.h>
#include "lisp.h"
OBJECT * _eval(OBJECT *expr, OBJECT *environ) {
  if (object_type(expr) == NUMBER) {
    return expr;
  } else if (object_type(expr) == STRING) {
    return expr;
  } else if (object_type(expr) == SYMBOL) {
    return _lookup(expr, environ); /*  ((atom e) (assoc e a))  */
  } else if (object_type(expr) == PAIR) {
    if (expr == NIL) /* NIL is a pair: '() */
      return NIL;
    else if (object_type(_car(expr)) == PAIR) {
      if (strcmp(symbol_name(_car(_car(expr))),"lambda")==0) {
        OBJECT *tmpn = _car(_cdr(_car(expr)));
        OBJECT *tmpv = _cdr(expr);
        OBJECT *closure_env = environ;
        for ( ; tmpv != NIL && tmpn != NIL; tmpv = _cdr(tmpv), tmpn = _cdr(tmpn)) {
          closure_env = _bind(_car(tmpn), _eval(_car(tmpv), environ), closure_env); /* REC */
        }
        return _eval(_car(_cdr(_cdr(_car(expr)))), closure_env); /* REC */
      }
    } else if (strcmp(symbol_name(_car(expr)), "quote")==0) {
      return _car(_cdr(expr)); /* ((eq (car e) (quote quote)) (cadr e)) */
    } else if (strcmp(symbol_name(_car(expr)), "atom")==0) {
      return is_atom( _eval(_car(_cdr(expr)), environ) ) ? TRUE : FALSE;       /* REC */
    } else if (strcmp(symbol_name(_car(expr)), "eq")==0) {
    /*  ((eq (car e) 'eq)    (eq     (eval. (cadr e) a)
     *                               (eval. (caddr e) a)))
     */
      OBJECT *o1 = _eval(_car(_cdr(expr)), environ); /* REC */
      OBJECT *o2 = _eval(_car(_cdr(_cdr(expr))), environ); /* REC */
      if (  (object_type(o1)==NUMBER && object_type(o2)==NUMBER && integer(o1)==integer(o2))
         || (object_type(o1)==SYMBOL && object_type(o2)==SYMBOL && symbol_name(o1)==symbol_name(o2))
         || o1 == o2  )
        return TRUE;
      return FALSE;
    } else if (strcmp(symbol_name(_car(expr)), "car")==0) {
      return _car(_eval(_car(_cdr(expr)), environ));  /* REC */
    } else if (strcmp(symbol_name(_car(expr)), "cdr")==0) {
      return _cdr(_eval(_car(_cdr(expr)), environ));  /* REC */
    } else if (strcmp(symbol_name(_car(expr)), "cons")==0) {
      OBJECT *o1 = _eval(_car(_cdr(expr)), environ); /* REC */
      OBJECT *o2 = _eval(_car(_cdr(_cdr(expr))), environ); /* REC */
      return _cons(o1, o2);
    } else if (strcmp(symbol_name(_car(expr)), "cond")==0) {
      /* ((eq (car e) 'cond)  (evcon (cdr e) a))
       * (def evcon (c a)
       *   (cond ((eval (caar c) a) (eval (cadar c) a))
       *         ('t (evcon (cdr c) a))))
       */
      OBJECT *tmp ;
      for (tmp = _cdr(expr); tmp != NIL; tmp = _cdr(tmp)) {
        OBJECT *o1 = _eval(_car(_car(tmp)), environ); /* REC */
        if (o1 != NIL && o1 != FALSE) {  /* any atom (sym or number) is true */
          return _eval(_car(_cdr(_car(tmp))), environ); /* REC */
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
      return expr;  /* function passed by value, does not evaluate here */
    } else  /* apply */ {
      OBJECT *proc = _eval(_car(expr), environ);  /* REC */
      if (object_type(proc) == OPERATOR) {
        OBJECT *args = _cdr(expr);
        OBJECT *list = NIL;
        for (  ; args != NIL; args = _cdr(args) ) { /* evlist */
          list = _cons( _eval(_car(args), environ), list); /* REC */
        }
        return proc->value.primitive( _reverse_in_place(list), environ );
      } else if (object_type(proc) == PAIR) {
        return _eval( _cons(proc, _cdr(expr)), environ);  /* REC */
      }
    }
  } /* type == PAIR */
  return NIL; /* this is an error condition */
}
