#include <stdio.h>

#include "fcgi_util.h"

const char *protocol_statuses[] = {
  "FCGI_REQUEST_COMPLETE",
  "FCGI_CANT_MPX_CONN",
  "FCGI_OVERLOADED",
  "FCGI_UNKNOWN_ROLE"
};

const char *types[] = {
  NULL,
  "FCGI_BEGIN_REQUEST",
  "FCGI_ABORT_REQUEST",
  "FCGI_END_REQUEST",
  "FCGI_PARAMS",
  "FCGI_STDIN",
  "FCGI_STDOUT",
  "FCGI_STDERR",
  "FCGI_DATA"
};

void print_header(FILE *stream, const fcgi_header_t *header) {
  if (header->type < FCGI_TYPE_MIN || header->type > FCGI_TYPE_MAX) {
    fprintf(stream, "%s\n", (char *) header);
    fprintf(stream, "\n");
    return;
  }

  fprintf(stream, "version: %d\n", header->version);
  fprintf(stream, "type: %s\n", types[header->type]);
  fprintf(stream, "request_id: %d\n", (header->request_id[0] << 8) + header->request_id[1]);
  fprintf(stream, "content_length: %d\n", (header->content_length[0] << 8) + header->content_length[1]);
  fprintf(stream, "padding_length: %d\n", header->padding_length);
  fprintf(stream, "\n");
}

void print_end_request(FILE *stream, const fcgi_end_request_t *body) {
  fprintf(stream, "app_status: %d\n", (body->app_status[0] << 24) + (body->app_status[1] << 16) + (body->app_status[2] << 8) + body->app_status[3]);
  fprintf(stream, "protocol_status: %s\n", protocol_statuses[body->protocol_status]);
  fprintf(stream, "\n");
}
