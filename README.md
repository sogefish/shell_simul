# shell_simul

Written in C, implements a virtual shell program that repeatedly reads commands from the user, interprets them, and translates them into a sequence of actions that likely involve an interaction with the operating system through system calls.

### Features

1. Prompt Symbol “$”:
  - The “$” symbol is placed as a prompt symbol, followed by a space when it is ready to
receive a command from the user.
  - Print out path of the current working directory in addition to the prompt
symbol, simulating actual shell environments.

2. Waiting for subcommands when ran in the foreground:
  - The implementation utilizes wait() and close() functions to ensure that if a program is run
in the foreground, it waits until the command is run and finished prior to prompting the
user for another command.
  - Commands launched in the background:
  - When a subcommand is launched in the background, signified by the suffix “&”, the shell
prints the pid of such process within square brackets, and then moving on to display the
prompt to take in new commands regardless of whether the initial command is still
running.

3. Subcommand not found:
  - When a command that is not recognized or found, an error message is displayed via
perror() command, and then is immediately followed by the next prompt.

4. Input Redirection Operator “<”:
  - When the “<” operator is included within the command, it redirects the input accordingly,
and if not found, it displays an error message.

5. Output Redirection Operator “>”:
  - When the “>” operator is included within the command, it redirects the output
accordingly, and creates a file if the file does not exist.

6. Subcommands separated by pipes:
  - The shell properly forks child processes when multiple pipes are required, opening and
closing file descriptors to avoid blocking reads on pipes.

7. Appending Output Redirection:
  - When a symbol “>>” is part of a subcommand, the shell will append the subcommand to
the destination file rather than writing over it.

### Execution Sample:
- The commands included within the screenshots provide a thorough testing of execution that can
be reproduced within the cslab environment. Within the zip file, along with the c file as well as
the Makefile file, a test.txt file was included to conveniently follow along with the execution
samples. Feel free to add your own as well!

### To Run:
- To run the project file, simply type 'make' into your terminal, which then will provide you with an executable file called 'shell' to run
