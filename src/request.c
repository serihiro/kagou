#include "request.h"
#include <strings.h>

const char *ATTRIBUTE_DELIMITER = " ";
const char *HEADER_KEY_METHOD = "method";
const char *HEADER_KEY_PATH = "path";
const char *HEADER_KEY_HTTP_VERSION = "http_version";

Request *Request_new(char *raw_request) {
  Request *this = (Request *)calloc(1, sizeof(Request));
  this->raw_request = (char *)calloc(strlen(raw_request) + 1, sizeof(char));
  strcpy(this->raw_request, raw_request);

  this->request_header_values =
      (Tuple *)calloc(REQUEST_HEADER_ITEM_MAX_SIZE, sizeof(Tuple));
  for (int i = 0; i < REQUEST_HEADER_ITEM_MAX_SIZE; i++) {
    this->request_header_values[i].key =
        (char *)calloc(REQUEST_HEADER_ITEM_STRING_LENGTH, sizeof(char));
    this->request_header_values[i].value =
        (char *)calloc(REQUEST_HEADER_ITEM_STRING_LENGTH, sizeof(char));
  }

  _Request_scan(this);

  return this;
}

void Request_delete(Request *this) {
  free(this->raw_request);
  if (this->request_header_values != NULL) {
    for (int i = 0; i < REQUEST_HEADER_ITEM_MAX_SIZE; i++) {
      free(this->request_header_values[i].key);
      free(this->request_header_values[i].value);
    }
    free(this->request_header_values);
  }
  free(this);
}

void _Request_scan(Request *this) {

  // Find the end of the request line
  char *request_end = strstr(this->raw_request, "\r\n");
  if (request_end == NULL) {
    return;
  }

  // Copy and parse the request line
  size_t request_line_len = request_end - this->raw_request;
  char *request_line = (char *)calloc(request_line_len + 1, sizeof(char));
  strncpy(request_line, this->raw_request, request_line_len);

  strcpy(this->request_header_values[0].key, HEADER_KEY_METHOD);
  strcpy(this->request_header_values[0].value,
         strtok(request_line, ATTRIBUTE_DELIMITER));
  strcpy(this->request_header_values[1].key, HEADER_KEY_PATH);
  strcpy(this->request_header_values[1].value,
         strtok(NULL, ATTRIBUTE_DELIMITER));
  strcpy(this->request_header_values[2].key, HEADER_KEY_HTTP_VERSION);
  char *http_version = strtok(NULL, ATTRIBUTE_DELIMITER);
  if (http_version != NULL) {
    strcpy(this->request_header_values[2].value, http_version);
  }
  free(request_line);

  // Default keep-alive behavior: HTTP/1.1 defaults to keep-alive, HTTP/1.0
  // defaults to close
  if (strstr(this->request_header_values[2].value, "HTTP/1.1") != NULL) {
    this->keep_alive = 1;
  } else {
    this->keep_alive = 0;
  }

  // Parse headers
  int header_index = 3;
  char *headers_start = request_end + 2; // Skip \r\n
  char *current_line = headers_start;

  while (*current_line != '\0' && header_index < REQUEST_HEADER_ITEM_MAX_SIZE) {
    char *line_end = strstr(current_line, "\r\n");
    if (line_end == NULL) {
      break;
    }

    // Empty line indicates end of headers
    if (line_end == current_line) {
      break;
    }

    // Copy the header line
    size_t line_len = line_end - current_line;
    char *header_line = (char *)calloc(line_len + 1, sizeof(char));
    strncpy(header_line, current_line, line_len);

    char *colon = strchr(header_line, ':');
    if (colon != NULL) {
      *colon = '\0';
      char *key = header_line;
      char *value = colon + 1;

      // Skip leading whitespace in value
      while (*value == ' ' || *value == '\t') {
        value++;
      }

      strcpy(this->request_header_values[header_index].key, key);
      strcpy(this->request_header_values[header_index].value, value);

      // Check for Connection header
      if (strcasecmp(key, "Connection") == 0) {
        if (strcasecmp(value, "close") == 0) {
          this->keep_alive = 0;
        } else if (strcasecmp(value, "keep-alive") == 0) {
          this->keep_alive = 1;
        }
      }

      header_index++;
    }

    free(header_line);
    current_line = line_end + 2; // Move to next line
  }
}
