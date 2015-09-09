/* read and evaluate symbolic expressions from the file 
 * in the provided environment
 */
#include <sys/stat.h>
#include <fcntl.h>
OBJECT * load(OBJECT *filename, OBJECT *environ) {
  OBJECT *port;
  FILE *fp;

  fp = fopen(symbol_name(filename), "r");
  if (fp == NULL) {
    return FALSE;
  }
  port = make_pointer(fp);

  exit_on_eof = 0;
  while(reached_end_of_file == 0) {
    OBJECT *exp = _read(port);
    _eval(exp, environ);
  }
  fclose(fp);
  reached_end_of_file = 0;
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
