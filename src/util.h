#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#define BUFFERSIZE  1024

struct tm *local_time();
extern void formated_system_datetime(char *ret, char *format);
extern void last_strtok(char *ret, char *target, char *pattern);
