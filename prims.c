
#define DEFMATH(op,name) \
OBJECT * name (OBJECT *args) { \
  int result = integer(_car(args));\
  for(args = _cdr(args) ;args != NIL; args = _cdr(args)) { \
    result = result op integer(_car(args)); \
  } \
  return make_number_i(result); \
}

DEFMATH(+,prim_add)
DEFMATH(-,prim_subtract)
DEFMATH(*,prim_multiply)
DEFMATH(/,prim_divide)

OBJECT * prim_car(OBJECT *obj)  { return _car(obj); }
OBJECT * prim_cdr(OBJECT *obj)  { return _cdr(obj); }
OBJECT * prim_cons(OBJECT *obj) { return _cons(_car(obj), _car(_cdr(obj))); }

OBJECT * prim_reverse(OBJECT *exp) {
  OBJECT *revexp = NIL;
  for ( ; exp != NIL; exp = _cdr(exp)) {
    revexp = _cons(_car(exp), revexp);
  }
  return revexp;
}
