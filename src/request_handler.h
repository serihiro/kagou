#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./util.h"

static struct tm *local_time();
void formated_system_datetime(char *ret, char *format);
void header(char *ret, int content_length);
