#ifndef HEADER_RESPONSE
#define HEADER_RESPONSE

#include "util.h"

typedef struct {
  char *header;
  char *body;
  char *status;
  Tuple *header_values;
} Response;

#define MAX_RESPONSE_HEADERS 10
#define RESPONSE_HEADER_STRING_MAX_LENGTH                                      \
  257 // 256 + 1 chars (sufficient for most headers)
#define RESPONSE_HEADER_BUFFER_SIZE                                            \
  2049 // 2k + 1 chars (sufficient for response headers)

// Backward compatibility aliases
#define RESPONSE_HEADER_VALUE_SIZE MAX_RESPONSE_HEADERS
#define RESPONSE_HEADER_ITEM_STRING_LENGTH RESPONSE_HEADER_STRING_MAX_LENGTH
#define RESPONSE_HEADER_BUFFER_STRING_LENGTH RESPONSE_HEADER_BUFFER_SIZE
extern const char *HEADER_SEPARATOR;
extern const char *HEADER_LINE_BREAK_CODE;

Response *Response_new();
void Response_delete(Response *this);
void Response_set_header(Response *this, const char *header);
void Response_set_body_as_text(Response *this, const char *body);
void Response_set_status(Response *this, const char *status);
void Response_create_header(Response *this);
#endif
