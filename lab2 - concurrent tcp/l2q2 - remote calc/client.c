#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    int num1, num2, result;
    char operator;

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

    // Get two integers and operator from the user
    printf("Enter first number: ");
    scanf("%d", &num1);
    printf("Enter second number: ");
    scanf("%d", &num2);
    printf("Enter operator (+, -, *, /): ");
    scanf(" %c", &operator);

    // Send numbers and operator to server
    write(sock_fd, &num1, sizeof(int));
    write(sock_fd, &num2, sizeof(int));
    write(sock_fd, &operator, sizeof(char));

    // Receive result from server
    read(sock_fd, &result, sizeof(int));

    // Display result
    printf("Result from server: %d\n", result);

    // Close the socket
    close(sock_fd);

    return 0;
}
