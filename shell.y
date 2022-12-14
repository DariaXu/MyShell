
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires 
{
#include <string>


#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT GREATGREAT GREATGREATAMPERSAND GREATAMPERSAND LESS TWOGREAT AMPERSAND PIPE NEWLINE

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"

void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
  pipe_list io_modifier_list background_opt NEWLINE {
   // printf("   Yacc: Execute command\n");
    Shell::_currentCommand.execute();

  }
  | NEWLINE 
  | error NEWLINE { yyerrok; }
  ;

pipe_list:
  pipe_list PIPE command_and_args | command_and_args
  ;

command_and_args:
  command_word argument_list {
    Shell::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
   // printf("   Yacc: insert argument \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand->insertArgument( $1 );\
  }
  ;

command_word:
  WORD {
   // printf("   Yacc: insert command \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;

iomodifier_opt:
  GREAT WORD {
   // printf("   Yacc: insert output \"%s\"\n", $2->c_str());
    if(Shell::_currentCommand._outFile != NULL){
	    printf("Ambiguous output redirect.\n");
    }else{
    	Shell::_currentCommand._outFile = $2;
    }
  }
  |GREATGREAT WORD
  {
   // printf("   Yacc: insert output \"%s\"\n", $2->c_str());
   if(Shell::_currentCommand._outFile != NULL){
	    printf("Ambiguous output redirect.\n");
    }else{
      Shell::_currentCommand._outFile = $2;
      Shell::_currentCommand._append = true;
    }
  }
  | GREATGREATAMPERSAND WORD {
   // printf("   Yacc: insert output and error \"%s\"\n", $2->c_str());
   if(Shell::_currentCommand._outFile != NULL){
	    printf("Ambiguous output redirect.\n");
    }else if (Shell::_currentCommand._errFile != NULL){
       printf("Ambiguous error redirect.\n");  
    }else{
      Shell::_currentCommand._outFile = $2;
      Shell::_currentCommand._errFile = $2;
      Shell::_currentCommand._append = true;
    }
  }
  | GREATAMPERSAND WORD{
  //  printf("   Yacc: insert output and error \"%s\"\n", $2->c_str());
  if(Shell::_currentCommand._outFile != NULL){
	    printf("Ambiguous output redirect.\n");
    }else if (Shell::_currentCommand._errFile != NULL){
       printf("Ambiguous error redirect.\n");  
    }else{
      Shell::_currentCommand._outFile = $2;
      Shell::_currentCommand._errFile = $2;
    }
  }
  | LESS WORD{
  //  printf("   Yacc: insert input \"%s\"\n", $2->c_str());
  if(Shell::_currentCommand._inFile != NULL){
	    printf("Ambiguous input redirect.\n");
    }else{
    Shell::_currentCommand._inFile = $2;
    }
  }
  | TWOGREAT WORD{
   // printf("   Yacc: insert error \"%s\"\n", $2->c_str());
   if (Shell::_currentCommand._errFile != NULL){
       printf("Ambiguous error redirect.\n");  
    }else{
      Shell::_currentCommand._errFile = $2;
    }
  }
  ;

io_modifier_list:
  io_modifier_list iomodifier_opt
  | iomodifier_opt
  |/*empty*/
    ;

background_opt:
  AMPERSAND{
  //  printf("   Yacc: insert background ");
    Shell::_currentCommand._background = true;
  }
  | {
    Shell::_currentCommand._background = false;
  }/*empty*/
  ;


%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
