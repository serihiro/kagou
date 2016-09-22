#include "./util.h"

struct tm *local_time() {
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
