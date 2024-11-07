#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to encrypt the message by adding 4 to the ASCII value of each character
void encryptMessage(char *message) {
    for (int i = 0; message[i] != '\0'; i++){
        message[i] += 4;  // Add 4 to each character's ASCII value
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    char buffer[BUF_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert address to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Get user input for the message
    printf("Enter message to encrypt and send: ");
    fgets(message, BUF_SIZE, stdin);

    // Encrypt the message
    encryptMessage(message);

    // Send encrypted message to server
    send(sock, message, strlen(message), 0);

    // Close the socket
    close(sock);
    return 0;
}
