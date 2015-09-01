/* symbolic expressions 
 * Copyright (C) 2015 A. Carl Douglas
 */

/*
<exp> ::= <var>
        |  (lambda (<var>) <exp>)
        |  (<exp> <exp>)
*/

void _get_token(FILE *fp, char *token) {
  int s0 = 0, s1 = 0,  ch;
top:
  ch = fgetc(fp);

  if (feof(fp) /* && fp == stdin */) { printf("Exiting.\n"); exit(0); }

  if      (isspace(ch)) goto top;

  if (issyntax(ch)) {
    *token = ch; token++; *token = 0;
    return;
  }

  if      (issym(ch))   s0 = 2;
  else if (isdigit(ch)) s0 = 3;
  else                  s0 = 4;
nextch:
  *token = ch; token++;
  ch = fgetc(fp);
  if      (isspace(ch)) s1 = 0;
  else if (issyntax(ch))s1 = 1;
  else if (issym(ch))   s1 = 2;
  else if (isdigit(ch)) s1 = 3;
  else                  s1 = 4;
  if (s0 == s1) goto nextch;
  ungetc(ch, fp);
  *token = '\0';
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
  return 0;
}


OBJECT * _read(FILE *fp) {
  OBJECT *token_stack = NIL;
  OBJECT *expr_stack  = NIL;
  OBJECT *expr = NIL;
  OBJECT *obj = NIL;
  int indent = 0;
  char token[128];

  for (;  ; ) {
    _get_token(fp, token);
#if DEBUG_TOKEN
    printf("Token: '%s'\n", token);
#endif
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
      case T_QUOTE:
        obj->type = QUOTE;
        expr = _cons(obj, expr);
        break;
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
  return _car(expr);
}

