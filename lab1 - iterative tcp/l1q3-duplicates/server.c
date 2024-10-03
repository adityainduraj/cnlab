#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// Function to remove duplicate words from the sentence
void remove_duplicates(char *str) {
    char *words[BUF_SIZE];
    int word_count = 0;
    char *token = strtok(str, " ");
    
    // Tokenize and store words
    while (token != NULL) {
        int is_duplicate = 0;
        for (int i = 0; i < word_count; i++) {
            if (strcmp(words[i], token) == 0) {
                is_duplicate = 1;  // Mark as duplicate
                break;
            }
        }
        if (!is_duplicate) {
            words[word_count++] = token;  // Retain non-duplicate word
        }
        token = strtok(NULL, " ");
    }

    // Rebuild the sentence without duplicates
    str[0] = '\0';  // Clear original string
    for (int i = 0; i < word_count; i++) {
        strcat(str, words[i]);
        if (i < word_count - 1) strcat(str, " ");  // Add spaces between words
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
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

    // Accept client connection
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Clear buffer
        memset(buffer, 0, BUF_SIZE);
        
        // Receive message from client
        int n = read(client_fd, buffer, BUF_SIZE);
        buffer[n] = '\0';  // Null-terminate the received string
        printf("Received from client: %s\n", buffer);

        // Check if the client wants to stop
        if (strcmp(buffer, "Stop") == 0) {
            printf("Termination request received. Closing connection.\n");
            break;
        }

        // Process the sentence to remove duplicate words
        remove_duplicates(buffer);

        // Send the modified sentence back to the client
        send(client_fd, buffer, strlen(buffer), 0);
    }

    // Close connection
    close(client_fd);
    close(server_fd);

    return 0;
}
