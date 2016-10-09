#include "request.h"

Request* Request_new(char *raw_request) {
    Request *this = (Request *)malloc(sizeof(Request));
    this->raw_request = (char *)malloc(strlen(raw_request));
    strcpy(this->raw_request, raw_request);

    this->request_header_values = (REQUSET_KEY_VALUE *)malloc(sizeof(REQUSET_KEY_VALUE) * REQUEST_HEADER_VALUE_SIZE);
    for(int i = 0; i < REQUEST_HEADER_VALUE_SIZE; i++){
        this->request_header_values[i].key = NULL;
        this->request_header_values[i].value = NULL;
    }

    _Request_scan(this);

    return this;
}

void Request_delete(Request *this){
    free(this->raw_request);
    free(this->request_header_values);
    free(this);
}

void _Request_scan(Request *this) {
    char *token;

    char cpy_message[strlen(this->raw_request)];
    memset(&cpy_message, 0, sizeof(cpy_message));
    strcpy(cpy_message, this->raw_request);

    token = strtok(cpy_message, ATTRIBUTE_DELIMITER);
    this->request_header_values[0].key = "method";
    this->request_header_values[0].value = token;
    token = strtok(NULL, ATTRIBUTE_DELIMITER);
    this->request_header_values[1].key = "path";
    this->request_header_values[1].value = token;
    token = strtok(NULL, ATTRIBUTE_DELIMITER);
    this->request_header_values[2].key = "http_version";
    this->request_header_values[2].value = token;
}
