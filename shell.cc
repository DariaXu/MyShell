#include <cstdio>
#include <unistd.h>

#include "shell.hh"

int yyparse(void);

void Shell::prompt() {

  if ( isatty(0) ) {
    prompt();
  }

  //printf("myshell>");
  //fflush(stdout); 
}

int main() {
  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
