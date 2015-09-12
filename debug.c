
/*
 * These functions print out internals of a symbolic expression.
 *
 * The debug function has been highly useful in development,
 * but is not required as part of the read-eval-print loop.
 *
 * copyright (C) 2015 A. Carl Douglas
 */
#include<stdio.h>
#define ERR(x) printf("%s:%d \n", __FILE__, __LINE__); debug(x);

char * obj_inspector(OBJECT *obj) {
  char *str = GC_MALLOC(256);
  if (obj == NIL) {
    snprintf(str,255,"[%p NIL]", (void *)obj);
    return str;
  }
  switch (object_type(obj)){
  case PAIR:   snprintf(str,255,"[%p, CONS %p %p %s] NIL=%p",
               (void *)obj,(void *)_car(obj),(void *)_cdr(obj), 
               _cdr(obj) == NIL ? "(NIL)" : "",
               (void *)NIL); break;
  case SYMBOL: snprintf(str,255,"[%p, SYMBOL %s]",
               (void *)obj,obj->value.symbol); break;
  case STRING: snprintf(str,255,"[%p, STRING %s]",
               (void *)obj,obj->value.string); break;
  case NUMBER: snprintf(str,255,"[%p, NUMBER %d.%d]",
               (void *)obj,obj->value.number.integer,obj->value.number.fraction); break;
  case OPERATOR: snprintf(str,255,"[%p, OPERATOR ]", 
               (void*)obj); break;
  default: abort();
  }
  return str;
}

void indent_print_obj(OBJECT *obj, int indent) {
  int i=0;
  for(i=0;i<indent;i++) printf("  ");
  printf("%s\n", obj_inspector(obj));
}

OBJECT * debug(OBJECT *exp) {
  OBJECT *expr_stack  = NIL;
  int indent = 0;
next:
  indent_print_obj(exp, indent);
  if (exp == NIL) 
    goto pop_frame;

  if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack);
    exp = _car(exp);
    indent++;
    goto next;
  } else if (object_type(exp) == SYMBOL 
          || object_type(exp) == NUMBER 
          || object_type(exp) == OPERATOR
          || object_type(exp) == STRING) {

pop_frame:
    if (expr_stack != NIL) {
      exp = _cdr(_car(expr_stack));
      expr_stack = _cdr(expr_stack);
      indent--;
      if(exp == NIL)
        goto pop_frame;
    } else {
      exp = NIL;
    }
  }
  if (exp != NIL) goto next;

  printf("\n");
  return NIL;
}

#define debugf(x,m) printf("%s:%d --- %s\n", __FILE__, __LINE__, m); debug(x)

