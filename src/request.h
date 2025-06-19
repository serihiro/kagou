#ifndef HEADER_REQUEST
#define HEADER_REQUEST

#include "util.h"

typedef struct {
  char *raw_request;
  Tuple *request_header_values;
  int keep_alive; // 1 for keep-alive, 0 for close
} Request;

#define MAX_HTTP_HEADERS 20
#define HTTP_HEADER_STRING_MAX_LENGTH                                          \
  257 // 256 + 1 chars (sufficient for most headers)

// Backward compatibility aliases
#define REQUEST_HEADER_ITEM_MAX_SIZE MAX_HTTP_HEADERS
#define REQUEST_HEADER_ITEM_STRING_LENGTH HTTP_HEADER_STRING_MAX_LENGTH

extern const char *ATTRIBUTE_DELIMITER;
extern const char *HEADER_KEY_METHOD;
extern const char *HEADER_KEY_PATH;
extern const char *HEADER_KEY_HTTP_VERSION;

Request *Request_new(char *raw_request);
void Request_delete(Request *this);
void _Request_scan(Request *this);
#endif
