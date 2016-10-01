#include "./request_handler.h"
#define HEADER_DATE_FORMAT "%a, %d %b %Y %H:%M:%S GMT"
#define HEADER_SEPARATOR ": "
#define HEADER_LINE_BREAK_CODE "\r\n"
#define ROW_BUFFER 4096
#define HEADER_VALUE_SIZE 10

void cleanup(header_value *request_header_values,
            header_value *response_header_values,
            FILE *target_file){
    if(request_header_values != NULL){
        free(request_header_values);
    }
    if(response_header_values != NULL){
        free(response_header_values);
    }
    if(target_file != NULL){
        fclose(target_file);
    }
}

void load_text_file(char *ret, FILE *target_file) {
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

     strcpy(ret, fbuf);
}

void scan_request_header(char *message, header_value *request_header_values){
    char *token;
    char *attribute_delimter = " ";
    char *row_delimter = "\n";

    char cpy_message[strlen(message)];
    memset(&cpy_message, 0, sizeof(cpy_message));
    strcpy(cpy_message, message);

    token = strtok(cpy_message, attribute_delimter);
    request_header_values[0].key = "method";
    request_header_values[0].value = token;
    token = strtok(NULL, attribute_delimter);
    request_header_values[1].key = "path";
    request_header_values[1].value = token;
    token = strtok(NULL, attribute_delimter);
    request_header_values[2].key = "http_version";
    request_header_values[2].value = token;

    token = strtok(message, row_delimter);
    while(1) {
        token = strtok(NULL, row_delimter);
        if(!token)
            break;

        char cpy_row[strlen(token)];
        token = strtok(cpy_row, attribute_delimter);
    }
}

void render_415(char *ret){
    sprintf(ret, "<html><head> \
                  <title>415 Unsupported Media Type</title> \
                  </head><body> \
                  <h1>415 Unsupported Media Type</h1> \
                  </body></html>");
}

void render_404(char *ret){
    sprintf(ret, "<html><head> \
                  <title>404 Not Found</title> \
                  </head><body> \
                  <h1>Not Found</h1> \
                  <p>The requested URL is Not Found</p> \
                  </body></html>");
}

void render_500(char *ret){
    sprintf(ret, "<html><head> \
                  <title>500 Server Internal Error</title> \
                  </head><body> \
                  <h1>erver Internal Error</h1> \
                  <p>Sorry!</p> \
                  </body></html>");
}

void create_html_message(char *ret, http_response response) {
    strcat(ret, response.response_status);
    strcat(ret, HEADER_LINE_BREAK_CODE);

    for(int i = 0; i < (int)sizeof(response.header_values); i++){
        if(response.header_values[i].key == NULL)
            break;
        strcat(ret, response.header_values[i].key);
        strcat(ret, HEADER_SEPARATOR);
        strcat(ret, response.header_values[i].value);
        strcat(ret, HEADER_LINE_BREAK_CODE);
    }
    strcat(ret, HEADER_LINE_BREAK_CODE);
    strcat(ret, response.body);
}

const mime_map MIME_TYPES[] = {
    {".htm", "text/html", FILE_ASCII},
    {".html", "text/html", FILE_ASCII},
    {".css", "text/css", FILE_ASCII},
    {".js", "application/javascript", FILE_ASCII},
    {".gif", "image/gif", FILE_BINARY},
    {".jpeg", "image/jpeg", FILE_BINARY},
    {".jpg", "image/jpeg", FILE_BINARY},
    {".ico", "image/x-icon", FILE_BINARY},
    {".pdf", "application/pdf", FILE_BINARY},
    {".mp4", "video/mp4", FILE_BINARY},
    {".png", "image/png", FILE_BINARY},
    {".svg", "image/svg+xml", FILE_BINARY},
    {".xml", "text/xml", FILE_BINARY},
    {NULL, NULL, FILE_ASCII},
};

char* content_type_from_filename(char* filename){
    char *dot = strrchr(filename, '.');
    mime_map *map = (mime_map *)MIME_TYPES;
    if(dot){
        while(map->extension){
            if(strcmp(map->extension, dot) == 0){
                return (char *)map->mime_type;
            }
            map++;
        }
    }

    return "text/plain";
}

void create_response(char *request_message, char *response_message, char *root_directory){
    header_value *request_header_values = NULL;
    header_value *response_header_values = NULL;
    FILE *target_file = NULL;

    request_header_values = (header_value *)malloc(sizeof(header_value) * HEADER_VALUE_SIZE);
    response_header_values = (header_value *)malloc(sizeof(header_value) * HEADER_VALUE_SIZE);
    memset(request_header_values, 0, sizeof(*request_header_values));
    memset(response_header_values, 0, sizeof(*response_header_values));
    for(int i = 0; i < HEADER_VALUE_SIZE; i++){
        request_header_values[i].key = NULL;
        request_header_values[i].value = NULL;
        response_header_values[i].key = NULL;
        response_header_values[i].value = NULL;
    }

    scan_request_header(request_message, request_header_values);
    char full_path[PATH_MAX + 1];
    strcpy(full_path, root_directory);
    char tmp_path[BUFFERSIZE];
    strcpy(tmp_path, request_header_values[1].value);
    strcat(full_path, tmp_path);

    char resolved_path[PATH_MAX + 1];
    realpath(full_path, resolved_path);

    char body[1024 * 500];
    char html_message[1024 * 1000];
    memset(&body, 0, sizeof(body));
    memset(&html_message, 0, sizeof(html_message));

    http_response response;
    memset(&response, 0, sizeof(response));

    char systime[128];
    formated_system_datetime(systime, HEADER_DATE_FORMAT);

    response_header_values[0].key = "Date";
    response_header_values[0].value = systime;
    response_header_values[1].key = "Server";
    response_header_values[1].value = SERVER_NAME;
    response_header_values[2].key = "Connection";
    response_header_values[2].value = "close"; // FIXME toriisogi

    struct stat st;
    int stat_result;
    stat_result = stat(resolved_path, &st);
    // file exists and regular file
    if (stat_result != 0 || (st.st_mode & S_IFMT) != S_IFREG){
        render_404(body);
        response_header_values[3].key =  "Content-type";
        response_header_values[3].value = "text/html";
        response_header_values[4].key = "Content-length";
        char length[100];
        sprintf(length, "%ld", strlen(body));
        response_header_values[4].value = length;

        response.response_status =  "HTTP/1.1 404 Not Found";
        response.header_values = response_header_values;
        response.body = body;
        create_html_message(response_message, response);
        cleanup(request_header_values, response_header_values, target_file);
        return;
    }

    target_file = fopen(resolved_path, "r");
    if (target_file == NULL) {
        perror("Failed to fopen target file");
        render_500(body);
        response_header_values[3].key = "Content-type";
        response_header_values[3].value = "text/html";
        response_header_values[4].key = "Content-length";
        char length[100];
        sprintf(length, "%ld", strlen(body));
        response_header_values[4].value = length;

        response.response_status =  "HTTP/1.1 500 Internal Server Error";
        response.header_values = response_header_values;
        response.body = body;
        create_html_message(response_message, response);
        cleanup(request_header_values, response_header_values, target_file);
        return;
     }

    char file_name[PATH_MAX + 1];
    char copied_resolved_path[PATH_MAX + 1];
    strcpy(copied_resolved_path, resolved_path);
    last_strtok(file_name, copied_resolved_path, "/");

    // TODO functionize
    if(strstr(file_name, ".html") != NULL || strstr(file_name, ".htm") != NULL) {
        load_text_file(body, target_file);
        response_header_values[3].key = "Content-type";
        response_header_values[3].value = content_type_from_filename(file_name);
        response_header_values[4].key = "Content-length";
        char length[100];
        sprintf(length, "%ld", strlen(body));
        response_header_values[4].value = length;

        response.response_status =  "HTTP/1.1 200 OK";
        response.header_values = response_header_values;
        response.body = body;
    } else if(strstr(file_name, ".js") != NULL) {
        load_text_file(body, target_file);
        response_header_values[3].key = "Content-type";
        response_header_values[3].value = content_type_from_filename(file_name);
        response_header_values[4].key = "Content-length";
        char length[100];
        sprintf(length, "%ld", strlen(body));
        response_header_values[4].value = length;

        response.response_status =  "HTTP/1.1 200 OK";
        response.header_values = response_header_values;
        response.body = body;
    } else {
        render_415(body);
        response_header_values[3].key = "Content-type";
        response_header_values[3].value = "text/html";
        response_header_values[4].key = "Content-length";
        char length[100];
        sprintf(length, "%ld", strlen(body));
        response_header_values[4].value = length;

        response.response_status =  "HTTP/1.1 415 Unsupported Media Type";
        response.header_values = response_header_values;
        response.body = body;
    }
    create_html_message(response_message, response);
    cleanup(request_header_values, response_header_values, target_file);
}
