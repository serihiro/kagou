#include "./request_handler.h"
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define DEFAULT_ROOT_DIRECTORY "."

int accept_socket_fd;
int request_socket_fd;

void sigint_handler() {
  if (accept_socket_fd) {
    close(accept_socket_fd);
  }

  if (request_socket_fd) {
    close(request_socket_fd);
  }
  exit(0);
}

void init_signal() {
  struct sigaction sa_sigint;
  memset(&sa_sigint, 0, sizeof(sa_sigint));
  sa_sigint.sa_handler = sigint_handler;
  sa_sigint.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &sa_sigint, NULL) < 0) {
    perror("Failed to bind sigaction\n");
    exit(1);
  }
  signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    puts("Usage: /path/to/binary *port root_file_directory");
    exit(1);
  }
  char root_directory[BUFFERSIZE];
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
  char raw_message[BUFFERSIZE];

  while (1) {
    int accept_socket_fd =
        accept(request_socket_fd, (struct sockaddr *)&client_address, &len);
    if (accept_socket_fd < 0) {
      close(request_socket_fd);
      perror("Failed to accept\n");
      exit(1);
    }

    while (1) {
      memset(&raw_message, 0, sizeof(raw_message));
      int raw_message_size = recv(accept_socket_fd, raw_message, BUFFERSIZE, 0);
      if (!raw_message_size || raw_message_size < 0) {
        close(accept_socket_fd);
        break;
      }

      respond(raw_message, root_directory, accept_socket_fd);
    }
  }

  return (0);
}
