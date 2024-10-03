#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to encrypt the message (add 4 to each letter's ASCII value)
void encrypt_message(char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        if ((message[i] >= 'A' && message[i] <= 'Z') || (message[i] >= 'a' && message[i] <= 'z')) {
            message[i] += 4;  // Encrypt by adding 4
        }
    }
}

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    char message[BUF_SIZE];

    // Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Get message from user
    printf("Enter message to encrypt: ");
    fgets(message, BUF_SIZE, stdin);
    message[strcspn(message, "\n")] = 0;  // Remove newline

    // Encrypt the message
    encrypt_message(message);

    // Send encrypted message to server
    send(sock_fd, message, strlen(message), 0);

    // Close the socket
    close(sock_fd);
    return 0;
}
