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
    int arr[BUF_SIZE], n, pid;

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

    // Get the number of elements in the array
    printf("Enter the number of elements in the array: ");
    scanf("%d", &n);

    // Get the array elements from the user
    printf("Enter the elements of the array: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    // Send the size of the array to the server
    send(sock, &n, sizeof(int), 0);

    // Send the integer array to the server
    send(sock, arr, sizeof(int) * n, 0);

    // Receive the sorted array from the server
    read(sock, arr, sizeof(int) * n);

    // Receive the process ID from the server
    read(sock, &pid, sizeof(pid));

    // Display the sorted array
    printf("Sorted array received from server: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Display the server's process ID
    printf("Server process ID: %d\n", pid);

    // Close the socket
    close(sock);

    return 0;
}
