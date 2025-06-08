#include "response.h"

Response *Response_new() {
  Response *this = (Response *)calloc(1, sizeof(Response));

  this->header_values =
      (Tuple *)calloc(RESPONSE_HEADER_VALUE_SIZE, sizeof(Tuple));
  for (int i = 0; i < RESPONSE_HEADER_VALUE_SIZE; i++) {
    this->header_values[i].key =
        (char *)calloc(RESPONSE_HEADER_ITEM_STRING_LENGTH, sizeof(char));
    this->header_values[i].value =
        (char *)calloc(RESPONSE_HEADER_ITEM_STRING_LENGTH, sizeof(char));
  }
  return this;
}

void Response_set_header(Response *this, const char *response_header) {
  this->header = (char *)calloc(strlen(response_header) + 1, sizeof(char));
  strcpy(this->header, response_header);
}

void Response_set_body_as_text(Response *this, const char *response_body) {
  this->body = (char *)calloc(strlen(response_body) + 1, sizeof(char));
  strcpy(this->body, response_body);
}

void Response_set_status(Response *this, const char *response_status) {
  this->status = (char *)calloc(strlen(response_status) + 1, sizeof(char));
  strcpy(this->status, response_status);
}

void Response_create_header(Response *this) {
  char *tmp_header =
      (char *)calloc(RESPONSE_HEADER_BUFFER_STRING_LENGTH, sizeof(char));

  strcat(tmp_header, this->status);
  strcat(tmp_header, HEADER_LINE_BREAK_CODE);

  for (int i = 0; i < RESPONSE_HEADER_VALUE_SIZE; i++) {
    if (this->header_values[i].key == NULL)
      break;
    strcat(tmp_header, this->header_values[i].key);
    strcat(tmp_header, HEADER_SEPARATOR);
    strcat(tmp_header, this->header_values[i].value);
    strcat(tmp_header, HEADER_LINE_BREAK_CODE);
  }
  strcat(tmp_header, HEADER_LINE_BREAK_CODE); // separator of header and body
  Response_set_header(this, tmp_header);
  free(tmp_header);
}

void Response_delete(Response *this) {
  free(this->header);
  free(this->body);
  free(this->status);
  if (this->header_values != NULL) {
    for (int i = 0; i < RESPONSE_HEADER_VALUE_SIZE; i++) {
      if (this->header_values[i].key == NULL)
        break;
      free(this->header_values[i].key);
      if (this->header_values[i].value != NULL)
        free(this->header_values[i].value);
    }
    free(this->header_values);
  }
  free(this);
}
