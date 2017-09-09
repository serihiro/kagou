#include "request.h"

Request* Request_new(char *raw_request) {
    Request *this = (Request *)malloc(sizeof(Request));
    this->raw_request = (char *)malloc(strlen(raw_request) + 1);
    strcpy(this->raw_request, raw_request);

    this->request_header_values = (REQUSET_KEY_VALUE *)malloc(sizeof(REQUSET_KEY_VALUE) * REQUEST_HEADER_VALUE_SIZE);
    for(int i = 0; i < REQUEST_HEADER_VALUE_SIZE; i++){
        this->request_header_values[i].key = (char *)malloc(RESPONSE_HEADER_VALUE_BUFFER_SIZE);
        this->request_header_values[i].value = (char *)malloc(RESPONSE_HEADER_VALUE_BUFFER_SIZE);
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
    char cpy_message[strlen(this->raw_request)];
    memset(&cpy_message, 0, sizeof(cpy_message));
    strcpy(cpy_message, this->raw_request);

    strcpy(this->request_header_values[0].key, "method");
    strcpy(this->request_header_values[0].value, strtok(cpy_message, ATTRIBUTE_DELIMITER));
    strcpy(this->request_header_values[1].key, "path");
    strcpy(this->request_header_values[1].value, strtok(NULL, ATTRIBUTE_DELIMITER));
    strcpy(this->request_header_values[2].key, "http_version");
    strcpy(this->request_header_values[2].value, strtok(NULL, ATTRIBUTE_DELIMITER));
}
