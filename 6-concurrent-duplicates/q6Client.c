#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    char buffer[BUF_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert address to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported\n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    while (1) {
        // Get user input
        printf("Enter a sentence (or type 'Stop' to terminate): ");
        fgets(message, BUF_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';  // Remove trailing newline

        // Send the message to the server
        send(sock, message, strlen(message), 0);

        // Check for "Stop" command
        if (strcmp(message, "Stop") == 0) {
            printf("Terminating client...\n");
            break;
        }

        // Receive the response from the server
        int bytes_read = read(sock, buffer, BUF_SIZE);
        buffer[bytes_read] = '\0';  // Null-terminate the received message

        // Display the cleaned sentence
        printf("Cleaned sentence from server: %s\n", buffer);
    }

    // Close the socket
    close(sock);
    return 0;
}
