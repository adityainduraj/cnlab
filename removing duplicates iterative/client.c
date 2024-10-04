// client.c
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define PORTNO 10200

int main() {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // Create client socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Setup server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
    serv_addr.sin_port = htons(PORTNO);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    while (1) {
        // Get sentence input from user
        printf("Enter a sentence (or type 'Stop' to terminate): ");
        memset(buffer, 0, 1024); // Clear the buffer
        fgets(buffer, 1023, stdin); // Read input from user

        // Send the sentence to the server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to socket");
            return 1;
        }

        // Check if the user wants to stop
        if (strncmp("Stop", buffer, 4) == 0) {
            printf("Terminating client.\n");
            break;
        }

        // Read the modified sentence from the server
        memset(buffer, 0, 1024);
        n = read(sockfd, buffer, 1023);
        if (n < 0) {
            perror("Error reading from socket");
            return 1;
        }

        // Display the modified sentence
        printf("Modified sentence from server: %s\n", buffer);
    }

    // Close the connection
    close(sockfd);
    return 0;
}
