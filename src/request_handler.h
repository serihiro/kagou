#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "./util.h"

static struct tm *local_time();
void formated_system_datetime(char *ret, char *format);
void header(char *ret, int content_length);
void load_body(char *ret, char *file_path);
