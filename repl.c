/* the read eval print loop
 *
 * copyright (c) 2015 A. Carl Douglas
 */

#include "expr.c"
#include "debug.c"
#include "read.c"
#include "print.c"
#include "eval_r.c"
#include "prims.c"
#include "load.c"

int main(int argc, char *argv[]) {

  OBJECT *environment = NIL;

  OBJECT *port = make_pointer(stdin);

  environment =  _bind(make_symbol("print"), make_primitive(prim_print), environment);
  environment =  _bind(make_symbol("newline"), make_primitive(prim_newline), environment);
  environment =  _bind(make_symbol("debug"), make_primitive(prim_debug), environment);
  environment =  _bind(make_symbol("+"), make_primitive(prim_add), environment);
  environment =  _bind(make_symbol("-"), make_primitive(prim_subtract), environment);
  environment =  _bind(make_symbol("*"), make_primitive(prim_multiply), environment);
  environment =  _bind(make_symbol("/"), make_primitive(prim_divide), environment);

  command_line(argc, argv, environment);

  exit_on_eof = 1;
  while(1) {
    _print(_eval(_read(port), environment));
    printf("\n");
  }

  return 0;
}

