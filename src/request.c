#include "request.h"
#include <ctype.h>
#include <strings.h>

const char *ATTRIBUTE_DELIMITER = " ";
const char *HEADER_KEY_METHOD = "method";
const char *HEADER_KEY_PATH = "path";
const char *HEADER_KEY_HTTP_VERSION = "http_version";

// HTTP/1.0 headers
static const char *HTTP_1_0_HEADERS[] = {"Host",
                                         "User-Agent",
                                         "Accept",
                                         "Accept-Language",
                                         "Accept-Encoding",
                                         "Content-Type",
                                         "Content-Length",
                                         "Authorization",
                                         "Connection",
                                         "Referer",
                                         "Cookie",
                                         "Date",
                                         "If-Modified-Since",
                                         "Last-Modified",
                                         "Pragma",
                                         "Cache-Control",
                                         "From",
                                         NULL};

// HTTP/1.1 additional headers (includes all HTTP/1.0 headers)
static const char *HTTP_1_1_HEADERS[] = {
    "Host", "User-Agent", "Accept", "Accept-Language", "Accept-Encoding",
    "Content-Type", "Content-Length", "Authorization", "Connection", "Referer",
    "Cookie", "Date", "If-Modified-Since", "Last-Modified", "Pragma",
    "Cache-Control", "From",
    // HTTP/1.1 specific
    "Transfer-Encoding", "TE", "Trailer", "Upgrade", "Via", "Warning", "Expect",
    "Max-Forwards", "Proxy-Authorization", "Range", "If-Range", "If-Match",
    "If-None-Match", "If-Unmodified-Since", "Age", "ETag", "Location",
    "Proxy-Authenticate", "Retry-After", "Server", "Vary", "WWW-Authenticate",
    "Allow", "Content-Encoding", "Content-Language", "Content-Location",
    "Content-MD5", "Content-Range", "Expires", "Extension-Header", NULL};

// Check if header is valid for the given HTTP version
static int is_valid_header(const char *header_name, const char *http_version) {
  // HTTP/0.9 doesn't support headers
  if (strstr(http_version, "HTTP/0.9") != NULL) {
    return 0;
  }

  // Check header name format (alphanumeric and hyphen only)
  for (const char *p = header_name; *p; p++) {
    if (!isalnum(*p) && *p != '-') {
      return 0;
    }
  }

  const char **valid_headers = NULL;
  if (strstr(http_version, "HTTP/1.0") != NULL) {
    valid_headers = HTTP_1_0_HEADERS;
  } else if (strstr(http_version, "HTTP/1.1") != NULL) {
    valid_headers = HTTP_1_1_HEADERS;
  } else {
    // Unknown version, allow all well-formed headers
    return 1;
  }

  // Case-insensitive comparison
  for (int i = 0; valid_headers[i] != NULL; i++) {
    if (strcasecmp(header_name, valid_headers[i]) == 0) {
      return 1;
    }
  }

  return 0;
}

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
  int header_index = 0;
  char *current_line = this->raw_request;
  char *http_version = NULL;

  // Process request line first
  char *line_end = strstr(current_line, "\r\n");
  if (line_end == NULL) {
    return;
  }

  // Copy the request line
  size_t line_len = line_end - current_line;
  char *request_line = (char *)calloc(line_len + 1, sizeof(char));
  strncpy(request_line, current_line, line_len);

  // Parse request line by finding spaces
  char *method_start = request_line;
  char *method_end = strchr(method_start, ' ');
  if (method_end == NULL) {
    // Malformed request - only method provided
    strcpy(this->request_header_values[header_index].key, HEADER_KEY_METHOD);
    strcpy(this->request_header_values[header_index].value, method_start);
    header_index++;

    strcpy(this->request_header_values[header_index].key, HEADER_KEY_PATH);
    strcpy(this->request_header_values[header_index].value,
           "/"); // Default path
    header_index++;

    strcpy(this->request_header_values[header_index].key,
           HEADER_KEY_HTTP_VERSION);
    strcpy(this->request_header_values[header_index].value,
           "HTTP/0.9"); // Assume HTTP/0.9

    free(request_line);
    this->keep_alive = 0;
    return;
  }
  *method_end = '\0';

  char *path_start = method_end + 1;
  char *path_end = strchr(path_start, ' ');
  if (path_end == NULL) {
    // HTTP/0.9 style request (no version)
    strcpy(this->request_header_values[header_index].key, HEADER_KEY_METHOD);
    strcpy(this->request_header_values[header_index].value, method_start);
    header_index++;

    strcpy(this->request_header_values[header_index].key, HEADER_KEY_PATH);
    strcpy(this->request_header_values[header_index].value, path_start);
    header_index++;

    strcpy(this->request_header_values[header_index].key,
           HEADER_KEY_HTTP_VERSION);
    strcpy(this->request_header_values[header_index].value, "HTTP/0.9");
    header_index++;

    http_version = "HTTP/0.9";
  } else {
    *path_end = '\0';
    char *version_start = path_end + 1;

    strcpy(this->request_header_values[header_index].key, HEADER_KEY_METHOD);
    strcpy(this->request_header_values[header_index].value, method_start);
    header_index++;

    strcpy(this->request_header_values[header_index].key, HEADER_KEY_PATH);
    strcpy(this->request_header_values[header_index].value, path_start);
    header_index++;

    strcpy(this->request_header_values[header_index].key,
           HEADER_KEY_HTTP_VERSION);
    strcpy(this->request_header_values[header_index].value, version_start);
    header_index++;

    http_version = this->request_header_values[2].value;
  }

  free(request_line);

  // Default keep-alive behavior
  if (strstr(http_version, "HTTP/1.1") != NULL) {
    this->keep_alive = 1;
  } else {
    this->keep_alive = 0;
  }

  // Move to headers section
  current_line = line_end + 2; // Skip \r\n

  // Parse headers
  while (*current_line != '\0' && header_index < REQUEST_HEADER_ITEM_MAX_SIZE) {
    line_end = strstr(current_line, "\r\n");
    if (line_end == NULL) {
      break;
    }

    // Empty line indicates end of headers
    if (line_end == current_line) {
      break;
    }

    // Copy the header line
    line_len = line_end - current_line;
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

      // Validate header based on HTTP version
      if (is_valid_header(key, http_version)) {
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
      // Invalid headers are silently skipped
    }

    free(header_line);
    current_line = line_end + 2; // Move to next line
  }
}
