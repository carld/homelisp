/* reads a symbol expression into an internal representation
 * what is special about this is that there are no recursive
 * function calls.
 *
 * Copyright (C) 2015 A. Carl Douglas
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int exit_on_eof = 1;
int reached_end_of_file = 0;

/* return zero if successful */
int _get_token(FILE *fp, char *token) {
  int s0 = 0, s1 = 0, ch, comment = 0;
top:
  ch = fgetc(fp);
  if (ch == EOF) {
    /* printf("EOF -- %02d\n", ch);*/
    if (exit_on_eof == 1)
    { 
      printf("Exiting.\n"); 
      exit(0);
    }
    *token = ch; /* EOF */
    reached_end_of_file = 1;
    return -1;
  }
  if (ch == '\n') {
    comment = 0;
  }
  if(ch == ';') {
    comment = 1;
  }
  if (comment==1)
   goto top;

  if (isspace(ch)) goto top;

  if (issyntax(ch)) {
    *token = ch; token++; *token = 0;
    return 0;
  }

  if      (issym(ch))   s0 = 2;
  else if (isdigit(ch)) s0 = 3;
  else                  s0 = 4;
nextch:
  *token = ch; token++;
  ch = fgetc(fp);
  if (ch == EOF) {
    if (exit_on_eof == 1)
    { 
      printf("Exiting.\n"); 
      exit(0);
    }
    *token = ch; /* EOF */
    reached_end_of_file = 1;
    return -1;
  }

  if      (isspace(ch)) s1 = 0;
  else if (issyntax(ch))s1 = 1;
  else if (issym(ch))   s1 = 2;
  else if (isdigit(ch)) s1 = 3;
  else                  s1 = 4;
  if (s0 == s1) goto nextch;
  ungetc(ch, fp);
  *token = '\0';
  return 0;
}

int token_type(const char *tok) {
  switch (*tok) {
  case '(': return T_LPAREN;
  case ')': return T_RPAREN;
  case '.': return T_DOT;
  case 0x27: return T_QUOTE;
  default:
    if (*tok >= '0' && *tok <= '9') return T_NUMBER;
    if (*tok >= 0x21 && *tok <= 0x7e) return T_SYMBOL;
  }
  return -1;
}

OBJECT * _read(OBJECT *port) {
  OBJECT *token_stack = NIL;
  OBJECT *expr_stack  = NIL;
  OBJECT *expr = NIL;
  OBJECT *obj = NIL;
  int err = 0;
  int indent = 0;
  char token[128];

  reached_end_of_file = 0;

  for (;  ; ) {
    err = _get_token((FILE *)pointer(port), token);
#if DEBUG_TOKEN
    printf("Token: '%s'\n", token);
#endif
    if (err != 0) break;

    if (token_type(token) == T_LPAREN) 
      indent++;
    else if (token_type(token) == T_RPAREN) 
      indent--;
    obj = make_symbol(token);
    token_stack = _cons(obj, token_stack);

    if (token_type(token) != T_QUOTE && indent == 0) 
      break;
  }

  for (    expr = NIL, obj = NIL; 
           token_stack != NIL; 
           token_stack = _cdr(token_stack)) {
    obj = _car(token_stack);
    switch(token_type(symbol_name(obj))) {
      case T_DOT: /* dotted pair? */
        printf("dotted pair not implemented\n");  
        break;
      case T_QUOTE:
        /* obj->type = QUOTE;*/
        expr = _cons(obj, expr); break;
      case T_NUMBER: 
        obj = make_number(symbol_name(obj));
        expr = _cons(obj, expr); break;
      case T_SYMBOL:
        expr = _cons(obj, expr); break;
      case T_RPAREN:
        indent--;
        expr_stack = _cons(expr, expr_stack);
        expr = NIL; 
      break;
      case T_LPAREN:
        indent++;
        if (expr_stack == NIL) {
           expr = _cons(expr,NIL);
        } else {
           expr = _cons(expr, _car(expr_stack));
           expr_stack = _cdr(expr_stack);
        }
      break;
    }
  }
  return expr != NIL ? _car(expr) : NIL;
}

