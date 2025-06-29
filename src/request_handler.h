#ifndef HEADER_REQUEST_HANDLER
#define HEADER_REQUEST_HANDLER

#include "request.h"
#include "response.h"
#include "util.h"
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define HTTP_HEADER_BUFFER_SIZE 1024 // Header buffer size
#define FILE_BINARY 0
#define FILE_ASCII 1

extern const char *SERVER_NAME;
extern const char *HEADER_DATE_FORMAT;
extern const char *NOT_FOUND_BODY;
extern const char *UNSUPPORTED_MEDIA;
extern const char *BAD_REQUEST;

typedef struct {
  char *key;
  char *value;
} KEY_VALUE;

typedef struct {
  char *response_status;
  KEY_VALUE *header_values;
  char *body;
} http_response;

typedef struct {
  const char *extension;
  const char *mime_type;
  const int file_type;
} mime_map;

// Returns 1 if connection should be kept alive, 0 if it should be closed
#include <openssl/ssl.h>

extern int respond(char *request_message, char *root_directory,
                   int response_target_fd, SSL *ssl);
char *content_type_from_filename(char *filename);
void generate_text_response(char *file_name, FILE *target_file,
                            Response *response);
#endif
