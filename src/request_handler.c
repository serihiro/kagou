#include "./request_handler.h"

static struct tm *local_time() {
    struct tm *t_st;
    time_t tt;

    time(&tt);
    t_st = localtime(&tt);

    return t_st;
}

void formated_system_datetime(char *ret, char *format) {
    struct tm *t_st;
    char buf[128];

    t_st = local_time();
    strftime(buf, sizeof(buf), format, t_st);
    strcpy(ret, buf);
}

void header(char *ret, int content_length){
    struct tm *t_st;
    time_t tt;

    time(&tt);
    t_st = localtime(&tt);
    char systime[128];
    formated_system_datetime(systime, "%a, %d %b %Y %H:%M:%S GMT");

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
