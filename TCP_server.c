#include <asm-generic/socket.h>
#include <assert.h>
#include <bits/sockaddr.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MSG 4096

static int32_t read_all(int fd, char *buf, size_t len) {
  while (len > 0) {
    // read client message
    ssize_t msg = read(fd, buf, len);

    // Error and EOF check
    if (msg <= 0) {
      return -1;
    }

    // Ensure message size and perform appliocation protocol loop
    assert((size_t)msg <= len);
    len -= (size_t)msg;
    buf += msg;
  }
  return 0;
}

static int32_t write_all(int fd, char *buf, size_t len) {
  while (len > 0) {
    // Write Server Response
    ssize_t msg = write(fd, buf, len);

    // Error and EOF check
    if (msg <= 0) {
      return -1;
    }

    // Ensure message size and perform application protocol loop
    assert((size_t)msg <= len);
    len -= (size_t)msg;
    buf += msg;
  }
  return 0;
}

static int32_t application_loop(int client_fd) {
  char read_buf[4 + MAX_MSG] = {0};
  errno = 0;

  // read 4 byte message header
  int32_t msg = read_all(client_fd, read_buf, 4);
  if (msg) {
    fprintf(stderr, "%s\n", errno == 0 ? "EOF" : "read() error");
    return msg;
  }

  // check message length
  uint32_t len = 0;
  memcpy(&len, read_buf, 4);
  if (len > MAX_MSG) {
    fprintf(stderr, "%s\n", "msg too long");
    return -1;
  }

  // read message body
  msg = read_all(client_fd, &read_buf[4], len);
  if (msg) {
    fprintf(stderr, "%s\n", "read() error");
    return msg;
  }

  // Send response to the client
  printf("Client Message: %*s\n", len, &read_buf[4]);
  const char res[] = "Message Received";
  char write_buf[4 + sizeof(res)];
  len = (uint32_t)strlen(res);
  memcpy(write_buf, &len, 4);
  memcpy(&write_buf[4], res, len);
  return write_all(client_fd, write_buf, 4 + len);

  return 0;
}

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

    // Have Server Do something
    for (;;) {
      int32_t chat = application_loop(conn_fd);
      if (chat) {
        break;
      }
    }
    conn_fd = close(conn_fd);
  }

  return 0;
}
