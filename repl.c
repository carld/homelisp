/* the read eval print loop
 *
 * copyright (c) 2015 A. Carl Douglas
 */
#include <unistd.h>

#include "expr.c"
#include "debug.c"
#include "read.c"
#include "eval_r.c"
#include "prims.c"
#include "print.c"

int main(int argc, char *argv[]) {

  OBJECT *environment = NIL;

  OBJECT *port = make_number_i(STDIN_FILENO);

  environment =  _bind(make_symbol("car"),     make_primitive(prim_car), environment);
  environment =  _bind(make_symbol("cdr"),     make_primitive(prim_cdr), environment);
  environment =  _bind(make_symbol("cons"),    make_primitive(prim_cons), environment);
  environment =  _bind(make_symbol("reverse"), make_primitive(prim_reverse), environment);

  environment =  _bind(make_symbol("+"), make_primitive(prim_add), environment);
  environment =  _bind(make_symbol("-"), make_primitive(prim_subtract), environment);
  environment =  _bind(make_symbol("*"), make_primitive(prim_multiply), environment);
  environment =  _bind(make_symbol("/"), make_primitive(prim_divide), environment);

  while(!feof(stdin)) {

    _print(_eval(_read(port), environment));

  }

  return 0;
}
