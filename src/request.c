#include "request.h"

Request *Request_new(char *raw_request) {
  Request *this = (Request *)calloc(1, sizeof(Request));
  this->raw_request = (char *)calloc(strlen(raw_request) + 1, sizeof(char));
  strcpy(this->raw_request, raw_request);

  this->request_header_values =
      (Tuple *)calloc(REQUEST_HEADER_VALUE_SIZE, sizeof(Tuple));
  for (int i = 0; i < REQUEST_HEADER_VALUE_SIZE; i++) {
    this->request_header_values[i].key =
        (char *)calloc(RESPONSE_HEADER_VALUE_BUFFER_SIZE, sizeof(char));
    this->request_header_values[i].value =
        (char *)calloc(RESPONSE_HEADER_VALUE_BUFFER_SIZE, sizeof(char));
  }

  _Request_scan(this);

  return this;
}

void Request_delete(Request *this) {
  free(this->raw_request);
  free(this->request_header_values);
  free(this);
}

void _Request_scan(Request *this) {
  char *cpy_message =
      (char *)calloc(strlen(this->raw_request) + 1, sizeof(char));
  strcpy(cpy_message, this->raw_request);

  strcpy(this->request_header_values[0].key, HEADER_KEY_METHOD);
  strcpy(this->request_header_values[0].value,
         strtok(cpy_message, ATTRIBUTE_DELIMITER));
  strcpy(this->request_header_values[1].key, HEADER_KEY_PATH);
  strcpy(this->request_header_values[1].value,
         strtok(NULL, ATTRIBUTE_DELIMITER));
  strcpy(this->request_header_values[2].key, HEADER_KEY_HTTP_VERSION);
  strcpy(this->request_header_values[2].value,
         strtok(NULL, ATTRIBUTE_DELIMITER));
}
