#include <unistd.h>  // For write, fork, execlp
#include <stdlib.h>   
#include <string.h>   
#include <sys/wait.h> // For wait
#include <stdio.h>    

#define MAX_INPUT_SIZE 256

// Input buffer
char input[MAX_INPUT_SIZE];
int status;

// Shell display function
void shellDisplay(void) {
    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    write(STDOUT_FILENO, welcomeMessage, strlen(welcomeMessage));
}

// Command execution function
void executeCommand(const char *input) {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: execute the command
        execlp(input, input, NULL);

        // If exec fails, print an error message and exit child process
        const char errorMessage[] = "Command not found\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child to complete
        wait(&status);
    }
}

int main(int argc, char **argv) {
    const char waitingPrompt[] = "enseash-Q2 % ";

    // Display shell welcome message
    shellDisplay();

    while (1) {
        // Display the prompt
        write(STDOUT_FILENO, waitingPrompt, strlen(waitingPrompt));

        // Read user input
        ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input) - 1);

        // Check if user pressed Ctrl+D or there was an error
        if (bytesRead <= 0) {
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        // Null-terminate the input and remove newline
        input[bytesRead - 1] = '\0';

        // Exit the shell if the user types "exit"
        if (strncmp(input, "exit", 4) == 0 && (bytesRead == 5 || input[4] == '\0')) {
            break;
        }

        // Execute the command
        executeCommand(input);
    }

    return EXIT_SUCCESS;
}