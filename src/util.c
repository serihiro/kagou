#include "./util.h"

#define TIME_FORMAT_BUFFER_SIZE 128 // Buffer size for formatted time string

struct tm *local_time() {
  struct tm *t_st;
  time_t tt;

  time(&tt);
  t_st = localtime(&tt);

  return t_st;
}

extern void formated_system_datetime(char *ret, const char *format) {
  struct tm *t_st;
  char buf[TIME_FORMAT_BUFFER_SIZE];

  t_st = local_time();
  strftime(buf, sizeof(buf), format, t_st);
  strcpy(ret, buf);
}

extern void last_strtok(char *ret, char *target, const char *pattern) {
  if (strstr(target, pattern) == NULL) {
    strcpy(ret, "");
    return;
  }

  char *tokenized = strtok(target, pattern);
  char *last_tokenized = tokenized;
  while ((tokenized = strtok(NULL, pattern)) != NULL) {
    last_tokenized = tokenized;
  }

  strcpy(ret, last_tokenized ? last_tokenized : "");
}
