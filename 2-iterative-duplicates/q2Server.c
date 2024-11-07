#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to remove duplicate words
void removeDuplicates(char *input) {
    char *words[BUF_SIZE], result[BUF_SIZE] = "";
    int word_count = 0;

    // Tokenize input string
    char *token = strtok(input, " ");
    while (token != NULL) {
        int found = 0;
        // Check if token is already in the list of words
        for (int i = 0; i < word_count; i++) {
            if (strcmp(token, words[i]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            words[word_count++] = token;  // Add unique word
            strcat(result, token);  // Append to result
            strcat(result, " ");
        }
        token = strtok(NULL, " ");
    }

    // Copy result back to input
    strcpy(input, result);
}


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");

    // Accept connection from client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read the message from the client
        int bytes_read = read(new_socket, buffer, BUF_SIZE);
        buffer[bytes_read] = '\0';  // Null-terminate the message

        // Check for the "Stop" command
        if (strcmp(buffer, "Stop") == 0) {
            printf("Terminating server...\n");
            break;
        }

        // Remove duplicate words
        removeDuplicates(buffer);
        
        // Send the cleaned sentence back to the client
        send(new_socket, buffer, strlen(buffer), 0);
    }

    // Close the socket
    close(new_socket);
    close(server_fd);
    return 0;
}
