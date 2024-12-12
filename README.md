# TP1-Synthèse

///Objective

The primary goal of this project was to create a micro shell capable of executing commands and displaying information about their execution. 

///Key Features and Development Process

1. Displaying a Welcome Message and Prompt

A welcome message is displayed on the terminal using file descriptors and the write() function. A simple prompt (enseash %) signals readiness to accept commands. File descriptors are carefully managed, ensuring unused ones are closed properly.

2. Executing User Commands

Reading Input: The read() function is used to capture user input. The newline character from the input buffer is removed to prepare the command for execution.

Executing Commands: A child process is created with fork() to execute user commands. The execlp() function is used for straightforward commands without arguments.

Returning to the Prompt: An infinite loop ensures the prompt is displayed after each command execution.

3. Managing Shell Exit

The shell exit is handled by detecting the exit command using string comparison (strcmp) and recognizing Ctrl+D, which is identified when read() returns a buffer size of 0.

4. Displaying Return Codes

The return code of the last executed command is dynamically displayed in the prompt. Formatted output is managed with sprintf() to include exit codes or signal values.

5. Measuring Execution Time

Execution time is measured using the clock_gettime() function from the time library. The elapsed time (in milliseconds) is displayed in the prompt alongside the return code.

6. Handling Complex Commands (with Arguments)

Input is tokenized using the strtok() function to parse commands into arguments based on spaces. The execvp() function replaces execlp() to allow for multi-argument commands.

7. Supporting Input and Output Redirections

Input redirection (<) and output redirection (>) are implemented using dup2(). Standard input and output are redirected to specified files:

///What I Learned

System Programming: Practical use of system calls like read, write, fork, execvp, and dup2. Understanding the role of file descriptors in managing stdin, stdout, and stderr.

Dynamic Prompt Management: Learned how to display execution results dynamically, including exit codes, signal values, and execution times.

Error Handling: Improved debugging skills for handling invalid commands, missing files, and platform-specific differences.

Version Control: Enhanced proficiency in using Git and GitHub for collaborative development and conflict resolution.

///Challenges Faced

Debugging platform-specific behavior (e.g., differences in commands like hostname -i on macOS vs Linux). Resolving Git conflicts and synchronizing local and remote repositories. Handling divergent Git histories during repository integration.


