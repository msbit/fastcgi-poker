#pragma once

#include <stdbool.h>
#include <stdio.h>

// Types
#define FCGI_TYPE_MIN 1
#define FCGI_TYPE_MAX 8

#define FCGI_BEGIN_REQUEST 1
#define FCGI_ABORT_REQUEST 2
#define FCGI_END_REQUEST 3
#define FCGI_PARAMS 4
#define FCGI_STDIN 5
#define FCGI_STDOUT 6
#define FCGI_STDERR 7
#define FCGI_DATA 8

typedef struct {
  unsigned char version;
  unsigned char type;
  unsigned char request_id[2];
  unsigned char content_length[2];
  unsigned char padding_length;
  unsigned char reserved;
} fcgi_header_t;

typedef struct {
  unsigned char role[2];
  unsigned char flags;
  unsigned char reserved[5];
} fcgi_begin_request_t;

typedef struct {
  unsigned char app_status[4];
  unsigned char protocol_status;
  unsigned char reserved[3];
} fcgi_end_request_t;

// Roles
#define FCGI_RESPONDER 1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER 3

// Flags
#define FCGI_KEEP_CONN 1

// Statuses
#define FCGI_REQUEST_COMPLETE 0
#define FCGI_CANT_MPX_CONN 1
#define FCGI_OVERLOADED 2
#define FCGI_UNKNOWN_ROLE 3

bool fcgi_begin_request(int);
bool fcgi_body(int, unsigned char, const char *); 
bool set_payload(int, char **, char *, char *);
