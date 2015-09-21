/* structures and functions for representing symbolic expressions .
 * an object structure can store either an atomic type:
 *   a symbol or a number, or a Cons cell.
 *
 * copyright (C) 2015 A. Carl Douglas 
 */
#include <stdlib.h>
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

extern OBJECT _NIL;
extern OBJECT _TRUE;
extern OBJECT _FALSE;

#define NIL     ((OBJECT *) &_NIL) 
#define TRUE    ((OBJECT *) &_TRUE)
#define FALSE   ((OBJECT *) &_FALSE)

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

OBJECT * _object_malloc (int type);
OBJECT * _cons(OBJECT *car, OBJECT *cdr);

/* in place reverse */
OBJECT * _reverse_in_place(OBJECT *expr);

/*
 * (def assoc (x y)
 *   (cond ((eq (caar y) x) (cadar y))
 *           (true (assoc x (cdr y)))))
 */
OBJECT * _lookup(OBJECT *expr /* x */, OBJECT *env /* y */);

OBJECT * make_number_i(int integer);

OBJECT * make_number(const char *token);

OBJECT * make_primitive(prim_op pp);

/* this helps passing around FILE * etc */
OBJECT * make_pointer(void * ptr);

OBJECT * make_symbol(const char *symbol);

/* if the length parameter is less than the length of 
 * the string the actual string length will be used
 */
OBJECT * make_string(const char *str, size_t length);

/* warning this mutates the first parameter */
OBJECT *_append(OBJECT *exp1, OBJECT *exp2);

const char * _strcat_alloc(const char *str1, const char *str2);

OBJECT * string_cat(OBJECT *obj1, OBJECT *obj2);


OBJECT * _evlis(OBJECT *expr, OBJECT *environ);

OBJECT * _eval(OBJECT *expr, OBJECT *environ);

OBJECT * _read(OBJECT *port, int eof_exit);

OBJECT * _print(OBJECT *exp);

OBJECT * debug(OBJECT *exp);

#define debugf(x,m) printf("%s:%d --- %s\n", __FILE__, __LINE__, m); debug(x)


