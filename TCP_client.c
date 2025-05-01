#include <asm-generic/socket.h>
#include <bits/sockaddr.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  // Create socket
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  /* Error Handling */
  if (fd < 0) {
    perror("Error Creating Socket");
    abort();
  }

  // Set socket address
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

  // Connect the Client to the Server
  int sock = connect(fd, (struct sockaddr *)&addr, sizeof(addr));

  /* Error Handling */
  if (sock < 0) {
    perror("Error Connecting Socket");
    abort();
  }

  // Have the Socket do Something
  char client_msg[] = "Hello World";
  write(fd, client_msg, strlen(client_msg));

  char read_buf[256];
  ssize_t server_msg = read(sock, read_buf, sizeof(read_buf) - 1);
  if (server_msg < 0) {
    perror("Error in Receiving Message");
  } else if (server_msg == 0) {
    printf("EOF\n");
  } else {
    printf("%s\n", read_buf);
  }

  // Close the Socket
  sock = close(sock);
}
