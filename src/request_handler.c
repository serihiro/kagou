#include "./request_handler.h"
#define HEADER_DATE_FORMAT "%a, %d %b %Y %H:%M:%S GMT"

void header(char *ret, int content_length){
    struct tm *t_st;
    time_t tt;

    time(&tt);
    t_st = localtime(&tt);
    char systime[128];
    formated_system_datetime(systime, HEADER_DATE_FORMAT);

    char header_buffer[1024] = "HTTP/1.1 200 OK\nDate: ";
    strcat(header_buffer, systime);
    strcat(header_buffer, "\n");
    strcat(header_buffer, "Server: kagou\n");
    strcat(header_buffer, "Connection: close\n");
    strcat(header_buffer, "Content-type: text/html\n");
    char content_length_content[16 + (int)ceil(log10(content_length))];
    sprintf(content_length_content, "Content-length: %d\n", content_length);
    strcat(header_buffer, content_length_content);

    strcpy(ret, header_buffer);
}

#define ROW_BUFFER 4096

void load_body(char *ret, char *file_path) {
    struct stat st;
    int stat_result;
    char resolved_path[PATH_MAX + 1];
    realpath(file_path, resolved_path);

    stat_result = stat(resolved_path, &st);
    // file exists and regular file
    if (stat_result != 0 || (st.st_mode & S_IFMT) != S_IFREG){
        render_404(ret, file_path);
        return;
    }

    FILE *target_file;
    target_file = fopen(resolved_path, "r");
    if (target_file == NULL) {
        perror("Failed to fopen target file");
        render_500(ret);
        return;
     }

     fpos_t fsize = 0;
     fseek(target_file, 0, SEEK_END);
     fgetpos(target_file, &fsize);
     rewind(target_file);

     char fbuf[fsize + 1];
     char rbuf[ROW_BUFFER];
     memset(&fbuf, 0, sizeof(fbuf));
     memset(&rbuf, 0, sizeof(rbuf));
     // 1行ごとの長さ取ってbuffer作ってstrcatでくっつける
     while(fgets(rbuf, fsize, target_file) != NULL){
         strcat(fbuf, rbuf);
     }
     fclose(target_file);

     strcpy(ret, fbuf);
}

void scan_request_header(header_value *header_values, char *message){
    char *token;
    char *attribute_delimter = " ";
    char *row_delimter = "\n";

    char cpy_message[strlen(message)];
    strcpy(cpy_message, message);

    token = strtok(cpy_message, attribute_delimter);
    strcpy(header_values[0].key, "method");
    strcpy(header_values[0].value, token);
    token = strtok(NULL, attribute_delimter);
    strcpy(header_values[1].key, "path");
    strcpy(header_values[1].value, token);
    token = strtok(NULL, attribute_delimter);
    strcpy(header_values[2].key, "http_version");
    strcpy(header_values[2].value, token);

    token = strtok(message, row_delimter);
    while(1) {
        token = strtok(NULL, row_delimter);
        if(!token)
            break;

        char cpy_row[strlen(token)];
        token = strtok(cpy_row, attribute_delimter);
    }
}

void render_404(char *ret, char *requested_path){
    sprintf(ret, "<html><head> \
                  <title>404 Not Found</title> \
                  </head><body> \
                  <h1>Not Found</h1> \
                  <p>The requested URL %s</p> \
                  </body></html>", requested_path);
}

void render_500(char *ret){
    sprintf(ret, "<html><head> \
                  <title>500 Server Internal Error</title> \
                  </head><body> \
                  <h1>erver Internal Error</h1> \
                  <p>Sorry!</p> \
                  </body></html>");
}

void ceate_response(char *request_message, char *response_message, char *root_directory){
    header_value *header_values = (header_value *)malloc(sizeof(header_value) * 10);
    scan_request_header(header_values, request_message);
    // If using malloc, somehow this cannot send response..

    char body[1024 * 500];
    char full_path[BUFFERSIZE];

    strcpy(full_path, root_directory);
    strcat(full_path, header_values[1].value);
    load_body(body, full_path);
    header(response_message, strlen(body));
    strcat(response_message, "\r\n");
    strcat(response_message, body);

    free(header_values);
}
