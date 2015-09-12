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

enum {T_LPAREN=0,T_RPAREN=1,T_SYMBOL=2,T_NUMBER=3,T_STRING=4,T_QUOTE=5,T_COMMENT=6};

/* return zero if successful */
int get_token(FILE *fp, char *token, int *type) {
  enum {S_START=0,S_COMMENT=1,S_SYMBOL=2,S_NUMBER=3,S_STRING=4} state = S_START;
  int ch;
  *type = -1;
top:
  ch = fgetc(fp);
  if (ch == EOF) {
    reached_end_of_file = 1;
    if (exit_on_eof == 1)
    { /* isatty? */
      printf("Exiting.\n"); 
      exit(0);
    }
    return -1;
  }

  if (state==S_START) {
    if (isspace(ch)) {
      /* ignore whitespace */
    } else if (strchr("()'",ch)) {
      *token = ch; token++; *token = '\0';
      if      (ch=='(') *type = T_LPAREN;
      else if (ch==')') *type = T_RPAREN;
      else if (ch=='\'') *type = T_QUOTE;
      return 0;
    }
    else if (strchr(";",ch)) {
      state = S_COMMENT;
    } else if (strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_:-+=*&^%$#@!~'<>/?`|",ch)) {
      *token = ch; token++; *token = '\0';
      state = S_SYMBOL;
    } else if (strchr("0123456789",ch)) {
      *token = ch; token++; *token = '\0';
      state = S_NUMBER;
    } else if (ch == '"') {
      *token = ch; token++; *token = '\0';
      state = S_STRING;
    } else { 
      printf("Error, unexpected '%c' in state %d\n", ch, state);
      exit(1);
    }
  } else if (state==S_COMMENT) {
    if (strchr("\r\n",ch)) {
      state = S_START;
    }
  } else if (state==S_SYMBOL) {
    if (strchr("()' \t\r\n",ch)) {
      ungetc(ch,fp);
      *type = T_SYMBOL;
      return 0;
    }
    *token = ch; token++; *token = '\0';
  } else if (state==S_NUMBER) {
    if (strchr("0123456789",ch)==NULL) {
      ungetc(ch,fp);
      *type = T_NUMBER;
      return 0;
    } 
    *token = ch; token++; *token = '\0';
  } else if (state==S_STRING) {
    *token = ch; token++; *token = '\0';
    if (ch == '"') {
      *type = T_STRING;
      return 0;
    }
  }
  goto top;
}

char * remove_quotes(char *str) {
  int len = strlen(str);
  memmove(str, str+1, len-2);
  str[len-2] = '\0';
  return str;
}

OBJECT * _read(OBJECT *port) {
  OBJECT *token_stack = NIL;
  OBJECT *expr_stack  = NIL;
  OBJECT *expr = NIL;
  OBJECT *obj = NIL;
  int err = 0;
  int indent = 0;
  char token[160];
  int tok_type;

  reached_end_of_file = 0;

  for (;  ; ) {
    err = get_token((FILE *)pointer(port), token, &tok_type);
#if DEBUG_TOKEN
    printf("Token: '%s' type: %d\n", token, tok_type);
#endif
    if (err != 0) break;

    if (tok_type == T_LPAREN) {
      indent++;
      obj = make_symbol(token);
    } else if (tok_type == T_RPAREN) {
      indent--;
      obj = make_symbol(token);
    } else if (tok_type == T_STRING) {
      obj = make_string(remove_quotes(token),0);
    } else if (tok_type == T_NUMBER) {
      obj = make_number(token);
    } else if (tok_type == T_SYMBOL) {
      obj = make_symbol(token);
    } else if (tok_type == T_QUOTE) {
      obj = make_symbol(token);
    } else {
      printf("unrecognised token: '%s'\n", token);
      exit(1);
    }

    token_stack = _cons(obj, token_stack);

    if (tok_type != T_QUOTE && indent == 0)
      break;
  }

  for (    expr = NIL, obj = NIL; 
           token_stack != NIL; 
           token_stack = _cdr(token_stack)) {
    obj = _car(token_stack);
    if (obj->type == SYMBOL) {
      if (strcmp(symbol_name(obj),"(")==0) {
        indent++;
        if (expr_stack == NIL) {
           expr = _cons(expr,NIL);
        } else {
           expr = _cons(expr, _car(expr_stack));
           expr_stack = _cdr(expr_stack);
        }
      } else if (strcmp(symbol_name(obj),")")==0) {
        expr_stack = _cons(expr, expr_stack);
        expr = NIL;
      } else if (strcmp(symbol_name(obj),"'")==0) {
        /* a 'b c   -> a (quote b) c
         * a '(b) c -> a (quote (b)) c
         */
        obj = expr;
        expr = _cons(_cons(make_symbol("quote"), obj), _cdr(obj));
        _rplacd(obj,NIL);
      } else { /* symbol */
        expr = _cons(obj, expr);
      }
    } else if (obj->type == NUMBER || obj->type == STRING) {
      expr = _cons(obj, expr);
    }
  } /* for */
  return expr != NIL ? _car(expr) : NIL;
}

