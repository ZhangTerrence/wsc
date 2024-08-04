#ifndef PARSER_H
#define PARSER_H

#include "http.h"

#define CRLF "\r\n"
#define SP " "

int parse_request_line(struct Request *request, char *request_line);

int parse_request(struct Request *request, char *request_string);

#endif
