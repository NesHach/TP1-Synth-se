#include <unistd.h>  // For write, STDOUT_FILENO
#include <stdlib.h>  // For exit, EXIT_SUCCESS
#include <string.h>  // For strlen

void shellDisplay(void) {
    // Informational messages
    const char welcomeMessage[] = "Welcome to ShellENSEA! \nType 'exit' to quit\n";
    const char waitingPrompt[] = "enseash %\n";

    // Display the messages
    write(STDOUT_FILENO, welcomeMessage, strlen(welcomeMessage));
    write(STDOUT_FILENO, waitingPrompt, strlen(waitingPrompt));
}

int main(int argc, char** argv) {
    // Display the shell interface
    shellDisplay();

    // Exit the program successfully
    return EXIT_SUCCESS;
}