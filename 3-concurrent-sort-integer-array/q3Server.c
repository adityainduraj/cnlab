#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to sort an array
void sortArray(int arr[], int n) {
    int temp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] > arr[j]) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int arr[BUF_SIZE];
    int n;

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
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Fork to handle multiple clients concurrently
        if (fork() == 0) {
            close(server_fd); // Child doesn't need the listener socket

            // Read the array size from the client
            read(new_socket, &n, sizeof(int));

            // Read the integer array from the client
            read(new_socket, arr, sizeof(int) * n);

            printf("Received array from client. Sorting...\n");

            // Sort the array
            sortArray(arr, n);

            // Get the process ID
            pid_t pid = getpid();

            // Send sorted array back to the client
            send(new_socket, arr, sizeof(int) * n, 0);

            // Send the process ID back to the client
            send(new_socket, &pid, sizeof(pid), 0);

            printf("Sorted array sent back to client along with process ID: %d\n", pid);

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
