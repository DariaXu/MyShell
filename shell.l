
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include "y.tab.hh"

static void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%option noyywrap

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

">" {
  return GREAT;
}

">>" {
  return GREATGREAT;
}

">>&" {
  return GREATGREATAMPERSAND;
}

">&" {
  return GREATAMPERSAND;
}

"<" {
  return LESS;
}

"2>" {
  return TWOGREAT;
}

"&" {
  return AMPERSAND;
}

"|" {
  return PIPE;
}

//subshell backtip apeend newline then exit(2 commands)
/*
["][^\"\n]*["] {
  /* 2.4Quotes*/
  char* temp = strdup(yytext+1);
  temp[strlen(temp)-1] = '\0';
  yylval.cpp_string = new std::string(temp);
  return WORD;
}


"source "[^ \t\n][^ \t\n]* {
  /* 2.6source*/
  char * temp = strdup(yytext + 7);
  temp[strlen(temp)] = '\0';
  yyin = fopen( temp, "r" );
  YY_BUFFER_STATE state = yy_create_buffer( yyin, YY_BUF_SIZE );
  yypush_buffer_state(state); 
  yyparse();
  yy_flush_buffer(state);
  yypop_buffer_state();
}

[^ \t\n]*[\\][^ \t\n]*  {
    /*escape*/
    char * temp = strdup(yytext);
    std::vector<char > done;
    while(*temp){
      char * next = temp+1;
      if(*temp == '\\' ){
        if( *next== '\\'){
          while(*(next) == '\\' ){
            next++;
            temp++;
          }
        }else{
          temp++;
        }
      }
      done.push_back(*temp);  
      temp++;
    }
    
    yylval.cpp_string = new std::string(done.data());
  
    return WORD;
}

[$][(][^\n]*[)]  {

    char * temp = strdup(yytext+2);
    temp[strlen(temp)-1] = '\0';

    int pin[2], pout[2];
    pipe(pin); 
    pipe(pout);

    write(pin[1],temp,strlen(temp));
    write(pin[1],"\nexit\n",6);
    close(pin[1]);
    int ret = fork();
        if (ret == -1)
        {
            perror("subshell-fork");
            exit(1);
        }
        
        if (ret == 0){
          dup2(pin[0],0);
          close(pin[0]);    
          dup2(pout[1],1);
          close(pout[1]);

          execvp("/proc/self/exe", NULL);
          perror("evecvp");
          _exit(1);
        }

    close(pin[0]); 
    close(pout[1]); 
    
    std::vector<char> buffer ;
    char c;
    while(read(pout[0],&c,1)){
      if(c == '\n'){
        buffer.push_back(' ');
      }else{
        buffer.push_back(c);
      }
    }
    close(pout[0]);
    for (auto it = buffer.end(); it != buffer.begin(); it--) {
      if(*it){
        myunputc(*it);
      }
      
    }
    myunputc(*(buffer.begin()));
   
}
*/

[^ \t\n][^ \t\n]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  //2.4 2.5 before this

  return WORD;
}


