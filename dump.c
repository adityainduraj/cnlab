#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define BUF_SIZE = 1024

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[BUF_SIZE];
  socklen_t addr_len = sizeof(client_addr);

  // create the socket 
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // set server addresses
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  
  // bind socket 
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // listen for incoming connections 
  if (listen(server_fd, 3) < 0) {
    perror("listening failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("listening for incoming connections on port: %d\n", PORT);

  while(1) {
    // accept client connection 
    if (client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0 ) {
      perror("accepting failed");
      continue;
    }

    // read client message
    int n = read(client_fd, buffer, BUF_SIZE);
    buffer[n] = "\0";
    printf("client message is: %s\n", buffer);


    // echo it back to the server 
    write(client_fd, buffer, strlen(buffer));

    // close client 
    close(client_fd);
  }
  // close server
  close(server_fd);
  return 0;
}
