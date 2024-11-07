#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    int a, b, result;
    char op;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert address to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Get two integers and operator from the user
    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);
    printf("Enter an operator (+, -, *, /): ");
    scanf(" %c", &op);

    // Send the integers and operator to the server
    send(sock, &a, sizeof(int), 0);
    send(sock, &b, sizeof(int), 0);
    send(sock, &op, sizeof(char), 0);

    // Receive the result from the server
    valread = read(sock, &result, sizeof(int));

    // Display the result
    printf("Result from server: %d %c %d = %d\n", a, op, b, result);

    // Close the socket
    close(sock);

    return 0;
}
