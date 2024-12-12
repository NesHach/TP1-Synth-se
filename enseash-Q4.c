#include <unistd.h>  // For write, fork, execlp
#include <stdlib.h>  // For exit, EXIT_SUCCESS
#include <stdio.h>   // For snprintf
#include <string.h>  // For strlen, strncmp
#include <sys/wait.h> // For wait

#define MAX_INPUT_SIZE 256

// Input buffer
char input[MAX_INPUT_SIZE];
int status; // To capture the child process exit status
char waitingPrompt[MAX_INPUT_SIZE] = ""; // Dynamic prompt buffer
char exitSuccess[] = "End of ShellENSEA\nBye bye...\n";

void shellDisplay(void) {
    // Informational Messages
    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    write(STDOUT_FILENO, welcomeMessage, strlen(welcomeMessage));
}

void return_code(void) {
    // Return code for exit and signal
    if (WIFEXITED(status)) {
        int exitCode = WEXITSTATUS(status);
        snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash [exit:%d] %% ", exitCode);
    } else if (WIFSIGNALED(status)) {
        int signalCode = WTERMSIG(status);
        snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash [sign:%d] %% ", signalCode);
    }
}

void executeCommand(char input[]) {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        const char errorMessage[] = "Fork failed\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: execute the command
        //printf("Child process PID: %d\n", getpid()); display PID and simulate a delay
        //fflush(stdout);
        //sleep (80) ;
        execlp(input, input, NULL);

        // If exec fails
        const char errorMessage[] = "Command not found\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child to complete
        wait(&status);
    }
}

int main(int argc, char **argv) {
    // Display shell welcome message
    shellDisplay();

    // Initialize prompt
    snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash %% ");

    while (1) {
        // Display the prompt
        write(STDOUT_FILENO, waitingPrompt, strlen(waitingPrompt));

        // Read user input
        ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input) - 1);
        if (bytesRead <= 0) {
            write(STDOUT_FILENO, "\n", 1);
            break; // Exit on Ctrl+D
        }

        // Null-terminate the input and remove newline
        input[bytesRead - 1] = '\0';

        // Handle "exit" command
        if (strcmp(input, "exit") == 0) {
            write(STDOUT_FILENO, exitSuccess, strlen(exitSuccess));
            break;
        }

        // Execute the command
        executeCommand(input);

        // Update the prompt
        return_code();
    }

    return EXIT_SUCCESS;
}