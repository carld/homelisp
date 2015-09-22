/* read-eval-print-loop
 * copyright (c) 2015 A. Carl Douglas
 */
#include <stdio.h>
#include <dlfcn.h>
#include "lisp.h"

OBJECT * prim_debug(OBJECT *exp, OBJECT *);
OBJECT * prim_read(OBJECT *exp, OBJECT *);
OBJECT * prim_newline(OBJECT *exp, OBJECT *);
OBJECT * string_cat(OBJECT *exp, OBJECT *);
OBJECT * prim_add(OBJECT *exp, OBJECT *);
OBJECT * prim_subtract(OBJECT *exp, OBJECT *);
OBJECT * prim_multiply(OBJECT *exp, OBJECT *);
OBJECT * prim_divide(OBJECT *exp, OBJECT *);
OBJECT * _eval(OBJECT *exp, OBJECT *);

void command_line(int argc, char *argv[], OBJECT *env);

int main(int argc, char *argv[]) {
  OBJECT *environment = NIL;
  OBJECT *port = make_pointer(stdin);
  int exit_on_eof = 1; /* isatty(fileno(stdin)); */

  environment = _bind(make_symbol("print"), make_primitive(_print), environment);
  environment = _bind(make_symbol("read"), make_primitive(prim_read), environment);
  environment = _bind(make_symbol("newline"), make_primitive(prim_newline), environment);
  environment = _bind(make_symbol("stdin"), port, environment);
  environment = _bind(make_symbol("string-append"), make_primitive(string_cat), environment);
  environment = _bind(make_symbol("+"), make_primitive(prim_add), environment);
  environment = _bind(make_symbol("-"), make_primitive(prim_subtract), environment);
  environment = _bind(make_symbol("*"), make_primitive(prim_multiply), environment);
  environment = _bind(make_symbol("/"), make_primitive(prim_divide), environment);
  environment = _bind(make_symbol("_"), make_primitive(_eval), environment);
  environment = _bind(make_symbol("#t"),TRUE, environment);
  environment = _bind(make_symbol("#f"),FALSE, environment);

  command_line(argc, argv, environment);
  while(1) {
    _print(_eval(_read(port, exit_on_eof), environment), environment);
    printf("\n");
  }
  return 0;
}
