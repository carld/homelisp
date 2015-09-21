/* read-eval-print-loop
 * copyright (c) 2015 A. Carl Douglas
 */
#include<stdio.h>
#include"lisp.h"

OBJECT * prim_print(OBJECT *exp);
OBJECT * prim_debug(OBJECT *exp);
OBJECT * prim_read(OBJECT *exp);
OBJECT * prim_newline(OBJECT *exp);
OBJECT * prim_string_append(OBJECT *exp);
OBJECT * prim_add(OBJECT *exp);
OBJECT * prim_subtract(OBJECT *exp);
OBJECT * prim_multiply(OBJECT *exp);
OBJECT * prim_divide(OBJECT *exp);
OBJECT * prim_eval(OBJECT *exp);

void command_line(int argc, char *argv[], OBJECT *env);

int main(int argc, char *argv[]) {
  OBJECT *environment = NIL;
  OBJECT *port = make_pointer(stdin);
  int exit_on_eof = 1; /* isatty(fileno(stdin)); */

  environment = _bind(make_symbol("print"), make_primitive(prim_print), environment);
  environment = _bind(make_symbol("read"), make_primitive(prim_read), environment);
  environment = _bind(make_symbol("newline"), make_primitive(prim_newline), environment);
  environment = _bind(make_symbol("debug"), make_primitive(prim_debug), environment);
  environment = _bind(make_symbol("stdin"), port, environment);
  environment = _bind(make_symbol("string-append"), make_primitive(prim_string_append), environment);
  environment = _bind(make_symbol("+"), make_primitive(prim_add), environment);
  environment = _bind(make_symbol("-"), make_primitive(prim_subtract), environment);
  environment = _bind(make_symbol("*"), make_primitive(prim_multiply), environment);
  environment = _bind(make_symbol("/"), make_primitive(prim_divide), environment);
  environment = _bind(make_symbol("_"), make_primitive(prim_eval), environment);

  command_line(argc, argv, environment);
  while(1) {
    _print(_eval(_read(port, exit_on_eof), environment));
    printf("\n");
  }
  return 0;
}
