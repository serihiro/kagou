#define _GNU_SOURCE
#include "request_handler.h"
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define ROOT_DIRECTORY_STRING_LENGTH PATH_MAX + 1    // PATH_MAX + 1 (typically 4096+1)
#define REQUEST_MESSAGE_STRING_LENGTH 8193           // 8K + 1 (sufficient for HTTP requests)

static const char *DEFAULT_ROOT_DIRECTORY = ".";

int accept_socket_fd = 0;
int request_socket_fd = 0;
int use_tls = 0;
SSL_CTX *ssl_ctx = NULL;

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

  if (ssl_ctx) {
    SSL_CTX_free(ssl_ctx);
    ssl_ctx = NULL;
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
    puts("Usage: /path/to/binary <port> [root_directory] [--https]");
    exit(1);
  }
  char *root_directory =
      (char *)calloc(ROOT_DIRECTORY_STRING_LENGTH, sizeof(char));
  strcpy(root_directory, DEFAULT_ROOT_DIRECTORY);
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--https") == 0) {
      use_tls = 1;
      puts("TLS 1.2+ is available");
    } else {
      strncpy(root_directory, argv[i], ROOT_DIRECTORY_STRING_LENGTH - 1);
      root_directory[ROOT_DIRECTORY_STRING_LENGTH - 1] = '\0'; // Ensure null termination
    }
  }

  init_signal();

  if (use_tls) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_server_method();
    ssl_ctx = SSL_CTX_new(method);
    SSL_CTX_set_min_proto_version(ssl_ctx, TLS1_2_VERSION);
    if (!ssl_ctx) {
      ERR_print_errors_fp(stderr);
      exit(1);
    }
    if (SSL_CTX_use_certificate_file(ssl_ctx, "cert/server.crt",
                                     SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ssl_ctx, "cert/server.key",
                                    SSL_FILETYPE_PEM) <= 0 ||
        !SSL_CTX_check_private_key(ssl_ctx)) {
      ERR_print_errors_fp(stderr);
      exit(1);
    }
  }

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

    SSL *ssl = NULL;
    if (use_tls) {
      ssl = SSL_new(ssl_ctx);
      SSL_set_fd(ssl, accept_socket_fd);
      if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(accept_socket_fd);
        accept_socket_fd = 0;
        continue;
      }
    }

    // Set socket timeout for keep-alive connections
    struct timeval timeout;
    timeout.tv_sec = 5; // 5 seconds timeout
    timeout.tv_usec = 0;
    setsockopt(accept_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
               sizeof(timeout));

    int keep_alive = 1;
    while (keep_alive) {
      if (use_tls)
        raw_message_size =
            SSL_read(ssl, raw_message, REQUEST_MESSAGE_STRING_LENGTH - 1);
      else
        raw_message_size = recv(accept_socket_fd, raw_message,
                                REQUEST_MESSAGE_STRING_LENGTH - 1, 0);
      if (!raw_message_size || raw_message_size < 0) {
        if (use_tls) {
          SSL_shutdown(ssl);
          SSL_free(ssl);
        }
        close(accept_socket_fd);
        accept_socket_fd = 0;
        break;
      }

      // Ensure null termination
      raw_message[raw_message_size] = '\0';

      keep_alive = respond(raw_message, root_directory, accept_socket_fd, ssl);
      memset(raw_message, 0, REQUEST_MESSAGE_STRING_LENGTH);

      // If respond returns -1 (error) or 0 (close), close the connection
      if (keep_alive <= 0) {
        accept_socket_fd = 0;
        break;
      }
    }
  }

  free(root_directory);
  free(raw_message);
  return (0);
}
