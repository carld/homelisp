/* read and evaluate symbolic expressions from the file in the provided environment */
#include <stdio.h>
#include <string.h>
#include "lisp.h"
OBJECT * load(OBJECT *filename, OBJECT *environ) {
  OBJECT *port;
  FILE *fp;
  int no_exit = 0; /* do not exit on EOF */
  fp = fopen(symbol_name(filename), "r");
  if (fp == NULL) return FALSE;
  port = make_pointer(fp);

  while(!feof(fp)) {
    OBJECT *exp = _read(port, no_exit);
    _eval(exp, environ);
  }
  fclose(fp);
  return TRUE;
}

void command_line(int argc, char *argv[], OBJECT *env) {
  int arg;
  for(arg = 1; arg < argc; arg++) {
    if (strcmp(argv[arg],"-i")==0) {
      arg++;
      if(argv[arg] != NULL) {
        printf("Loading '%s'...\n", argv[arg]);
        load(make_symbol(argv[arg]), env);
      }
    }
  }
}
