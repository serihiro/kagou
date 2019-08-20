#include "./util.h"

struct tm *local_time() {
  struct tm *t_st;
  time_t tt;

  time(&tt);
  t_st = localtime(&tt);

  return t_st;
}

extern void formated_system_datetime(char *ret, const char *format) {
  struct tm *t_st;
  char buf[128];

  t_st = local_time();
  strftime(buf, sizeof(buf), format, t_st);
  strcpy(ret, buf);
}

extern void last_strtok(char *ret, char *target, const char *pattern) {
  if (strstr(target, pattern) == NULL) {
    strcpy(ret, "");
    return;
  }

  char *last_tokenized;
  char *tokenized = strtok(target, pattern);
  while (1) {
    tokenized = strtok(NULL, pattern);
    if (tokenized == NULL) {
      break;
    } else {
      last_tokenized = tokenized;
    }
  }

  strcpy(ret, last_tokenized);
}
