// server.c
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<ctype.h>
#include<stdlib.h>
#define PORTNO 10200

// Function to remove duplicate words from the sentence
void removeDuplicates(char *sentence) {
    char *words[256]; // Array to hold the words from the sentence
    int count[256] = {0}; // Array to count occurrences of each word
    int wordIndex = 0;
    char result[1024] = "";
    char *word = strtok(sentence, " \n"); // Tokenize the sentence into words

    // Process each word
    while (word != NULL) {
        // Check if the word is already in the array
        int found = 0;
        for (int i = 0; i < wordIndex; i++) {
            if (strcmp(words[i], word) == 0) {
                count[i]++;
                found = 1;
                break;
            }
        }
        // If the word is not a duplicate, add it to the array
        if (!found) {
            words[wordIndex] = word;
            count[wordIndex]++;
            wordIndex++;
        }
        word = strtok(NULL, " \n");
    }

    // Construct the result sentence with unique words
    for (int i = 0; i < wordIndex; i++) {
        strcat(result, words[i]);
        strcat(result, " ");
    }

    // Copy the result back into the sentence
    strcpy(sentence, result);
}

int main() {
    int sockfd, newsockfd, clilen, n;
    struct sockaddr_in seraddr, cliaddr;
    char buffer[1024];

    // Create server socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Setup server address
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = INADDR_ANY;
    seraddr.sin_port = htons(PORTNO);

    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    // Start listening for connections
    listen(sockfd, 5);
    printf("Server is waiting for a connection...\n");

    // Accept a connection
    clilen = sizeof(cliaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
    if (newsockfd < 0) {
        perror("Error accepting connection");
        return 1;
    }

    while (1) {
        // Clear the buffer
        memset(buffer, 0, 1024);

        // Read the message from the client
        n = read(newsockfd, buffer, 1023);
        if (n < 0) {
            perror("Error reading from socket");
            return 1;
        }

        // Check if the client wants to stop the conversation
        if (strncmp("Stop", buffer, 4) == 0) {
            printf("Client requested to stop. Terminating server.\n");
            break;
        }

        printf("Received sentence: %s\n", buffer);

        // Remove duplicate words from the sentence
        removeDuplicates(buffer);

        // Send the modified sentence back to the client
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to socket");
            return 1;
        }
    }

    // Close the connection
    close(newsockfd);
    close(sockfd);
    return 0;
}
