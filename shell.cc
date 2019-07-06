#include <cstdio>
#include <unistd.h>

#include "shell.hh"

int yyparse(void);

void Shell::prompt() {
  printf("myshell>");
  fflush(stdout);
//  if ( isatty(0) ) {
  //  prompt();
 // }
}

int main() {
  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
