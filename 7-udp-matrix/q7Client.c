#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];
    int rows, cols;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Get matrix dimensions
    printf("Enter number of rows and columns of the matrix: ");
    scanf("%d %d", &rows, &cols);

    // Send the dimensions to the server
    sprintf(buffer, "%d %d", rows, cols);
    sendto(sock, buffer, strlen(buffer), 0,
           (const struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // Clear input buffer
    while (getchar() != '\n');

    // Send each row of the matrix
    for (int i = 0; i < rows; i++) {
        printf("Enter row %d (space-separated values): ", i + 1);
        fgets(buffer, BUF_SIZE, stdin);

        // Remove trailing newline if present
        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';

        sendto(sock, buffer, strlen(buffer), 0,
               (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
        printf("Row %d sent to server.\n", i + 1);
    }

    close(sock);
    return 0;
}
