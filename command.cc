/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>

#include "command.hh"
#include "shell.hh"

Command::Command()
{
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    _append = false;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear()
{
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands)
    {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if (_outFile)
    {
        if (_outFile == _errFile)
        {
            _errFile = NULL;
        }
        delete _outFile;
    }
    _outFile = NULL;

    if (_inFile)
    {
        if (_inFile == _errFile)
        {
            _errFile = NULL;
        }
        delete _inFile;
    }
    _inFile = NULL;

    if (_errFile)
    {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
    _append = false;
}

void Command::print()
{
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for (auto &simpleCommand : _simpleCommands)
    {
        printf("  %-3d ", i++);
        simpleCommand->print();
    }

    printf("\n\n");
    printf("  Output       Input        Error        Background\n");
    printf("  ------------ ------------ ------------ ------------\n");
    printf("  %-12s %-12s %-12s %-12s\n",
           _outFile ? _outFile->c_str() : "default",
           _inFile ? _inFile->c_str() : "default",
           _errFile ? _errFile->c_str() : "default",
           _background ? "YES" : "NO");
    printf("\n\n");
}

void Command::execute()
{
    // Don't do anything if there are no simple commands
    if (_simpleCommands.size() == 0)
    {
        Shell::prompt();
        return;
    }

    // Print contents of Command data structure
    print();

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec
    //save in/out
    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);
    int fdin;
    //set the initial input int fdin;
    if (_inFile)
    {
        fdin = open(_inFile->c_str(), O_RDONLY, 0440);
    }
    else
    {
        // Use default input
        fdin = dup(tmpin);
    }
    int ret;
    int fdout;
    int fderr;
    //redirect err
    if (_errFile)
    {
        if (_errFile == _outFile)
        {
            if (_append)
            {
                fdout = open(_outFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0660);
            }
            else
            {
                fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0660);
            }
            dup2(fdout, 2);
        }
        else
        {
            fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0660);
            dup2(fderr, 2);
        }
    }
    else
    {
        fderr = dup(tmperr);
    }

    for (int i = 0; static_cast<unsigned int>(i) < _simpleCommands.size(); i++){
        //redirect input
        dup2(fdin, 0);
        close(fdin);

        //setup output
        if (static_cast<unsigned int>(i) == _simpleCommands.size() - 1){
            // Last simple command
            if (_outFile)
            {
                if (_errFile != _outFile)
                {
                    //if append
                    if (_append)
                    {
                        fdout = open(_outFile->c_str(), O_WRONLY | O_APPEND | O_CREAT, 0660);
                    }
                    else
                    {
                        fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0660);
                    }
                }
            }
            else
            {
                // Use default output
                fdout = dup(tmpout);
            }
        }else{
            // Not last simple command
            //create pipe
            int fdpipe[2];
            if (pipe(fdpipe) == -1)
            {
                perror("pipe\n");
                exit(1);
            }

            fdout = fdpipe[1];
            fdin = fdpipe[0];
        }
        // Redirect output
        dup2(fdout, 1);
        close(fdout);
        // Create child process
        ret = fork();
        if (ret == -1)
        {
            perror("fork\n");
            exit(1);
        }
        if (ret == 0)
        {
            close(fdin);
            close(fderr);
            close(tmperr);
            close(tmpin);
            close(tmpout);
            char * argv[_simpleCommands[i]->_arguments.size() + 1 ];
            int k =0;
            for (auto & arg : _simpleCommands[i]->_arguments) {
                char * t = strdup(arg ->c_str());
                argv[k] =  (char*)t;
                k++;
            }
            argv[_simpleCommands[i]->_arguments.size()] = NULL;
            execvp(_simpleCommands[i]->_arguments[0]->c_str(), argv);
            perror("execvp");
            exit(1);
        }
    }
    //restore in/out defaults
    dup2(tmpin, 0);
    dup2(tmpout, 1);
    dup2(tmperr, 2);
    close(tmpin);
    close(tmpout);
    close(tmperr);
    if (!_background)
    {
        // Wait for last command
        waitpid(ret, NULL, 0);
    }

    // Clear to prepare for next command
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand *Command::_currentSimpleCommand;
