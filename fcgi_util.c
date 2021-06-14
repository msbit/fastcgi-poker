#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "fcgi_util.h"

bool fcgi_begin_request(int fd) {
  fcgi_header_t header = {
    .version = 1,
    .type = FCGI_BEGIN_REQUEST,
    .request_id = {0, 1},
    .content_length = {0, sizeof(fcgi_begin_request_t)},
    .padding_length = 0,
    .reserved = 0
  };
  fcgi_begin_request_t body = {
    .role = {0, FCGI_RESPONDER}, 
    .flags = FCGI_KEEP_CONN, 
    .reserved = {0, 0, 0, 0, 0}
  };
  if (send(fd, &header, sizeof(fcgi_header_t), 0) != sizeof(fcgi_header_t)) {
    return false;
  }

  if (send(fd, &body, sizeof(fcgi_begin_request_t), 0) != sizeof(fcgi_begin_request_t)) {
    return false;
  }

  return true;
}

bool fcgi_body(int fd, unsigned char type, const char *payload) {
  size_t length = strlen(payload);
  int padding = 0;
  if (length % 8 != 0) {
    padding = (8 - (length % 8));
  }
  fcgi_header_t header = {
    .version = 1,
    .type = type, 
    .request_id = {0, 1},
    .content_length = {length >> 8, length & 0xff},
    .padding_length = padding,
    .reserved = 0
  };
  if (send(fd, &header, sizeof(fcgi_header_t), 0) != sizeof(fcgi_header_t)) {
    return false;
  }

  length += padding;
  char *buffer = calloc(length, sizeof(char));
  memcpy(buffer, payload, length);
  bool result = send(fd, payload, length, 0) == length;

  free(buffer);
  return result;
}

bool set_payload(int argc, char **argv, char *start, char *end) {
  for (char *position = start; argc > 1 && position < end; argc -= 2, argv += 2) {
    const char *name = *argv;
    const char *value = *(argv + 1);

    size_t name_len = strlen(name);
    size_t value_len = strlen(value);

    *position++ = name_len;
    *position++ = value_len;

    memcpy(position, name, name_len);
    position += name_len;

    memcpy(position, value, value_len);
    position += value_len;
  }

  return argc == 0;
}
