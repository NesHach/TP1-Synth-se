#include <unistd.h>  // For write, fork, execlp
#include <stdlib.h>  // For exit, EXIT_SUCCESS
#include <stdio.h>   // For snprintf
#include <string.h>  // For strlen
#include <fcntl.h>   // For open/creat
#include <sys/wait.h> // For wait
#include <time.h>    // For clock_gettime

#define MAX_INPUT_SIZE 256

// File Descriptors
int terminal = STDOUT_FILENO; // Sending arguments to terminal
int fd_input = STDIN_FILENO;  // Getting arguments

int status;

// Input variables
char input[MAX_INPUT_SIZE];
char waitingPrompt[MAX_INPUT_SIZE] = "";
char exitSuccess[] = "\nEnd of ShellENSEA\nBye bye...\n";

// Timing variables
struct timespec starttime, endtime;
double time_elapsed;

void shellDisplay(void) {
    // Informational Messages
    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    write(terminal, welcomeMessage, strlen(welcomeMessage));
}

void command(char input[]) {
    if (strcmp(input, "exit") == 0) {  // Handle "exit" command
        write(terminal, exitSuccess, strlen(exitSuccess));
        exit(EXIT_SUCCESS);
    }

    pid_t pid = fork();

    if (pid < 0) {
        const char errorMessage[] = "Fork failed\n";
        write(terminal, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        //printf("Child process PID: %d\n", getpid()); display PID and simulate a delay
        //fflush(stdout);
        //sleep (80) ;
        // Child process: execute the command
        execlp(input, input, NULL);

        // If exec fails
        const char errorMessage[] = "Command not found\n";
        write(terminal, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else {
        // Parent process: wait for the child
        wait(&status);
    }
}

void return_code(void) {
    // Calculate elapsed time in milliseconds
    time_elapsed = (endtime.tv_sec - starttime.tv_sec) * 1000 + (endtime.tv_nsec - starttime.tv_nsec) / 1e6;

    // Update the prompt with exit code or signal
    if (WIFEXITED(status)) {
        int exitCode = WEXITSTATUS(status);
        snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash [exit:%d|%.0f ms] %% ", exitCode, time_elapsed);
    } else if (WIFSIGNALED(status)) {
        int signalCode = WTERMSIG(status);
        snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash [sign:%d|%.0f ms] %% ", signalCode, time_elapsed);
    }
}

int main(int argc, char **argv) {
    shellDisplay();

    // Initialize prompt
    snprintf(waitingPrompt, sizeof(waitingPrompt), "enseash %% ");

    while (1) {
        // Display the prompt
        write(terminal, waitingPrompt, strlen(waitingPrompt));

        // Read user input
        ssize_t bytesRead = read(fd_input, input, sizeof(input) - 1);
        if (bytesRead <= 0) {
            write(terminal, "\n", 1);
            break; // Exit on Ctrl+D
        }

        // Null-terminate the input and remove newline
        input[bytesRead - 1] = '\0';

        // Record start time
        clock_gettime(CLOCK_MONOTONIC, &starttime);

        // Execute the command
        command(input);

        // Record end time
        clock_gettime(CLOCK_MONOTONIC, &endtime);

        // Update the prompt
        return_code();
    }

    return EXIT_SUCCESS;
}