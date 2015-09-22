/* structures and functions for representing symbolic expressions .
 * an object structure can store either an atomic type:
 *   a symbol or a number, or a Cons cell.
 *
 * copyright (C) 2015 A. Carl Douglas 
 */
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "lisp.h"
/*  
 * For now using the garbage collector shipped with my Operating System:
 *  man 3 gc
 *  Boehm, H., and M. Weiser, "Garbage Collection in an Uncooperative Environment", "Software Practice &  Experience",
 *  September 1988, pp. 807-820.
 */
#include <gc.h>

OBJECT _NIL   = { PAIR,   { NULL } }; 
OBJECT _TRUE  = { SYMBOL, { "#t" } }; 
OBJECT _FALSE = { SYMBOL, { "#f" } };

OBJECT *_interned_syms = NIL;

OBJECT * _object_malloc (int type)  { 
  OBJECT *obj = GC_MALLOC(sizeof(OBJECT));
  obj->type = type; return obj; 
}

OBJECT * _cons(OBJECT *car, OBJECT *cdr) {
  OBJECT *obj = _object_malloc(PAIR);
  obj->value.pair.car = car;
  obj->value.pair.cdr = cdr;
  return obj;
}

/* in place reverse */
OBJECT * _reverse_in_place(OBJECT *expr) {
  OBJECT *tmp, *revexpr = NIL;
  while (expr != NIL) {
    tmp = _cdr(expr);
    _rplacd(expr, revexpr);
    revexpr = expr; 
    expr = tmp;
  }
  return revexpr;
}

/* (def assoc (x y)
 *   (cond ((eq (caar y) x) (cadar y))
 *           (true (assoc x (cdr y)))))
 */
OBJECT * _lookup(OBJECT *expr /* x */, OBJECT *env /* y */) {
  for ( ; env != NIL ; env = _cdr(env) ) {
    if (symbol_name(expr) == symbol_name( _car(_car(env)))) {
      return _car(_cdr(_car(env)));
    }
  }
  return NIL;
}

OBJECT * make_number_i(int integer) {
  OBJECT *obj = _object_malloc(NUMBER);
  obj->value.number.integer = integer;
  return obj;
}

OBJECT * make_number(const char *token) {
  OBJECT *obj = _object_malloc(NUMBER);
  char *dec   = strchr(token, '.');
  if (dec) {
    *dec = '\0';
    obj->value.number.fraction = atoi(dec+1);
  }
  obj->value.number.integer  = atoi(token); 
  return obj;
}

OBJECT * make_primitive(prim_op *pp) {
  OBJECT *obj = _object_malloc(OPERATOR);
  obj->value.primitive = (OBJECT * (*) (OBJECT *, OBJECT *))pp;
  return obj;
}

/* this helps passing around FILE * etc */
OBJECT * make_pointer(void * ptr) {
  OBJECT *obj = _object_malloc(POINTER);
  obj->value.ptr = ptr;
  return obj;
}

OBJECT * make_symbol(const char *symbol) {
  OBJECT *obj  = _interned_syms;
  size_t len = strlen(symbol) + 1;
  char * storage = 0;
  for ( ; obj != NIL; obj = _cdr(obj)) {
    if (strcmp(symbol, symbol_name(_car(obj))) == 0) {
      return _car(obj);
    }
  }
  storage = GC_MALLOC(len);
  memcpy(storage, symbol, len);
  obj = _object_malloc(SYMBOL);
  obj->value.symbol = storage;
  obj->size = len;
  _interned_syms = _cons(obj, _interned_syms);
  return obj;
}

/* if the length parameter is less than the length of 
 * the string the actual string length will be used
 */
OBJECT * make_string(const char *str, size_t length) {
  OBJECT *obj = _object_malloc(STRING);
  size_t len = strlen(str) + 1;
  obj->size = length > len ? length : len;
  obj->value.string = GC_MALLOC(obj->size);
  memcpy(obj->value.string, str, len);
  return obj;
}

/* warning this mutates the first parameter */
OBJECT * _append(OBJECT *exp1, OBJECT *exp2) {
  if (exp1 != NIL) {
    OBJECT *tmp = exp1;
    for( ; _cdr(tmp) != NIL; tmp = _cdr(tmp) )
         ; /* walk to end of list */
    _rplacd(tmp, exp2);
    return exp1;
  }
  return exp2;
}

const char * _strcat_alloc(const char *str1, const char *str2) {
  size_t len1 = strlen(str1), len2 = strlen(str2);
  char * str3 = GC_MALLOC(len1 + len2 + 1); /* strlen exludes the NULL trailing byte */
  memcpy((void *) str3, (void *)str1, len1);
  memcpy((void *) (str3+len1), (void *)str2, len2+1);
  return str3;
}

OBJECT * string_cat(OBJECT *args, OBJECT *env) {
  return make_string( _strcat_alloc(string(_car(args)), string(_car(_cdr(args)))), 0);
}
