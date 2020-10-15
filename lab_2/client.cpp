#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

int main() {
    int sock;
    struct sockaddr_in addr;
    struct hostent* hostinfo;
    port = atoi(3425)

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    send(sock, message, sizeof(message), 0);
    recv(sock, buf, sizeof(message), 0);

    close(sock);

    return 0;
}

