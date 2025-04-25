#include <asm-generic/socket.h>
#include <bits/sockaddr.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  // Create Socket
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  /* Error Handling */
  if (fd < 0) {
    perror("Error Creating Socket");
    abort();
  }

  // Socket Options
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  // Bind Socket to the adddress
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(0);

  int sock = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (sock == -1) {
    perror("Error Binding Socket");
    abort();
  }

  // Have Socket listen for a client
  sock = listen(fd, SOMAXCONN);
  if (sock == -1) {
    perror("Error in Socket Listening");
    abort();
  }

  // Connect a client
  for (;;) {
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int conn_fd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (conn_fd < 0) {
      continue;
    }

    char read_buf[256] = {};

    ssize_t msg = read(conn_fd, read_buf, sizeof(read_buf) - 1);
    if (msg < 0) {
      perror("Error in Receiving Message");
    } else if (msg == 0) {
      printf("EOF\n");
    } else {
      printf("%s\n", read_buf);
    }

    /* Have Server Do something */
    conn_fd = close(conn_fd);
  }

  return 0;
}
