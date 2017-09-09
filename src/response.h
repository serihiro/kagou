#include "util.h"

typedef struct {
    char *key;
    char *value;
} REPONSE_KEY_VALUE;

typedef struct {
    char *header;
    char *body;
    char *status;
    REPONSE_KEY_VALUE *header_values;
} Response;

#define RESPONSE_HEADER_VALUE_SIZE 10
#define RESPONSE_HEADER_VALUE_BUFFER_SIZE 1025 // 1KiB + 1byte
#define RESPONSE_HEADER_BUFFER_SIZE 16385 // 16 KiB + 1byte
#define HEADER_SEPARATOR ": "
#define HEADER_LINE_BREAK_CODE "\r\n"

Response *Response_new();
void Response_delete(Response *this);
void Response_set_header(Response *this, char *header);
void Response_set_body_as_text(Response *this, char *body);
void Response_set_status(Response *this, char *status);
void Response_create_header(Response *this);
