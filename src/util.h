#ifndef HEADER_UTIL
#define HEADER_UTIL

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define BUFFERSIZE 1024

struct tm *local_time();
extern void formated_system_datetime(char *ret, const char *format);
extern void last_strtok(char *ret, char *target, const char *pattern);

typedef struct {
  char *key;
  char *value;
} Tuple;
#endif