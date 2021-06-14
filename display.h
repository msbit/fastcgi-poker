#pragma once

#include <stdio.h>

#include "fcgi_util.h"

void print_header(FILE *, const fcgi_header_t *); 
void print_end_request(FILE *, const fcgi_end_request_t *); 
