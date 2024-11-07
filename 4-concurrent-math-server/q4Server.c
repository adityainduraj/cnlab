#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define BUF_SIZE 1024

int server_fd; // Global variable to handle cleanup in signal handler

// Signal handler for proper cleanup
void cleanup(int sig) {
    printf("\nCleaning up and shutting down server...\n");
    close(server_fd);
    exit(0);
}

// Function to perform arithmetic operations
int performOperation(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return (b != 0) ? a / b : 0;
        default: return 0;
    }
}

int main() {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int a, b, result;
    char op;

    // Set up signal handlers for proper cleanup
    signal(SIGINT, cleanup);  // Handle Ctrl+C
    signal(SIGTERM, cleanup); // Handle termination signal

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set up the address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue; // Continue listening instead of exiting
        }

        // Fork to handle multiple clients concurrently
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(new_socket);
            continue;
        }

        if (pid == 0) { // Child process
            close(server_fd); // Child doesn't need the listener socket

            // Read the two integers and the operator from the client
            if (read(new_socket, &a, sizeof(int)) <= 0 ||
                read(new_socket, &b, sizeof(int)) <= 0 ||
                read(new_socket, &op, sizeof(char)) <= 0) {
                printf("Error reading from client\n");
                close(new_socket);
                exit(1);
            }

            // Perform the operation
            result = performOperation(a, b, op);
            printf("Operation: %d %c %d = %d\n", a, op, b, result);

            // Send the result back to the client
            if (send(new_socket, &result, sizeof(int), 0) < 0) {
                printf("Error sending result to client\n");
            }

            close(new_socket);
            exit(0);
        }

        // Parent process
        close(new_socket); // Parent doesn't need this socket

        // Clean up zombie processes
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    return 0;
}
