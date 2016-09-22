#include <time.h>
#include <string.h>

#define BUFFERSIZE  1024

struct tm *local_time();
void formated_system_datetime(char *ret, char *format);
