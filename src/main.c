#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "./request_handler.h"

#define BUFFERSIZE  1024

int accept_socket_fd;
int request_socket_fd;

void sigint_handler(int signum){
    if(accept_socket_fd){
        close(accept_socket_fd);
    }

    if(request_socket_fd){
        close(request_socket_fd);
    }
    exit(0);
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        puts("Usage: /path/to/binary port");
        exit(1);
    }

    struct sigaction sa_sigint;
    memset(&sa_sigint, 0, sizeof(sa_sigint));
    sa_sigint.sa_handler = sigint_handler;
    sa_sigint.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa_sigint, NULL) < 0) {
        perror("Failed to bind sigaction\n");
        exit(1);
    }

    request_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(request_socket_fd < 0) {
        perror("Failed to create socket\n");
        exit(1);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = PF_INET;
    server_address.sin_port   = htons(atoi(argv[1]));
    signal(SIGPIPE, SIG_IGN);

    int bind_result = bind(request_socket_fd, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
    if(bind_result < 0) {
        perror("Failed to bind\n");
        exit(1);
    }

    int listen_result = listen(request_socket_fd, 5);
    if(listen_result < 0) {
        perror("Failed to listen\n");
        exit(1);
    }

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));

    int accept_socket_fd;
    socklen_t len = sizeof(client_address);
    char buf[BUFFERSIZE];

    while(1){
        accept_socket_fd = accept(request_socket_fd, (struct sockaddr*)&client_address, &len);
        if (accept_socket_fd < 0){
            close(request_socket_fd);
            perror("Failed to accept\n");
            exit(1);
        }

        while(1) {
            memset(&buf, 0, sizeof(buf));
            int recv_message_size = recv(accept_socket_fd, buf, BUFFERSIZE, 0);
            if (!recv_message_size || recv_message_size < 0) {
                close(accept_socket_fd);
                break;
            }

            // If using malloc, somehow this cannot send response..
            char contents[1024 * 1000];
            char body[1024 * 500];
            load_body(body, "test/test.html");
            header(contents, strlen(body));
            strcat(contents, "\n");
            strcat(contents, body);

            int send_message_size = send(accept_socket_fd, contents, sizeof(contents), 0);
            if(send_message_size < 0) {
                close(accept_socket_fd);
                break;
            }
        }
    }

    return (0);
}
