#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to decrypt the message by subtracting 4 from the ASCII value of each character
void decryptMessage(char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        message[i] -= 4;  // Subtract 4 from each character's ASCII value
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUF_SIZE] = {0};

    // Create socket file descriptor
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

    printf("Waiting for a connection...\n");

    // Accept connection from client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Read the encrypted message from client
    int bytes_read = read(new_socket, buffer, BUF_SIZE);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the received message
        printf("Encrypted message received: %s\n", buffer);

        // Decrypt the message
        decryptMessage(buffer);
        printf("Decrypted message: %s\n", buffer);
    }

    // Close the connection
    close(new_socket);
    close(server_fd);
    return 0;
}
