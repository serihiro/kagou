#include <time.h>
#include <string.h>

#define BUFFERSIZE  1024

struct tm *local_time();
extern void formated_system_datetime(char *ret, char *format);
extern void last_strtok(char *ret, char *target, char *pattern);
