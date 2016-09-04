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
