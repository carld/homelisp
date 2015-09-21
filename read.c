/* reads a symbolic expression into an internal representation
 * Copyright (C) 2015 A. Carl Douglas
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp.h"
enum {T_LPAREN=0,T_RPAREN=1,T_SYMBOL=2,T_NUMBER=3,T_STRING=4,T_QUOTE=5,T_COMMENT=6};

/* returns token type on success, -1 on error or EOF */
int get_token(FILE *fp, char *token, int exit_on_eof) {
 /* lexer states and transitions from each state to the
  * next state are also described in the diagram lexer-state.dot */
  enum {S_START=0,S_COMMENT=1,S_SYMBOL=2,S_NUMBER=3,S_STRING=4} state = S_START;
  int ch;
top:
  ch = fgetc(fp);
  if (ch == EOF) {
    if (exit_on_eof == 1) {
      printf("Exiting.\n");
      exit(0);
    }
    return -1;
  }
  if (state==S_START) {
    if (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r') {
      /* ignore leading whitespace */
    } else if (ch=='(') {
      *token++ = ch; *token = '\0';
      return T_LPAREN;
    } else if (ch==')') {
      *token++ = ch; *token = '\0';
      return T_RPAREN;
    } else if (ch=='\'') {
      *token++ = ch; *token = '\0';
      return T_QUOTE;
    } else if (strchr(";",ch)) {
      state = S_COMMENT;
    } else if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_:-+=*&^%$#@!~'<>/?`|",ch)) {
      *token++ = ch; *token = '\0';
      state = S_SYMBOL;
    } else if (strchr("0123456789",ch)) {
      *token++ = ch; *token = '\0';
      state = S_NUMBER;
    } else if (ch == '"') {
      state = S_STRING;
    } else { 
      printf("Error, unexpected '%c' in state %d\n", ch, state);
      exit(1);
    }
  } else if (state==S_COMMENT) {
    if (strchr("\r\n",ch) /* hit a new line */) {
      state = S_START;
    }
  } else if (state==S_SYMBOL) {
    if (strchr("()' \t\r\n",ch) /* hit whitespace or syntax */) {
      ungetc(ch,fp);
      return T_SYMBOL;
    }
    *token++ = ch; *token = '\0';
  } else if (state==S_NUMBER) {
    if (strchr("0123456789",ch)==NULL /* not a number */) {
      ungetc(ch,fp);
      return T_NUMBER;
    }
    *token++ = ch; *token = '\0';
  } else if (state==S_STRING) {
    if (ch == '"') {
      *token = '\0';
      return T_STRING;
    }
    *token++ = ch; *token = '\0';
  }
  goto top;
}

OBJECT * _read(OBJECT *port, int eof_exit) {
  OBJECT *expr_stack  = NIL;
  OBJECT *quote_stack = NIL;
  OBJECT *expr = NIL;
  OBJECT *obj = NIL;
  char token[160];
  int tok_type;
  for (;  ; ) {
    tok_type = get_token((FILE *)pointer(port), token, eof_exit);
    if (tok_type < 0) break; /* EOF */
#if DEBUG_TOKEN
    printf("Token: '%s' type: %d\n", token, tok_type);
#endif

    if (tok_type == T_LPAREN) {
      expr_stack = _cons(expr, expr_stack); /* remember the current list */
      expr = NIL; /* start a new list */
    } else if (tok_type == T_RPAREN) {
      obj = _cons(expr, NIL); /* the new list */
      expr = _car(expr_stack);
      expr_stack = _cdr(expr_stack); /* forget the saved list */
      expr = _append(expr, obj); /* append the new list to the saved list */
    } else if (tok_type == T_STRING) {
      obj = make_string(token,0);
      expr = _append(expr, _cons(obj, NIL));
    } else if (tok_type == T_NUMBER) {
      obj = make_number(token);
      expr = _append(expr, _cons(obj, NIL));
    } else if (tok_type == T_SYMBOL) {
      obj = make_symbol(token);
      expr = _append(expr, _cons(obj, NIL));
    } else if (tok_type == T_QUOTE) {
      obj = _cons(_cons(make_symbol("quote"), NIL), NIL);
      expr = _append(expr, obj);
      /* keep track of quotes */
      quote_stack = _cons(obj, quote_stack);
      continue; /* go to top and read the quoted object */
    } else {
      printf("unrecognised token: '%s'\n", token);
      exit(1);
    }

    if (expr_stack == NIL)
      break;
  }

  /*   expand quotes  */
  /* a 'b c   -> a (quote b) c
   * a '(b) c -> a (quote (b)) c
   */
  for (; quote_stack != NIL; quote_stack = _cdr(quote_stack)) {
    OBJECT *rest;
    obj = _car(quote_stack);
    rest = _cdr(obj);
    _rplacd(_car(obj), rest);
    _cdr(obj) = _cdr(_cdr(obj));
    _cdr(rest) = NIL;
  }
  return expr != NIL ? _car(expr) : NIL;
}

