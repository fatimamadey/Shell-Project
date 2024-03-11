# Simple Shell

## Overview
The Simple Shell project is a command line interpreter implemented in C, providing basic shell functionalities similar to Unix shells. It was developed as part of an assignment focusing on system programming concepts, process management, and defensive programming.

## Objectives
1. **Complete Program**: The shell was built from scratch.
2. **Shell Functionalities**: It incorporates essential Unix shell features, including process creation, execution, and interaction.
3. **Process Interaction**: Understanding of parent-child process relationships, waiting for child processes, and communication via pipes.
4. **Defensive Programming**: Robust error handling and sanity checks were implemented to ensure stable operation.

## Features
### Basic Shell
- Interactive loop prompting the user with "myshell> ".
- Parses input, executes commands, and waits for completion until the user types "exit".

### Command Line Parsing
- Parses commands and executes them using `execvp` unless it is a built in command.
- Supports arguments separated by whitespace or tabs.

### Multiple Commands
- Supports running multiple commands on a single line separated by ";" characters.
- Executes commands sequentially from left to right.

### Built-in Commands
- Implements built-in commands `exit`, `cd`, and `pwd`.
- Handles these commands internally without forking a child process.

### Redirection
- Supports basic redirection using ">" character to redirect standard output to a file.

## Usage
1. Clone the repository and navigate to the project directory.
2. Compile the code using the provided makefile.
3. Run the shell executable `myshell`.
4. Enter commands at the prompt and observe the output.

## Testing
- Extensive testing was conducted to verify correct behavior under various scenarios.
- Automated tests were utilized to ensure robustness and correctness.
- Edge cases and error scenarios were thoroughly examined to guarantee stability.

