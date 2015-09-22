/* reads a symbolic expression into memory
 * Copyright (C) 2015 A. Carl Douglas
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"
enum {T_NONE=-1,T_LPAREN=0,T_RPAREN=1,T_SYMBOL=2,T_NUMBER=3,T_STRING=4,T_QUOTE=5,T_COMMENT=6};

/* returns token type on success, -1 on error or EOF
 * lexer states and transitions are in the diagram lexer-state.dot */
int get_token(FILE *fp, char *token, int exit_on_eof) {
  int state = T_NONE;
  int ch;
top:
  ch = fgetc(fp);
  if (ch == EOF) {
    if (exit_on_eof == 1) {
      printf("Exiting.\n");
      exit(0);
    }
    return T_NONE;
  }
  switch(state) {
  case T_NONE:
    if (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r') { /* ignore leading whitespace */
    } else if (ch=='(') {
      *token++ = ch; *token = '\0'; return T_LPAREN;
    } else if (ch==')') {
      *token++ = ch; *token = '\0'; return T_RPAREN;
    } else if (ch=='\'') {
      *token++ = ch; *token = '\0'; return T_QUOTE;
    } else if (ch==';') {
      state = T_COMMENT;
    } else if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_:-+=*&^%$#@!~'<>/?`|",ch)) {
      *token++ = ch; *token = '\0';
      state = T_SYMBOL;
    } else if (strchr("0123456789",ch)) {
      *token++ = ch; *token = '\0';
      state = T_NUMBER;
    } else if (ch == '"') {
      state = T_STRING;
    } else { 
      printf("Error, unexpected '%c' in state %d\n", ch, state);
      exit(1);
    }
  break;
  case T_COMMENT:
    if (strchr("\r\n",ch) /* hit a new line */) {
      state = T_NONE;
    }
  break;
  case T_SYMBOL:
    if (strchr("()' \t\r\n",ch) /* hit whitespace or syntax */) {
      ungetc(ch,fp);
      return T_SYMBOL;
    }
    *token++ = ch; *token = '\0';
  break;
  case T_NUMBER:
    if (strchr("0123456789",ch)==NULL /* not a number */) {
      ungetc(ch,fp);
      return T_NUMBER;
    }
    *token++ = ch; *token = '\0';
  break;
  case T_STRING:
    if (ch == '"') {
      *token = '\0';
      return T_STRING;
    }
    *token++ = ch; *token = '\0';
  break;
  }
  goto top;
}

OBJECT * _read(OBJECT *port, int eof_exit) {
  OBJECT *expr_stack  = NIL;
  OBJECT *quote_stack = NIL;
  OBJECT *expr = NIL;
  OBJECT *obj = NIL;
  char token[80];
  int tok_type;
  for (;  ; ) {
    tok_type = get_token((FILE *)pointer(port), token, eof_exit);
    if (tok_type < 0) break; /* EOF */
    if (tok_type == T_LPAREN) {
      expr_stack = _cons(expr, expr_stack); /* remember the current list */
      expr = NIL; /* start a new list */
      continue; /* go to top and read the first object in the expression */
    } else if (tok_type == T_RPAREN) {
      obj = _cons(expr, NIL); /* the new list */
      expr = _car(expr_stack); /* pop the outer expression off the stack */
      expr_stack = _cdr(expr_stack);
    } else if (tok_type == T_STRING) {
      obj = _cons(make_string(token, 0), NIL);
    } else if (tok_type == T_NUMBER) {
      obj = _cons(make_number(token), NIL);
    } else if (tok_type == T_SYMBOL) {
      obj = _cons(make_symbol(token), NIL);
    } else if (tok_type == T_QUOTE) {
      obj = _cons(_cons(make_symbol("quote"), NIL), NIL); /* insert quote procedure call */
      quote_stack = _cons(obj, quote_stack); /* keep track of quotes */
      expr = _append(expr, obj); /* append (quote) to the current expression */
      continue; /* go to the top and read the quoted object */
    } else {
      printf("unrecognised token: '%s'\n", token); exit(1);
    }
    expr = _append(expr, obj); /* append the object to the current expression */
    if (expr_stack == NIL) /* an expression as been read */
      break;
  }
  /*  finish expanding quotes: a 'b c  ->  a (quote b) c  ...  a '(b) c  ->  a (quote (b)) c   */
  for (; quote_stack != NIL; quote_stack = _cdr(quote_stack)) {
    _rplacd(_car(_car(quote_stack)), _cdr(_car(quote_stack))); /* a (quote) b c  ->  a (quote b) c */
    _rplacd(_car(quote_stack), _cdr(_cdr(_car(quote_stack))));
    _rplacd(_cdr(_car(quote_stack)), NIL);
  }
  return expr != NIL ? _car(expr) : NIL;
}
