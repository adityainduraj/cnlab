#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

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

    while (1) {
        // Get message from user
        printf("Enter a sentence (or 'Stop' to terminate): ");
        fgets(message, BUF_SIZE, stdin);
        message[strcspn(message, "\n")] = 0;  // Remove newline character

        // Send message to server
        send(sock_fd, message, strlen(message), 0);

        // Check if user wants to stop
        if (strcmp(message, "Stop") == 0) {
            printf("Termination request sent. Closing client.\n");
            break;
        }

        // Clear buffer
        memset(buffer, 0, BUF_SIZE);

        // Receive modified sentence from server
        int n = read(sock_fd, buffer, BUF_SIZE);
        buffer[n] = '\0';  // Null-terminate the received string
        printf("Received from server: %s\n", buffer);
    }

    // Close the socket
    close(sock_fd);

    return 0;
}
