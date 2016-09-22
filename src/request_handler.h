#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "./util.h"
#define HEADER_BUFFER 1024

typedef struct  {
    char key[HEADER_BUFFER];
    char value[HEADER_BUFFER];
} header_value;

void header(char *ret, int content_length);
void load_body(char *ret, char *file_path);
void scan_request_header(header_value *header_values, char *message);
void render_404(char *ret, char *requested_path);
void render_500(char *ret);
void ceate_response(char *request_message, char *response_message, char *root_directory);
