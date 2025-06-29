#include "request_handler.h"
#include <sys/stat.h>

#define PATH_BUFFER_SIZE (MAXPATHLEN + 1)     // Path buffer allocation size
#define TIME_STRING_BUFFER_SIZE (128 + 1)     // System time string buffer size
#define FILENAME_BUFFER_SIZE (MAXPATHLEN + 1) // File name buffer size
#define RESPONSE_HEADER_CONTENT_TYPE_INDEX 3  // Index for Content-Type header
#define RESPONSE_HEADER_CONTENT_LENGTH_INDEX                                   \
  4 // Index for Content-Length header

static int connection_write(int fd, SSL *ssl, const void *buf, int len) {
  if (ssl)
    return SSL_write(ssl, buf, len);
  return send(fd, buf, len, 0);
}

static int connection_close(int fd, SSL *ssl) {
  if (ssl) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
  }
  return close(fd);
}

const char *SERVER_NAME = "kagou";
const char *HEADER_DATE_FORMAT = "%a, %d %b %Y %H:%M:%S GMT";

const char *NOT_FOUND_BODY = "<html><head>\
<title>404 Not Found</title>\
</head><body>\
<h1>Not Found</h1>\
<p>The requested URL is Not Found</p>\
</body></html>";

const char *UNSUPPORTED_MEDIA = "<html><head>\
<title>415 Unsupported Media Type</title>\
</head><body>\
<h1>415 Unsupported Media Type</h1>\
</body></html>";

const char *BAD_REQUEST = "<html><head>\
<title>500 Server Internal Error</title>\
</head><body>\
<h1>erver Internal Error</h1>\
<p>Sorry!</p>\
</body></html>";

void cleanup(Request *request, Response *response, FILE *target_file) {
  if (request != NULL) {
    Request_delete(request);
  }
  if (response != NULL) {
    Response_delete(response);
  }
  if (target_file != NULL) {
    fclose(target_file);
  }
}

void load_text_file(Response *response, FILE *target_file) {
  fseek(target_file, 0, SEEK_END);
  long fsize = ftell(target_file);
  rewind(target_file);

  char *fbuf = (char *)calloc(fsize + 1, sizeof(char));

  size_t read_size = fread(fbuf, 1, fsize, target_file);
  if (read_size != (size_t)fsize) {
    fprintf(stderr, "Warning: fread read %zu bytes, expected %ld bytes\n",
            read_size, fsize);
  }
  Response_set_body_as_text(response, fbuf);
  free(fbuf);
}

const mime_map MIME_TYPES[] = {
    {".htm", "text/html", FILE_ASCII},
    {".html", "text/html", FILE_ASCII},
    {".css", "text/css", FILE_ASCII},
    {".js", "application/javascript", FILE_ASCII},
    {".csv", "text/csv", FILE_ASCII},
    {".json", "application/json", FILE_ASCII},
    {".gif", "image/gif", FILE_BINARY},
    {".jpeg", "image/jpeg", FILE_BINARY},
    {".jpg", "image/jpeg", FILE_BINARY},
    {".ico", "image/x-icon", FILE_BINARY},
    {".pdf", "application/pdf", FILE_BINARY},
    {".mp4", "video/mp4", FILE_BINARY},
    {".png", "image/png", FILE_BINARY},
    {".svg", "image/svg+xml", FILE_BINARY},
    {".xml", "text/xml", FILE_BINARY},
    {NULL, NULL, FILE_ASCII},
};

char *content_type_from_filename(char *filename) {
  char *dot = strrchr(filename, '.');
  mime_map *map = (mime_map *)MIME_TYPES;
  if (dot) {
    while (map->extension) {
      if (strcmp(map->extension, dot) == 0) {
        return (char *)map->mime_type;
      }
      map++;
    }
  }

  return "text/plain";
}

void generate_text_response(char *file_name, FILE *target_file,
                            Response *response) {
  load_text_file(response, target_file);
  strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].key,
         "Content-type");
  strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].value,
         content_type_from_filename(file_name));
  strcpy(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].key,
         "Content-length");
  sprintf(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].value,
          "%ld", strlen(response->body));

  Response_set_status(response, "HTTP/1.1 200 OK");
}

long generate_binary_response(char *file_name, FILE *target_file,
                              Response *response) {
  fseek(target_file, 0, SEEK_END);
  long fsize = ftell(target_file);
  rewind(target_file);

  response->body = (char *)calloc(fsize, sizeof(char));
  size_t read_size = fread(response->body, 1, fsize, target_file);
  if (read_size != (size_t)fsize) {
    fprintf(stderr, "Warning: fread read %zu bytes, expected %ld bytes\n",
            read_size, fsize);
  }

  strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].key,
         "Content-type");
  strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].value,
         content_type_from_filename(file_name));
  strcpy(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].key,
         "Content-length");
  sprintf(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].value,
          "%ld", fsize);

  Response_set_status(response, "HTTP/1.1 200 OK");

  return fsize;
}

int file_type_from_filename(char *filename) {
  char *dot = strrchr(filename, '.');
  mime_map *map = (mime_map *)MIME_TYPES;
  if (dot) {
    while (map->extension) {
      if (strcmp(map->extension, dot) == 0) {
        return map->file_type;
      }
      map++;
    }
  }
  return FILE_ASCII;
}

extern int respond(char *request_message, char *root_directory,
                   int response_target_fd, SSL *ssl) {
  FILE *target_file = NULL;
  Request *request = Request_new(request_message);

  char *full_path = (char *)calloc(PATH_BUFFER_SIZE, sizeof(char));
  strcpy(full_path, root_directory);
  strcat(full_path, request->request_header_values[1].value);

  char *resolved_path = (char *)calloc(PATH_BUFFER_SIZE, sizeof(char));
  char *realpath_result = realpath(full_path, resolved_path);
  free(full_path);

  char *systime = (char *)calloc(TIME_STRING_BUFFER_SIZE, sizeof(char));
  formated_system_datetime(systime, HEADER_DATE_FORMAT);

  Response *response = Response_new();
  strcpy(response->header_values[0].key, "Date");
  strcpy(response->header_values[0].value, systime);
  strcpy(response->header_values[1].key, "Server");
  strcpy(response->header_values[1].value, SERVER_NAME);
  strcpy(response->header_values[2].key, "Connection");
  // Set Connection header based on request
  if (request->keep_alive) {
    strcpy(response->header_values[2].value, "keep-alive");
  } else {
    strcpy(response->header_values[2].value, "close");
  }
  free(systime);

  if (realpath_result == NULL) {
    // realpath failed, treat as 404
    const char *body = NOT_FOUND_BODY;
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].key,
           "Content-type");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].value,
           "text/html");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].key,
           "Content-length");
    snprintf(
        response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].value,
        RESPONSE_HEADER_ITEM_STRING_LENGTH, "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 404 Not Found");
    Response_set_body_as_text(response, body);
    Response_create_header(response);
    int send_message_size = connection_write(
        response_target_fd, ssl, response->header, strlen(response->header));
    if (send_message_size < 0) {
      free(resolved_path);
      cleanup(request, response, NULL);
      return -1;
    }
    send_message_size = connection_write(
        response_target_fd, ssl, response->body, strlen(response->body));
    if (send_message_size < 0) {
      free(resolved_path);
      cleanup(request, response, NULL);
      return -1;
    }

    // Only close connection if not keep-alive
    int keep_alive = request->keep_alive;
    if (!keep_alive) {
      int close_result = connection_close(response_target_fd, ssl);
      if (close_result < 0) {
        free(resolved_path);
        cleanup(request, response, NULL);
        return 0; // Return 0 to indicate connection should be closed
      }
    }

    free(resolved_path);
    cleanup(request, response, NULL);
    return keep_alive;
  }

  struct stat st;
  int stat_result;
  stat_result = stat(resolved_path, &st);

  // file exists and regular file
  if (stat_result != 0 || !S_ISREG(st.st_mode)) {
    const char *body = NOT_FOUND_BODY;
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].key,
           "Content-type");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].value,
           "text/html");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].key,
           "Content-length");
    sprintf(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].value,
            "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 404 Not Found");
    Response_set_body_as_text(response, body);
    Response_create_header(response);
    int send_message_size = connection_write(
        response_target_fd, ssl, response->header, strlen(response->header));
    if (send_message_size < 0) {
      return -1;
    }
    send_message_size = connection_write(
        response_target_fd, ssl, response->body, strlen(response->body));
    if (send_message_size < 0) {
      return -1;
    }

    // Only close connection if not keep-alive
    int keep_alive = request->keep_alive;
    if (!keep_alive) {
      int close_result = connection_close(response_target_fd, ssl);
      if (close_result < 0) {
        cleanup(request, response, target_file);
        return 0; // Return 0 to indicate connection should be closed
      }
    }

    cleanup(request, response, target_file);
    return keep_alive;
  }

  target_file = fopen(resolved_path, "rb");
  if (target_file == NULL) {
    perror("Failed to fopen target file");
    const char *body = BAD_REQUEST;
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].key,
           "Content-type");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].value,
           "text/html");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].key,
           "Content-length");
    sprintf(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].value,
            "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 500 Internal Server Error");
    Response_set_body_as_text(response, body);
    Response_create_header(response);
    int send_message_size = connection_write(
        response_target_fd, ssl, response->header, strlen(response->header));
    if (send_message_size < 0) {
      return -1;
    }
    send_message_size = connection_write(
        response_target_fd, ssl, response->body, strlen(response->body));
    if (send_message_size < 0) {
      return -1;
    }

    // Only close connection if not keep-alive
    int keep_alive = request->keep_alive;
    if (!keep_alive) {
      int close_result = connection_close(response_target_fd, ssl);
      if (close_result < 0) {
        free(resolved_path);
        cleanup(request, response, target_file);
        return 0; // Return 0 to indicate connection should be closed
      }
    }

    free(resolved_path);
    cleanup(request, response, target_file);

    return keep_alive;
  }

  char *file_name = calloc(FILENAME_BUFFER_SIZE, sizeof(char));
  last_strtok(file_name, resolved_path, "/");
  free(resolved_path);

  int file_type = file_type_from_filename(file_name);
  long body_len = 0;
  if (file_type == FILE_ASCII) {
    generate_text_response(file_name, target_file, response);
    body_len = strlen(response->body);
  } else if (file_type == FILE_BINARY) {
    body_len = generate_binary_response(file_name, target_file, response);
  } else {
    const char *body = UNSUPPORTED_MEDIA;
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].key,
           "Content-type");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_TYPE_INDEX].value,
           "text/html");
    strcpy(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].key,
           "Content-length");
    sprintf(response->header_values[RESPONSE_HEADER_CONTENT_LENGTH_INDEX].value,
            "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 415 Unsupported Media Type");
    Response_set_body_as_text(response, body);
    body_len = strlen(response->body);
  }
  free(file_name);

  Response_create_header(response);
  int send_message_size = connection_write(
      response_target_fd, ssl, response->header, strlen(response->header));
  if (send_message_size < 0) {
    return -1;
  }

  send_message_size =
      connection_write(response_target_fd, ssl, response->body, body_len);
  if (send_message_size < 0) {
    return -1;
  }

  // Only close connection if not keep-alive
  int keep_alive = request->keep_alive;
  if (!keep_alive) {
    int close_result = connection_close(response_target_fd, ssl);
    if (close_result < 0) {
      cleanup(request, response, target_file);
      return 0; // Return 0 to indicate connection should be closed
    }
  }

  cleanup(request, response, target_file);
  return keep_alive;
}
