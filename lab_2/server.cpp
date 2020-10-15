#include <sys/types.h>
#include <sys/socket.h>
#include <cstddef>
#include <cstdio>
#include <sys/un.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

int main() {
    int sock, listener;
    struct sockaddr_in addr; // address family, port number, IP, "addition" to the size of the sockaddr structure
    char buf[1024];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0); // create socket(internet domain(IPv4), pre-installation of the connection, protocol)

    // checking for the existence of a socket
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET; // internet domain(IPv4)
    addr.sin_port = htons(3425); // Host TO Network Short
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // Host TO Network Long(any interface)

    // naming a socket(socket , pointer to a structure with address, len of structure)
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    listen(listener, 10); // create a queue of connection requests(socket, len of queue)

    // server is ready to accept a request
    while (true) {
        // creates a new socket to communicate with client and returns descriptor
        // (listening socket, structure pointer, len of structure pointer)
        // NULL - not fill
        sock = accept(listener, NULL, NULL);
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        while (true) {
            // reading data from socket(socket, buf pointer, len of buf, combination of bit flags)
            bytes_read = recv(sock, buf, 1024, 0); // if flags = 0 -> delete data from the socket
            if (bytes_read <= 0) break;
            const char* str = "Oleg";
            send(sock, str, strlen(str), 0); // sending data(socket, buf pointer, len of buf, combination of bit flags)
        }
        close(sock); // closing a socket
    }
    return 0;
}

