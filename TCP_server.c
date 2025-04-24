#include <asm-generic/socket.h>
#include <bits/sockaddr.h>
#include <errno.h>
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
  // Create Socket
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  /* Error Handling */
  if (fd == -1) {
    printf("Error creating socket.");
    return -1;
  };

  // Socket Options
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  // Bind Socket to the adddress
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = htonl(0);

  int sock = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (sock == -1) {
    printf("Error binding socket");
    return -1;
  };

  // Have Socket listen for a client
  sock = listen(fd, SOMAXCONN);
  if (sock == -1) {
    printf("Error in Socket Listening");
    return -1;
  };

  // Connect a client
  for (;;) {
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int conn_fd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connect < 0) {
      /* error handling */
      return 0;
    };

    /* Have Server Do something */
    conn_fd = close(conn_fd);
  };
};
