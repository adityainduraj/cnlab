#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to get the current date and time
void getCurrentTime(char *buffer) {
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, BUF_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE];
    pid_t pid;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set up the address struct
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening for connections...\n");

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Fork to handle multiple clients concurrently
        if (fork() == 0) {
            close(server_fd); // Child doesn't need the listener socket

            // Get current date and time
            getCurrentTime(buffer);

            // Get the process ID
            pid = getpid();

            // Send the date, time, and PID to the client
            send(new_socket, buffer, BUF_SIZE, 0);
            send(new_socket, &pid, sizeof(pid), 0);

            printf("Sent date and time along with PID: %d to client\n", pid);

            close(new_socket); // Close the connection for this client
            exit(0); // Child process exits after handling client
        }

        // Parent process closes the socket for this client
        close(new_socket);

        // Clean up child processes that have finished
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    return 0;
}
