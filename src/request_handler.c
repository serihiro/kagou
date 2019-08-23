#include "request_handler.h"

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
  fpos_t fpos;
  fseek(target_file, 0, SEEK_END);
  fgetpos(target_file, &fpos);
  rewind(target_file);

  long fsize = sizeof(char) * (fpos + 1);
  char *fbuf = (char *)calloc(fsize, sizeof(char));

  fread(fbuf, fsize, 1, target_file);
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
  strcpy(response->header_values[3].key, "Content-type");
  strcpy(response->header_values[3].value,
         content_type_from_filename(file_name));
  strcpy(response->header_values[4].key, "Content-length");
  sprintf(response->header_values[4].value, "%ld", strlen(response->body));

  Response_set_status(response, "HTTP/1.1 200 OK");
}

extern int respond(char *request_message, char *root_directory,
                   int response_target_fd) {
  FILE *target_file = NULL;
  Request *request = Request_new(request_message);

  char *full_path = (char *)calloc(MAXPATHLEN + 1, sizeof(char));
  strcpy(full_path, root_directory);
  strcat(full_path, request->request_header_values[1].value);

  char *resolved_path = (char *)calloc(MAXPATHLEN + 1, sizeof(char));
  realpath(full_path, resolved_path);
  free(full_path);
  if (resolved_path == NULL) {
    perror("Failed to solve path");
    exit(1);
  }

  char *systime = (char *)calloc(128 + 1, sizeof(char));
  formated_system_datetime(systime, HEADER_DATE_FORMAT);

  Response *response = Response_new();
  strcpy(response->header_values[0].key, "Date");
  strcpy(response->header_values[0].value, systime);
  strcpy(response->header_values[1].key, "Server");
  strcpy(response->header_values[1].value, SERVER_NAME);
  strcpy(response->header_values[2].key, "Connection");
  strcpy(response->header_values[2].value, "close"); // FIXME toriisogi
  free(systime);

  struct stat st;
  int stat_result;
  stat_result = stat(resolved_path, &st);

  // file exists and regular file
  if (stat_result != 0 || (st.st_mode & S_IFMT) != S_IFREG) {
    const char *body = NOT_FOUND_BODY;
    strcpy(response->header_values[3].key, "Content-type");
    strcpy(response->header_values[3].value, "text/html");
    strcpy(response->header_values[4].key, "Content-length");
    sprintf(response->header_values[4].value, "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 404 Not Found");
    Response_set_body_as_text(response, body);
    Response_create_header(response);
    int send_message_size =
        send(response_target_fd, response->header, strlen(response->header), 0);
    if (send_message_size < 0) {
      return -1;
    }
    send_message_size =
        send(response_target_fd, response->body, strlen(response->body), 0);
    if (send_message_size < 0) {
      return -1;
    }

    // To send FIN
    int close_result = close(response_target_fd);
    if (close_result < 0) {
      return -1;
    }

    cleanup(request, response, target_file);
    return 0;
  }

  target_file = fopen(resolved_path, "r");
  if (target_file == NULL) {
    perror("Failed to fopen target file");
    const char *body = BAD_REQUEST;
    strcpy(response->header_values[3].key, "Content-type");
    strcpy(response->header_values[3].value, "text/html");
    strcpy(response->header_values[4].key, "Content-length");
    sprintf(response->header_values[4].value, "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 500 Internal Server Error");
    Response_set_body_as_text(response, body);
    Response_create_header(response);
    int send_message_size =
        send(response_target_fd, response->header, strlen(response->header), 0);
    if (send_message_size < 0) {
      return -1;
    }
    send_message_size =
        send(response_target_fd, response->body, strlen(response->body), 0);
    if (send_message_size < 0) {
      return -1;
    }

    // To send FIN
    int close_result = close(response_target_fd);
    if (close_result < 0) {
      return -1;
    }

    free(resolved_path);
    cleanup(request, response, target_file);

    return 0;
  }

  char *file_name = calloc(MAXPATHLEN + 1, sizeof(char));
  last_strtok(file_name, resolved_path, "/");
  free(resolved_path);

  // TODO functionize
  if (strstr(file_name, ".html") != NULL || strstr(file_name, ".htm") != NULL ||
      strstr(file_name, ".js") != NULL || strstr(file_name, ".css") != NULL ||
      strstr(file_name, ".csv") != NULL) {
    generate_text_response(file_name, target_file, response);
  } else {
    const char *body = UNSUPPORTED_MEDIA;
    strcpy(response->header_values[3].key, "Content-type");
    strcpy(response->header_values[3].value, "text/html");
    strcpy(response->header_values[4].key, "Content-length");
    sprintf(response->header_values[4].value, "%ld", strlen(body));

    Response_set_status(response, "HTTP/1.1 415 Unsupported Media Type");
    Response_set_body_as_text(response, body);
    Response_create_header(response);
  }
  free(file_name);

  Response_create_header(response);
  int send_message_size =
      send(response_target_fd, response->header, strlen(response->header), 0);
  if (send_message_size < 0) {
    return -1;
  }

  send_message_size =
      send(response_target_fd, response->body, strlen(response->body), 0);
  if (send_message_size < 0) {
    return -1;
  }

  // To send FIN
  int close_result = close(response_target_fd);
  if (close_result < 0) {
    return -1;
  }

  cleanup(request, response, target_file);
  return 0;
}
