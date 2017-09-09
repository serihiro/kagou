#include "util.h"
typedef struct {
    char *key;
    char *value;
} REQUSET_KEY_VALUE;

typedef struct {
    char *raw_request;
    REQUSET_KEY_VALUE *request_header_values;
} Request;

#define REQUEST_HEADER_VALUE_SIZE 10
#define RESPONSE_HEADER_VALUE_BUFFER_SIZE 1025 // 1KiB + 1byte
#define ATTRIBUTE_DELIMITER " "

Request *Request_new(char *raw_request);
void Request_delete(Request *this);
void _Request_scan(Request *this);
