#include "response.h"

Response *Response_new(){
    Response *this = (Response *)malloc(sizeof(Response));

    this->header = NULL;
    this->body = NULL;
    this->status = NULL;
    this->header_values = (REPONSE_KEY_VALUE *)malloc(sizeof(REPONSE_KEY_VALUE) * RESPONSE_HEADER_VALUE_SIZE);
    memset(this->header_values, 0, sizeof(*this->header_values));
    for(int i = 0; i < RESPONSE_HEADER_VALUE_SIZE; i++){
        this->header_values[i].key = NULL;
        this->header_values[i].value = NULL;
    }
    return this;
}

void Response_set_header(Response *this, char *response_header) {
    this->header = (char *)malloc(strlen(response_header));
    this->header = (char *)realloc(this->header, strlen(response_header));

    memset(this->header, 0, strlen(response_header));
    strcpy(this->header, response_header);
}

void Response_set_body_as_text(Response *this, char *response_body) {
    this->body = (char *)realloc(this->body, strlen(response_body) * 4);

    memset(this->body, 0, strlen(response_body) * 4);
    strcpy(this->body, response_body);
}

void Response_set_status(Response *this, char *response_status) {
    this->status = (char *)realloc(this->status, strlen(response_status));

    memset(this->status, 0, strlen(response_status));
    strcpy(this->status, response_status);
}

void Response_create_header(Response *this) {
    char *tmp_header = (char *)malloc(RESPONSE_HEADER_BUFFER_SIZE);
    memset(tmp_header, 0, RESPONSE_HEADER_BUFFER_SIZE);

    strcat(tmp_header, this->status);
    strcat(tmp_header, HEADER_LINE_BREAK_CODE);

    for(int i = 0; i < (int)sizeof(this->header_values); i++){
        if(this->header_values[i].key == NULL)
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

void Response_delete(Response *this){
    free(this->header);
    free(this->body);
    free(this->status);
    free(this->header_values);
    free(this);
}
