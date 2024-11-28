#include <unistd.h>   
#include <stdlib.h>   
#include <string.h>  
#include <sys/wait.h>  

#define MAX_INPUT_SIZE 256

// Input buffer
char input[MAX_INPUT_SIZE];
int status; // To capture the child process exit status

//  Function to convert an integer to a string
void intToStr(int num, char *str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
    } else {
        int temp = num;
        while (temp > 0) {
            temp /= 10;
            i++;
        }
        str[i] = '\0';
        while (num > 0) {
            str[--i] = (num % 10) + '0';
            num /= 10;
        }
    }
}

//  Function to dynamically update the prompt
void updatePrompt(const char *type, int code) {
    // Write the fixed part of the prompt
    const char base[] = "enseash [";
    write(STDOUT_FILENO, base, strlen(base));

    // Write the type (exit or sign)
    write(STDOUT_FILENO, type, strlen(type));

    // Write the separator ":"
    const char separator[] = ":";
    write(STDOUT_FILENO, separator, strlen(separator));

    // Convert the code to a string and write it
    char codeStr[12]; // Buffer to hold the converted code
    intToStr(code, codeStr);
    write(STDOUT_FILENO, codeStr, strlen(codeStr));

    // Write the closing part of the prompt
    const char closing[] = "] % ";
    write(STDOUT_FILENO, closing, strlen(closing));
}

// Function to execute the command
void executeCommand(const char *input) {
    pid_t pid = fork();

    if (pid < 0) {
        const char errorMessage[] = "Fork failed\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execlp(input, input, NULL);

        const char errorMessage[] = "Command not found\n";
        write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
        exit(EXIT_FAILURE);
    } else {
        wait(&status);

        //   Check the child's exit status and update the prompt
        if (WIFEXITED(status)) {
            // If the child exited normally, get the exit code
            int exitCode = WEXITSTATUS(status);
            updatePrompt("exit", exitCode); // Update prompt with exit code
        } else if (WIFSIGNALED(status)) {
            // If the child was terminated by a signal, get the signal code
            int signalCode = WTERMSIG(status);
            updatePrompt("sign", signalCode); // Update prompt with signal code
        }
    }
}

int main(int argc, char **argv) {
    //  Display initial prompt
    write(STDOUT_FILENO, "enseash-Q3 % ", 10);

    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    write(STDOUT_FILENO, welcomeMessage, strlen(welcomeMessage));

    while (1) {
        // Read user input
        ssize_t bytesRead = read(STDIN_FILENO, input, sizeof(input) - 1);

        if (bytesRead <= 0) {
            write(STDOUT_FILENO, "\n", 1);
            break;
        }

        input[bytesRead - 1] = '\0'; // Remove the newline character

        if (strncmp(input, "exit", 4) == 0 && (bytesRead == 5 || input[4] == '\0')) {
            break;
        }

        executeCommand(input);
    }

    return EXIT_SUCCESS;
}