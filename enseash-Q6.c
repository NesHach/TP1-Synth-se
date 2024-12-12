#include <unistd.h>  // For write, fork, execvp
#include <stdlib.h>  // For exit, EXIT_SUCCESS
#include <stdio.h>   // For snprintf
#include <string.h>  // For strtok, strlen
#include <sys/wait.h> // For wait
#include <time.h>    // For timing execution

#define MAX_INPUT_SIZE 256
#define MAX_ARG_COUNT 16

// Input buffer and status variables
char input[MAX_INPUT_SIZE];
char *args[MAX_ARG_COUNT]; // Array to store command and arguments
int status;                // To capture the child process exit status
char waitingPrompt[MAX_INPUT_SIZE] = ""; // Dynamic prompt buffer
char exitSuccess[] = "End of ShellENSEA\nBye bye...\n";

// Timing variables
struct timespec starttime, endtime;
double time_elapsed;

void shellDisplay(void) {
    // Informational Messages
    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    write(STDOUT_FILENO, welcomeMessage, strlen(welcomeMessage));
}

void parseInput(char *input) {
    // Tokenize the input string into command and arguments
    char *token;
    int i = 0;

    token = strtok(input, " "); // Split input by spaces
    while (token != NULL && i < MAX_ARG_COUNT - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the array
}

void return_code(void) {
    // Calculate elapsed time in milliseconds
    time_elapsed = (endtime.tv_sec - starttime.tv_sec) * 1000 +
                   (endtime.tv_nsec - starttime.tv_nsec) / 1e6;

    // Update the prompt with exit code or signal
    if (WIFEXITED(status)) {
        int exitCode = WEXITSTATUS(status);
        snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash [exit:%d|%.0f ms] %% ", exitCode, time_elapsed);
    } else if (WIFSIGNALED(status)) {
        int signalCode = WTERMSIG(status);
        snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash [sign:%d|%.0f ms] %% ", signalCode, time_elapsed);
    }
}

void executeCommand(char *input) {
    parseInput(input); // Parse the input into command and arguments

    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        const char errorMessage[] = "Fork failed\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: execute the command
        execvp(args[0], args);

        // If execvp fails
        const char errorMessage[] = "Command not found\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child to complete
        wait(&status);
    }
}

int main(int argc, char **argv) {
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

        // Record start time
        clock_gettime(CLOCK_MONOTONIC, &starttime);

        // Execute the command
        executeCommand(input);

        // Record end time
        clock_gettime(CLOCK_MONOTONIC, &endtime);

        // Update the prompt
        return_code();
    }

    return EXIT_SUCCESS;
}