#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

int main(int argc, char **argv) {
    int sock;
    char message[1024];
    char buf[sizeof(message)];
    struct sockaddr_in addr;
    struct hostent* hostinfo;
    int port = atoi(argv[1]);
    hostinfo = argv[2];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    while (true) {
        printf("Enter message to server (For exit: exit): ");
        if (!strcmp(gets(message), "exit")){close(sock);return 0;}
        printf("sending a message to the server...\n");
        send(sock, message, sizeof(message), 0);
        int bytes_read = 0;
        printf("Waiting for a message\n");
        bytes_read = recv(sock, buf, sizeof(message), 0);
        printf("Received %d bytes\tMessage: %s\n", bytes_read, buf);
    }
    return 0;
}

