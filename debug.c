
#define ERR(x) printf("%s:%d \n", __FILE__, __LINE__); debug(x);

char * obj_inspector(OBJECT *obj) {
  char *str = GC_MALLOC(256);
  switch (object_type(obj)){
  case PAIR:   snprintf(str,255,"[%p, CONS %p %p %s] NIL=%p",
               (void *)obj,(void *)_car(obj),(void *)_cdr(obj), 
               _cdr(obj) == NIL ? "(NIL)" : "",
               (void *)NIL); break;
  case SYMBOL: snprintf(str,255,"[%p, SYMBOL %s]",
               (void *)obj,obj->value.symbol); break;
  case NUMBER: snprintf(str,255,"[%p, NUMBER %d.%d]",
               (void *)obj,obj->value.number.integer,obj->value.number.fraction); break;
  case OPERATOR: snprintf(str,255,"[%p, OPERATOR ]", 
               (void*)obj); break;
  case QUOTE:   
    snprintf(str,255,"[%p, QUOTE]", (void *)obj); break;
  default: abort();
  }
  return str;
}

void indent_print_obj(OBJECT *obj, int indent) {
  int i=0;
  for(i=0;i<indent;i++) printf("  ");
  printf("%s\n", obj_inspector(obj));
}

void debug(OBJECT *exp) {
  OBJECT *expr_stack  = NIL;
  int indent = 0;
next:
  indent_print_obj(exp, indent);
  if (object_type(exp) == PAIR) {
    expr_stack = _cons(exp, expr_stack);
    exp = _car(exp);
    indent++;
  } else if (object_type(exp) == SYMBOL || object_type(exp) == NUMBER || object_type(exp) == OPERATOR || object_type(exp) == QUOTE) {
    if (expr_stack != NIL) {
      exp = _cdr(_car(expr_stack));
      expr_stack = _cdr(expr_stack);
      indent--;
    } else {
      exp = NIL;
    }
  }
  if (exp != NIL) goto next;
  printf("\n");
}

#define debugf(x) printf("%s:%d ---\n", __FILE__, __LINE__); debug(x)
