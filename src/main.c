#include <builtins.h>
#include <dlfcn.h>
#include <nativecomp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stem.h>
#include <string.h>
#include <unistd.h>

extern ht_t *WORD_TABLE;
extern array_t *STACK;
extern parser_t *PARSER;
extern array_t *EVAL_STACK;
extern ht_t *OBJ_TABLE;
extern ht_t *FLIT;
extern ht_t *RUNTIME_TABLE;
extern string_t *CODE;

/*! prints usage then exits */
void usage() {
  printf("Usage: stem [-hv] [file]\n");
  exit(1);
}

/*! prints version and exits */
void version() {
  printf("Author: Preston Pan, MIT License 2024\n");
  printf("stem, version 1.3 alpha\n");
  exit(0);
}

/*! frees all global variables */
void global_free() {
  free(PARSER->source);
  ht_free(WORD_TABLE, value_free);
  ht_free(FLIT, func_free);
  ht_free(OBJ_TABLE, custom_free);
  array_free(STACK);
  free(PARSER);
  array_free(EVAL_STACK);
  ht_free(RUNTIME_TABLE, string_free);
  string_free(CODE);
}

/*! handles SIGINT signal; frees memory before exit */
void sigint_handler(int signum) {
  signal(SIGINT, sigint_handler);
  global_free();
  fflush(stdout);
  exit(1);
}

int main(int argc, char **argv) {
  value_t *v;
  size_t len = 0;
  char *buf = "";

  /* Parsing arguments */
  if (argc < 2) {
    usage();
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    usage();
  } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    version();
  }

  /* Read code from file */
  FILE *FP = fopen(argv[1], "rb");

  if (!FP) {
    usage();
  }

  ssize_t bytes_read = getdelim(&buf, &len, '\0', FP);
  fclose(FP);

  /* Set up global variables */
  PARSER = parser_pp(buf);
  STACK = init_array(10);
  WORD_TABLE = init_ht(500);
  EVAL_STACK = init_array(10);
  FLIT = init_ht(500);
  OBJ_TABLE = init_ht(500);
  RUNTIME_TABLE = init_ht(500);
  CODE = init_string("");

  add_funcs();
  add_runtime_funcs();
  signal(SIGINT, sigint_handler);

  /* parse and eval loop */
  add_preamble();
  while (1) {
    v = parser_get_next(PARSER);
    if (v == NULL)
      break;
    if (PARSER->comp)
      eval(v);
    else
      gen(v);
  }

  add_postamble();
  printf("%s\n", CODE->value);

  /* Free all global variables */
  global_free();
  return 0;
}
