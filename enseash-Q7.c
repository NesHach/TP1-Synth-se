#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_INPUT_SIZE 256
#define MAX_ARG_COUNT 16

// Global variables for timing and process status
struct timespec starttime, endtime;
double time_elapsed;
int status;

// Display the welcome message
void displayWelcomeMessage(void) {
    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    write(STDOUT_FILENO, welcomeMessage, strlen(welcomeMessage));
}

// Update the dynamic prompt with time and exit/sign status
void updatePrompt(char *waitingPrompt) {
    if (WIFEXITED(status)) {
        snprintf(waitingPrompt, MAX_INPUT_SIZE, "enseash [exit:%d|%.0f ms] %% ", WEXITSTATUS(status), time_elapsed);
    } else if (WIFSIGNALED(status)) {
        snprintf(waitingPrompt, MAX_INPUT_SIZE, "enseash [sign:%d|%.0f ms] %% ", WTERMSIG(status), time_elapsed);
    } else {
        snprintf(waitingPrompt, MAX_INPUT_SIZE, "enseash %% ");
    }
}

// Parse user input to detect arguments and redirections
int parseInput(char *input, char **args, char **inputFile, char **outputFile) {
    int argCount = 0;
    char *token = strtok(input, " ");

    *inputFile = NULL;
    *outputFile = NULL;

    while (token != NULL) {
        if (strcmp(token, "<") == 0) { // Input redirection
            token = strtok(NULL, " ");
            *inputFile = token;
        } else if (strcmp(token, ">") == 0) { // Output redirection
            token = strtok(NULL, " ");
            *outputFile = token;
        } else { // Normal argument
            args[argCount++] = token;
        }
        token = strtok(NULL, " ");
    }

    args[argCount] = NULL; // Null-terminate the arguments array
    return argCount;
}

// Execute the command with optional redirections
void executeCommand(char **args, char *inputFile, char *outputFile) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Handle input redirection
        if (inputFile) {
            int fd = open(inputFile, O_RDONLY);
            if (fd < 0) {
                perror("Input redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Handle output redirection
        if (outputFile) {
            int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Output redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Execute the command
        execvp(args[0], args);
        perror("Command execution failed"); // If execvp fails
        exit(EXIT_FAILURE);
    } else {
        // Parent process waits for the child
        clock_gettime(CLOCK_MONOTONIC, &starttime);
        wait(&status);
        clock_gettime(CLOCK_MONOTONIC, &endtime);

        // Calculate execution time in milliseconds
        time_elapsed = (endtime.tv_sec - starttime.tv_sec) * 1000.0 +
                       (endtime.tv_nsec - starttime.tv_nsec) / 1e6;
    }
}

// Main loop of the shell
void shellLoop(void) {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARG_COUNT];
    char *inputFile, *outputFile;
    char waitingPrompt[MAX_INPUT_SIZE] = "enseash %% ";

    while (1) {
        // Display the prompt
        write(STDOUT_FILENO, waitingPrompt, strlen(waitingPrompt));

        // Read user input
        ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input) - 1);
        if (bytesRead <= 0) {
            write(STDOUT_FILENO, "\n", 1);
            break; // Exit on Ctrl+D
        }

        input[bytesRead - 1] = '\0'; // Remove the '\n' character
        if (strcmp(input, "exit") == 0) break;

        // Parse input and execute the command
        parseInput(input, args, &inputFile, &outputFile);
        executeCommand(args, inputFile, outputFile);

        // Update the prompt after execution
        updatePrompt(waitingPrompt);
    }
}

int main(void) {
    displayWelcomeMessage();
    shellLoop();

    const char exitMessage[] = "End of ShellENSEA\nBye bye...\n";
    write(STDOUT_FILENO, exitMessage, strlen(exitMessage));
    return EXIT_SUCCESS;
}