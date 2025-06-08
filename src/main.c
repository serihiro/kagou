#define _GNU_SOURCE
#include "request_handler.h"
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define ROOT_DIRECTORY_STRING_LENGTH 4097     // 4k + 1
#define REQUEST_MESSAGE_STRING_LENGTH 1024001 // 1M + 1

static const char *DEFAULT_ROOT_DIRECTORY = ".";

int accept_socket_fd = 0;
int request_socket_fd = 0;

void cleanup_and_exit(int sig) {
  printf("\nReceived signal %d, cleaning up...\n", sig);
  fflush(stdout);

  if (accept_socket_fd > 0) {
    shutdown(accept_socket_fd, SHUT_RDWR);
    close(accept_socket_fd);
    accept_socket_fd = 0;
  }

  if (request_socket_fd > 0) {
    shutdown(request_socket_fd, SHUT_RDWR);
    close(request_socket_fd);
    request_socket_fd = 0;
  }

  exit(0);
}

void init_signal() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = cleanup_and_exit;
  sa.sa_flags = SA_RESTART;

  // SIGINTハンドラーの設定
  if (sigaction(SIGINT, &sa, NULL) < 0) {
    perror("Failed to bind SIGINT handler");
    exit(1);
  }

  // SIGTERMハンドラーの設定
  if (sigaction(SIGTERM, &sa, NULL) < 0) {
    perror("Failed to bind SIGTERM handler");
    exit(1);
  }

  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    puts("Usage: /path/to/binary *port root_file_directory");
    exit(1);
  }
  char *root_directory =
      (char *)calloc(ROOT_DIRECTORY_STRING_LENGTH, sizeof(char));
  if (argc == 3) {
    strcpy(root_directory, argv[2]);
  } else {
    strcpy(root_directory, DEFAULT_ROOT_DIRECTORY);
  }

  init_signal();

  request_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (request_socket_fd < 0) {
    perror("Failed to create socket\n");
    exit(1);
  }

  // SO_REUSEADDRオプションを設定して、ポートの再利用を許可
  int reuse = 1;
  if (setsockopt(request_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                 sizeof(reuse)) < 0) {
    perror("Failed to set SO_REUSEADDR");
    close(request_socket_fd);
    exit(1);
  }

  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = PF_INET;
  server_address.sin_port = htons(atoi(argv[1]));

  int bind_result = bind(request_socket_fd, (struct sockaddr *)&server_address,
                         sizeof(struct sockaddr_in));
  if (bind_result < 0) {
    perror("Failed to bind\n");
    exit(1);
  }

  int listen_result = listen(request_socket_fd, 5);
  if (listen_result < 0) {
    perror("Failed to listen\n");
    exit(1);
  }

  struct sockaddr_in client_address;
  memset(&client_address, 0, sizeof(client_address));

  socklen_t len = sizeof(client_address);
  char *raw_message =
      (char *)calloc(REQUEST_MESSAGE_STRING_LENGTH, sizeof(char));

  int raw_message_size = 0;
  while (1) {
    accept_socket_fd =
        accept(request_socket_fd, (struct sockaddr *)&client_address, &len);
    if (accept_socket_fd < 0) {
      close(request_socket_fd);
      perror("Failed to accept\n");
      exit(1);
    }

    while (1) {
      raw_message_size = recv(accept_socket_fd, raw_message, 1024, 0);
      if (!raw_message_size || raw_message_size < 0) {
        close(accept_socket_fd);
        accept_socket_fd = 0;
        break;
      }

      respond(raw_message, root_directory, accept_socket_fd);
      memset(raw_message, 0, REQUEST_MESSAGE_STRING_LENGTH);
    }
  }

  free(root_directory);
  free(raw_message);
  return (0);
}
