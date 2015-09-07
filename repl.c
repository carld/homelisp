/* the read eval print loop
 *
 * copyright (c) 2015 A. Carl Douglas
 */

#include "expr.c"
#include "debug.c"
#include "read.c"
#include "eval_r.c"
#include "prims.c"
#include "print.c"
#include "load.c"

int main(int argc, char *argv[]) {

  OBJECT *environment = NIL;

  OBJECT *port = make_pointer(stdin);

  environment =  _bind(make_symbol("print"), make_primitive(_print), environment);
  environment =  _bind(make_symbol("+"), make_primitive(prim_add), environment);
  environment =  _bind(make_symbol("-"), make_primitive(prim_subtract), environment);
  environment =  _bind(make_symbol("*"), make_primitive(prim_multiply), environment);
  environment =  _bind(make_symbol("/"), make_primitive(prim_divide), environment);

  command_line(argc, argv, environment);

  exit_on_eof = 1;
  while(1) {
    _print(_eval(_read(port), environment));
  }

  return 0;
}

