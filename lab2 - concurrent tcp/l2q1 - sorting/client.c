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
    int arr[BUF_SIZE];
    int n;

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

    // Input array size and elements
    printf("Enter the number of elements in the array: ");
    scanf("%d", &n);

    printf("Enter the array elements:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    // Send array size to server
    write(sock_fd, &n, sizeof(int));

    // Send array to server
    write(sock_fd, arr, sizeof(int) * n);

    // Receive sorted array from server
    read(sock_fd, arr, sizeof(int) * n);

    // Receive server process ID
    pid_t pid;
    read(sock_fd, &pid, sizeof(pid_t));

    // Display sorted array and server's process ID
    printf("Sorted array received from server:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nServer process ID: %d\n", pid);

    // Close the socket
    close(sock_fd);

    return 0;
}
