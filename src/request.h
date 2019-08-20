#ifndef HEADER_REQUEST
#define HEADER_REQUEST

#include "util.h"

typedef struct {
  char *raw_request;
  Tuple *request_header_values;
} Request;

#define REQUEST_HEADER_VALUE_SIZE 10
#define RESPONSE_HEADER_VALUE_BUFFER_SIZE 1025 // 1KiB + 1byte

static const char *ATTRIBUTE_DELIMITER = " ";
static const char *HEADER_KEY_METHOD = "method";
static const char *HEADER_KEY_PATH = "path";
static const char *HEADER_KEY_HTTP_VERSION = "http_version";

Request *Request_new(char *raw_request);
void Request_delete(Request *this);
void _Request_scan(Request *this);
#endif