#include "util.h"
#include "request.h"
#include "response.h"
#include <sys/socket.h>
#include <unistd.h>

#define HEADER_BUFFER 1024
#define SERVER_NAME "kagou"
#define FILE_BINARY 0
#define FILE_ASCII 1
#define HEADER_DATE_FORMAT "%a, %d %b %Y %H:%M:%S GMT"

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

void render_404(char *ret);
void render_415(char *ret);
void render_500(char *ret);
extern int respond(char *request_message, char *root_directory, int response_target_fd);
char* content_type_from_filename(char* filename);
void generate_text_response(char *file_name, FILE *target_file, Response *response);
