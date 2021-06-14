#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "display.h"
#include "fcgi_util.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    return EXIT_FAILURE;
  }

  char payload[1024];
  memset(payload, 0, 1024);
  if (!set_payload(argc - 2, argv + 2, payload, payload + 1024)) {
    return EXIT_FAILURE;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) {
    return EXIT_FAILURE;
  }

  struct sockaddr_un addr;
  if (strlen(argv[1]) > sizeof(addr.sun_path) - 1) {
    return EXIT_FAILURE;
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, argv[1], sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    perror("connect");
    return EXIT_FAILURE;
  }

  if (!fcgi_begin_request(fd)) {
    perror("fcgi_begin_request");
    return EXIT_FAILURE;
  }
  if (!fcgi_body(fd, FCGI_PARAMS, payload)) {
    perror("fcgi_body");
    return EXIT_FAILURE;
  }
  if (!fcgi_body(fd, FCGI_STDIN, "")) {
    perror("fcgi_body");
    return EXIT_FAILURE;
  }

  char buffer[8192];

  while (true) {
    fcgi_header_t header;
    memset(&header, 0, sizeof(fcgi_header_t));

    ssize_t count = recv(fd, &header, sizeof(fcgi_header_t), MSG_WAITALL);
    if (count < 0) {
      perror("recv (1)");
      return EXIT_FAILURE;
    }

    if (count == 0) { break; }

    print_header(stderr, &header);

    if (header.type == FCGI_STDOUT) {
      int remaining = (header.content_length[0] << 8) + header.content_length[1] + header.padding_length;
      while (remaining > 0) {
        int length = remaining;
        if (length > 8191) {
          length = 8191;
        }
        count = recv(fd, buffer, length, 0);
        if (count < 0) {
          perror("recv (2)");
          return EXIT_FAILURE;
        }

        remaining -= count;

        buffer[count] = '\0';

        printf("%s", buffer);
      }
      continue;
    }

    if (header.type == FCGI_END_REQUEST) {
      fcgi_end_request_t body;
      count = recv(fd, &body, sizeof(fcgi_end_request_t), 0);
      if (count < 0) {
        perror("recv (3)");
        return EXIT_FAILURE;
      }

      if (count < sizeof(fcgi_end_request_t)) { break; }

      print_end_request(stderr, &body);
      break;
    }
  }

  return EXIT_SUCCESS;
}
