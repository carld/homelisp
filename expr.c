/* symbolic expressions 
 * Copyright (C) 2015 A. Carl Douglas
 */

/*
<exp> ::= <var>
        |  (lambda (<var>) <exp>)
        |  (<exp> <exp>)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gc.h>
#include <dlfcn.h>
#include <signal.h>

#define CHECK(test,message)    if (!(test)) { printf("%s:%u -- %s\n", __FILE__, __LINE__, message); abort(); }

enum { PAIR = 1, NUMBER = 2, SYMBOL = 3, OPERATOR = 4 };
enum { T_LPAREN = 1, T_RPAREN = 2, T_QUOTE = 3, T_NUMBER = 4, T_SYMBOL = 5, T_DOT = 6, T_STRING = 7, T_NIL = 8 };

#define issym(x)  ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || (x == '-') || (x == '*') || (x == '_'))
#define issyntax(x) (x == '(' || x == ')' || x == '.' || x == '\'' || x == '`' || x == ',')

typedef struct object OBJECT;

typedef OBJECT * (*prim_op) (OBJECT *);

struct object {
  int type;
  union {
    const char * symbol;
    struct { int integer; int fraction; } number;
    struct { OBJECT * car; OBJECT * cdr; } pair;
    prim_op primitive;
  } value;
};

OBJECT _NIL; 
OBJECT _TRUE; 
OBJECT _FALSE;

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
#define object_type(x)  (x->type)
#define _cdr(x)         (x->value.pair.cdr)
#define _car(x)         (x->value.pair.car)
#define _rplaca(x,y)    (x->value.pair.car = y)
#define _rplacd(x,y)    (x->value.pair.cdr = y)

unsigned _input_size = 1 << 12; /* 4096 */
char   * _input = (char *)0;
char   * _token = (char *)0;

FILE *fip = 0;

void _input_malloc () { _input = GC_MALLOC(_input_size); }

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

OBJECT * _reverse(OBJECT *expr) {
  OBJECT *tmp, *revexpr = NIL;
  while (expr != NIL) {
    tmp = _cdr(expr);
    _cdr(expr) = revexpr;
    revexpr = expr; 
    expr = tmp;
  }
  return revexpr;
}

OBJECT * _bind(OBJECT *expr, OBJECT *val, OBJECT *env) {
  return _cons(_cons(expr, val), env);
}

OBJECT * _lookup(OBJECT *expr, OBJECT *env) {
  for ( ; env != NIL ; env = _cdr(env) ) {
    if (symbol_name(expr) == symbol_name( _car(_car(env)))) {
      return _cdr(_car(env));
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

OBJECT * make_primitive(prim_op pp) {
  OBJECT *obj = _object_malloc(OPERATOR);
  obj->value.primitive = pp;
  return obj;
}

int equal(OBJECT *obj1, OBJECT *obj2) {
  return 1;
}

OBJECT * make_symbol(const char *symbol) {
  OBJECT *obj  = _interned_syms;
  char * storage = 0;
  for (; obj != NIL; obj = _cdr(obj)) {
    if (strcmp(symbol, symbol_name(_car(obj))) == 0) {
      return _car(obj);
    }
  }
  storage =  GC_MALLOC(strlen(symbol) + 1);
  memcpy(storage, symbol, strlen(symbol) + 1);
  obj =  _object_malloc(SYMBOL);
  obj->value.symbol = storage;
  _interned_syms = _cons(obj, _interned_syms);
  return obj;
}

/* primitives */
OBJECT * car(OBJECT *obj)  { return _car(obj); }
OBJECT * cdr(OBJECT *obj)  { return _cdr(obj); }
OBJECT * cons(OBJECT *obj) { return _cons(_car(obj), _cdr(obj)); }


