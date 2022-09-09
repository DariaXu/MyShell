#include <cstdio>
#include <unistd.h>
#include <signal.h>

#include "shell.hh"

int yyparse(void);

void Shell::prompt()
{

  if (isatty(0))
  {
    printf("myshell>");
    fflush(stdout);
  }
}
extern "C" void sigIntHandler(int sig)
{
  printf("\n");
  Shell::prompt();
}

extern "C" void sigChildHandler(int sig)
{
  while (int pid = waitpid(-1, NULL, WNOHANG))
  {
  }
}

int main()
{
  //ctrl-c
  struct sigaction signalAction;
  signalAction.sa_handler = sigIntHandler;
  sigemptyset(&signalAction.sa_mask);
  signalAction.sa_flags = SA_RESTART;
  int error = sigaction(SIGINT, &signalAction, NULL);
  if (error)
  {
    perror("crtl_c");
    exit(-1);
  }

  //zombie
  struct sigaction signalActionz;
  signalActionz.sa_handler = sigChildHandler;
  sigemptyset(&signalActionz.sa_mask);
  signalActionz.sa_flags = SA_RESTART;
  int error = sigaction(SIGCHLD, &signalActionz, NULL);
  if (error)
  {
    perror("zombie");
    exit(-1);
  }

  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
