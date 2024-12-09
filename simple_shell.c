/*
project: 01
author:Anna Downey
email: adowney1@umbc.edu
student id: EL64253
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

/*
In this project, you are going to implement a number of functions to
create a simple linux shell interface to perform basic linux commands
*/


//DEFINE THE FUNCTION PROTOTYPES
void user_prompt_loop();
char* get_user_command();
void proc_run(char** pComm);
void append_to_history(const char* uComm);
void displayHistory();
char** parse_command(char* uComm);
void execute_command(char** pComm, char* gComm);


int main(int argc, char** argv)
{
    /*
    Write the main function that checks the number of argument passed to ensure
    no command-line arguments are passed; if the number of argument is greater
    than 1 then exit the shell with a message to stderr and return value of 1
    otherwise call the user_prompt_loop() function to get user input repeatedly
    until the user enters the "exit" command.
    */

    if (argc <= 1) {
        user_prompt_loop();
    }
    else {
        fprintf(stderr, "Too many args");
        return 1;
    }

    return 0;
}

/*
user_prompt_loop():
Get the user input using a loop until the user exits, prompting the user for a command.
Gets command and sends it to a parser, then compares the first element to the two
different commands ("/proc", and "exit"). If it's none of the commands,
send it to the execute_command() function. If the user decides to exit, then exit 0 or exit
with the user given value.
*/

void user_prompt_loop()
{
    // Initialize variables
    int exitRequested = 0;

    // Main loop: Continue until exitRequested is set to 1
    while (!exitRequested) {
        // Display the prompt symbol ('$') to the user
        printf("$ ");

        // Get a command from the user
        char* userCommand = get_user_command();

        // Append the user's command to the command history
        append_to_history(userCommand);

        // Check if the user entered an empty command
        if (userCommand[0] == '\0') {
            // Free memory for an empty command
            free(userCommand);
        }
        else {
            // Tokenize the user's command
            char** commandTokens = parse_command(userCommand);

            // Check if the command is "exit"
            if (strcmp(commandTokens[0], "exit") == 0) {
                int tokenCount = 0;
                char* token = commandTokens[tokenCount];

                // Count the number of tokens in the command
                while (token != NULL) {
                    tokenCount++;
                    token = commandTokens[tokenCount];
                }

                // Handle the "exit" command based on the number of tokens
                switch (tokenCount) {
                case 1: {
                    // Free memory and exit the program with status 0
                    free(userCommand);
                    free(commandTokens);
                    exit(0);
                    exitRequested = 1; // Set the exit flag to terminate the loop
                    break;
                }
                case 2: {
                    // Check if the second token is not a valid number
                    if (!isdigit(*commandTokens[1])) {
                        printf("Invalid input\n", commandTokens[1]);
                        break;
                    }
                }
                default: {
                    if (tokenCount >= 3) {
                        printf("Invalid exit\n");
                    }
                    else {
                        // Convert the second token to an integer
                        int val = atoi(commandTokens[1]);

                        // Free memory and exit the program with the specified status
                        free(userCommand);
                        free(commandTokens);
                        exit(val);

                        // Set the exit flag to terminate the loop
                        exitRequested = 1;
                    }
                    break;
                }
                }
            }

            // Check if the command is "/proc"
            if (strcmp(commandTokens[0], "/proc") == 0) {
                // Execute the "proc" command
                proc_run(commandTokens);
            }

            // Check if the command is "history"
            if (strcmp(commandTokens[0], "history") == 0) {
                // Display the command history
                displayHistory();
            }

            // Check if the command is not "exit", "/proc", or "history" (other commands)
            if (strcmp(commandTokens[0], "exit") != 0 && strcmp(commandTokens[0], "/proc") != 0 && strcmp(commandTokens[0], "history") != 0) {
                // Execute the user's command
                execute_command(commandTokens, userCommand);

                // Free memory for the user's command and tokens
                free(userCommand);
                free(commandTokens);
            }
        }
    }
}

void proc_run(char** commandTokens) {
    // Check if the second token (process ID) is provided
    if (commandTokens[1] != NULL) {
        // Define a prefix for the "/proc" folder
        char* folderPrefix = "/proc/";

        // Calculate the length needed for the full file location
        int fileLocationLen = strlen(commandTokens[1]) + strlen(folderPrefix) + 1;

        // Allocate memory for the file location string
        char* fileLocation = (char*)malloc(fileLocationLen);

        // Construct the full file location by concatenating folderPrefix and the process ID
        strcpy(fileLocation, folderPrefix);
        strcat(fileLocation, commandTokens[1]);

        // Open the file located at the specified path
        FILE* procFile = fopen(fileLocation, "r");

        // Check if the file was successfully opened
        if (procFile != NULL) {
            // Read characters from the file until the end of file (EOF) is reached
            char fileChar = fgetc(procFile);
            while (fileChar != EOF) {
                // Print each character to the console
                printf("%c", fileChar);
                fileChar = fgetc(procFile);
            }

            // Print a newline character to separate the output
            printf("\n");

            // Close the file
            fclose(procFile);
        }
        else {
            // Print an error message if the file could not be opened
            printf("File not found\n");
        }

        // Free the dynamically allocated memory for fileLocation
        free(fileLocation);
    }
    else {
        // Print an error message if the process ID is not provided
        printf("Invalid proc\n");
    }
}

void append_to_history(const char* uComm) {
    // Get the user's home directory path
    char* home_directory = getenv("HOME");

    // Check if the home directory path is valid
    if (home_directory != NULL) {
        // Create a buffer for the history file path
        char history_file_path[256];

        // Construct the full path to the history file within the home directory
        snprintf(history_file_path, sizeof(history_file_path), "%s/.421sh", home_directory);

        // Open the history file in append mode
        FILE* history_file = fopen(history_file_path, "a");

        // Check if the history file was opened successfully
        if (history_file != NULL) {
            // Write the executed command to the history file followed by a newline
            fprintf(history_file, "%s\n", uComm);

            // Close the history file
            fclose(history_file);
        }
        else {
            // Print an error message to the standard error if the file couldn't be opened
            fprintf(stderr, "Failed to open the history file for appending.\n");
        }
    }
}

void displayHistory() {
    // Get the user's home directory path
    char* home_directory = getenv("HOME");

    // Check if the home directory path is valid
    if (home_directory != NULL) {
        // Create a buffer for the history file path
        char history_file_path[256];

        // Construct the full path to the history file within the home directory
        snprintf(history_file_path, sizeof(history_file_path), "%s/.421sh", home_directory);

        // Open the history file for reading
        FILE* history_file = fopen(history_file_path, "r");

        // Check if the history file was opened successfully
        if (history_file != NULL) {
            char line[256];

            // Read lines from the history file and print them to the console
            while (fgets(line, sizeof(line), history_file) != NULL) {
                printf("%s", line);
            }

            // Close the history file
            fclose(history_file);
        }
        else {
            // Print an error message to the standard error if the file couldn't be opened
            fprintf(stderr, "Failed to open the history file for reading.\n");
        }
    }
}

/*
get_user_command():
Take input of arbitrary size from the user and return to the user_prompt_loop()
*/

char* get_user_command()
{
    // Initial buffer size for user input
    int bufferSize = 50;

    // Index to keep track of the current position in the input buffer
    int idx = 0;

    // Allocate memory for the input buffer
    char* inputBuffer = (char*)malloc(bufferSize);

    do {
        // Read a character from the standard input (stdin)
        char current = fgetc(stdin);

        // Exit the loop when the user hits the Enter key (newline character)
        if (current == '\n') {
            break;
        }

        // Store the current character in the input buffer
        inputBuffer[idx] = current;

        // Increment the index to move to the next position in the buffer
        idx = idx + 1;

        // Check if the buffer is full, and if so, increase its size
        if (idx == (bufferSize - 1)) {
            bufferSize = bufferSize + 50;

            // Reallocate memory to accommodate the expanded buffer size
            char* newBuffer = (char*)realloc(inputBuffer, bufferSize);

            // Update the inputBuffer to point to the new memory location
            inputBuffer = newBuffer;
        }
    } while (1);  // Continue reading until the user hits Enter

    // Terminate the input buffer with a null character to create a C-style string
    inputBuffer[idx] = '\0';

    // Return the input buffer containing the user's command
    return inputBuffer;
}

/*
parse_command():
Take command grabbed from the user and parse appropriately.
Example:
    user input: "ls -la"
    parsed output: ["ls", "-la", NULL]
Example:
    user input: "echo     hello                     world  "
    parsed output: ["echo", "hello", "world", NULL]
*/

char** parse_command(char* uComm)
{
    // Maximum number of tokens expected in the input command
    int maxTokens = 15;

    // Index to keep track of the current position in the tokenArray
    int idx = 0;

    // Allocate memory for an array of char pointers (tokens)
    char** tokenArray = (char**)calloc(maxTokens, sizeof(char*));

    int spaceIdx;
    const char* remainingInput = uComm;

    do {
        // Find the position of the first unquoted space in the remaining input
        spaceIdx = first_unquoted_space(remainingInput);

        if (spaceIdx == -1) {
            // No unquoted space found, exit the loop
            break;
        }

        // Allocate memory for the token and copy characters up to the unquoted space
        char* token = (char*)malloc(spaceIdx + 1);
        strncpy(token, remainingInput, spaceIdx);
        token[spaceIdx] = '\0';

        // Unescape the token and store it in the tokenArray
        tokenArray[idx] = unescape(token, stderr);

        // Move the remaining input pointer to the next character after the space
        remainingInput += spaceIdx + 1;
        idx++;

        if (idx == (maxTokens - 1)) {
            // Increase the maximum token count and reallocate memory if needed
            maxTokens = maxTokens + 5;
            char** newTokenArray = (char**)realloc(tokenArray, maxTokens * sizeof(char*));

            // Update the tokenArray to point to the new memory location
            tokenArray = newTokenArray;
        }
    } while (1);

    // Unescape the remaining input and store it as the last token in the tokenArray
    tokenArray[idx] = unescape(remainingInput, stderr);

    // Return the tokenArray containing the parsed command tokens
    return tokenArray;
}

/*
execute_command():
Execute the parsed command if the commands are neither /proc nor exit;
fork a process and execute the parsed command inside the child process
*/

void execute_command(char** pComm, char* gComm)
{
    // Create a new process by forking the current process
    pid_t execute = fork();

    if (execute < 0) {
        // Forking failed, print an error message
        printf("Forking failed\n");
    }

    if (execute == 0) {
        // This code block is executed in the child process

        // Execute the command specified in pComm[0] with its arguments
        int valid = execvp(pComm[0], pComm);

        // Free memory for the global command and the parsed command tokens
        free(gComm);
        free(pComm);

        // Exit the child process
        exit(0);
    }

    if (execute > 0) {
        // This code block is executed in the parent process

        // Wait for the child process to complete
        wait(NULL);
    }
}

