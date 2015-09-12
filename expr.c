/* structures and functions for representing symbolic expressions .
 * an object structure can store either an atomic type:
 *   a symbol or a number, or a Cons cell.
 *
 * copyright (C) 2015 A. Carl Douglas 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*  
 * For now using the garbage collector shipped with my Operating System:
 *  man 3 gc
 *  Boehm, H., and M. Weiser, "Garbage Collection in an Uncooperative Environment", "Software Practice &  Experience",
 *  September 1988, pp. 807-820.
 */
#include <gc.h>

enum { PAIR = 1, NUMBER = 2, SYMBOL = 3, OPERATOR = 4, POINTER = 5, STRING = 6 };

typedef struct object OBJECT;

typedef OBJECT * (*prim_op) (OBJECT *);

struct object {
  int type;
  union {
    const char * symbol;
    struct { int integer; int fraction; } number;
    struct { OBJECT * car; OBJECT * cdr; } pair;
    prim_op primitive;
    void * ptr;
    char * string;
  } value;
  size_t size; /* for string length etc */
};

OBJECT _NIL   = { PAIR,   { NULL, NULL } }; 
OBJECT _TRUE  = { SYMBOL, { "#t" } }; 
OBJECT _FALSE = { SYMBOL, { "#f" } };

#define NIL     ((OBJECT *) &_NIL) 
#define TRUE    ((OBJECT *) &_TRUE)
#define FALSE   ((OBJECT *) &_FALSE)

OBJECT *_interned_syms = NIL;

#define is_nil(x)       (x == NIL)
#define is_true(x)      (x == TRUE)
#define is_pair(x)      (x->type == PAIR)
#define is_atom(x)      (x->type == SYMBOL || x->type == NUMBER)
#define symbol_name(x)  (x->value.symbol)
#define integer(x)      (x->value.number.integer)
#define string(x)       (x->value.string)
#define pointer(x)      (x->value.ptr)
#define object_type(x)  (x->type)
#define _cdr(x)         (x->value.pair.cdr)
#define _car(x)         (x->value.pair.car)
#define _rplaca(x,y)    (x->value.pair.car = y)
#define _rplacd(x,y)    (x->value.pair.cdr = y)

#define _bind(ex,va,en)    _cons(_cons(ex, _cons(va, NIL)), en)

OBJECT * _object_malloc (int type)  { 
  OBJECT *obj = GC_MALLOC(sizeof(OBJECT));
  obj->type = type; return obj; 
}

/* this cons has a feature where if both car and cdr are NIL it returns NIL
 *   cons(NIL,NIL) = ()
 */
OBJECT * _cons(OBJECT *car, OBJECT *cdr) {
  if (car == NULL && cdr == NULL)
    return NIL;
  else {
    OBJECT *obj = _object_malloc(PAIR);
    obj->value.pair.car = car;
    obj->value.pair.cdr = cdr;
    return obj;
  }
}

/* in place reverse */
OBJECT * _reverse_in_place(OBJECT *expr) {
  OBJECT *tmp, *revexpr = NIL;
  while (expr != NIL) {
    tmp = _cdr(expr);
    _cdr(expr) = revexpr;
    revexpr = expr; 
    expr = tmp;
  }
  return revexpr;
}

/*
 * (def assoc (x y)
 *   (cond ((eq (caar y) x) (cadar y))
 *           (true (assoc x (cdr y)))))
 */
OBJECT * _lookup(OBJECT *expr /* x */, OBJECT *env /* y */) {
  for ( ; env != NIL ; env = _cdr(env) ) {
    if (symbol_name(expr) == symbol_name( _car(_car(env)))) {
      return _car(_cdr(_car(env)));
    }
  }
  printf("Warning: not found -- %s\n",symbol_name(expr));
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

OBJECT * make_primitive(prim_op pp) {
  OBJECT *obj = _object_malloc(OPERATOR);
  obj->value.primitive = pp;
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
  for (; obj != NIL; obj = _cdr(obj)) {
    if (strcmp(symbol, symbol_name(_car(obj))) == 0) {
      return _car(obj);
    }
  }
  storage =  GC_MALLOC(len);
  memcpy(storage, symbol, len);
  obj =  _object_malloc(SYMBOL);
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
  if (length > len) len = length;
  obj->value.string = GC_MALLOC(len + 1);
  memcpy(obj->value.string, str, len);
  obj->size = len + 1;
  return obj;
}

/* warning this mutates it's parameter */
OBJECT *_append(OBJECT *exp1, OBJECT *exp2) {
  OBJECT *tmp = exp1;
  for( ; _cdr(tmp) != NIL; tmp = _cdr(tmp) )
       ;
  _cdr(tmp) = exp2;
  return exp1;
}

const char * _strcat_alloc(const char *str1, const char *str2) {
  /* note strlen excludes the NULL trailing byte */
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  char * str3 = NULL;

  str3 = GC_MALLOC(len1 + len2 + 1);

  memcpy((void *)str3, (void *)str1, len1);
  memcpy((void *) (str3+len1), (void *)str2, len2+1);
 
  return str3;
}

OBJECT * string_cat(OBJECT *obj, const char *str) {
  return make_string( _strcat_alloc(string(obj), str), 0);
}
