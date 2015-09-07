/* built-in functions that can be applied
 * during evaluation of a symbolic expression
 *
 * copyright (c) 2015 A. Carl Douglas
 */

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

