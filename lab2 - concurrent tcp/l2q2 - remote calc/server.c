#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to perform arithmetic operations
int perform_operation(int num1, int num2, char operator) {
    switch (operator) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': 
            if (num2 != 0) 
                return num1 / num2;
            else 
                return 0; // Return 0 to indicate division by zero error
        default: return 0; // Return 0 for unsupported operators
    }
}

// Thread function to handle each client
void *handle_client(void *client_socket) {
    int client_fd = *((int *)client_socket);
    free(client_socket); // Free the dynamically allocated socket descriptor
    int num1, num2, result;
    char operator;

    // Receive two integers and operator from client
    read(client_fd, &num1, sizeof(int));
    read(client_fd, &num2, sizeof(int));
    read(client_fd, &operator, sizeof(char));

    // Perform the arithmetic operation
    result = perform_operation(num1, num2, operator);

    // Send the result back to the client
    write(client_fd, &result, sizeof(int));

    // Close the client socket
    close(client_fd);

    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
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

    printf("Server listening on port %d\n", PORT);

    // Accept multiple client connections concurrently
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Create a new thread to handle the client
        pthread_t thread_id;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_fd;
        pthread_create(&thread_id, NULL, handle_client, (void *)new_sock);
        pthread_detach(thread_id); // Allow threads to clean up after themselves
    }

    // Close the server socket
    close(server_fd);
    return 0;
}
