#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "./util.h"
#define HEADER_BUFFER 1024
#define SERVER_NAME "kagou"

typedef struct  {
    char *key;
    char *value;
} header_value;

typedef struct {
    char * response_status;
    header_value *header_values;
    char *body;
} http_response;

void load_text_file(char *ret, FILE *target_file);
void scan_request_header(char *message, header_value *request_header_values);
void render_404(char *ret);
void render_415(char *ret);
void render_500(char *ret);
void create_html_message(char *ret, http_response response);
void create_response(char *request_message, char *response_message, char *root_directory);
