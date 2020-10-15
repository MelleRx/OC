#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <cstddef>
#include <cstdio>
#include <sys/un.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <algorithm>
#include <set>


int main() {
    int listener;
    struct sockaddr_in addr; // address family, port number, IP, "addition" to the size of the sockaddr structure
    char buf[1024];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0); // create socket(internet domain(IPv4), pre-installation of the connection, protocol)

    // checking for the existence of a socket
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    fcntl(listener, F_SETFL, O_NONBLOCK); // additional operations on the fd file descriptor(socket, file status flag(F_SETFL), ...)

    addr.sin_family = AF_INET; // internet domain(IPv4)
    addr.sin_port = htons(3425); // Host TO Network Short
    addr.sin_addr.s_addr = INADDR_ANY; // Host TO Network Long(any interface)

    // naming a socket(socket , pointer to a structure with address, len of structure)
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    // create a queue of connection requests(socket, len of queue)
    if (listen(listener, 2) < 0) {
        perror("listen");
        exit(3);
    }

    std::set<int> clients; // set of clients
    clients.clear();

    // server is ready to accept a request
    while (true) {
        // fill set of sockets
        fd_set readset;
        FD_ZERO(&readset); // clear set
        FD_SET(listener, &readset); // add descriptor fd in set

        for (std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)
            FD_SET(*it, &readset);

        // setting a timeout
        timeval timeout;
        timeout.tv_sec = 15;
        timeout.tv_usec = 0;

        // waiting for an event in one of the sockets
        int mx = std::max(listener, *max_element(clients.begin(), clients.end()));
        if (select(mx+1, &readset, NULL, NULL, &timeout) <= 0) {
            perror("select");
            exit(3);
        }

        // defining event type and performing the appropriate actions
        // FD_ISSET - check fd in set
        if (FD_ISSET(listener, &readset)) {
            // creates a new socket to communicate with client and returns descriptor
            // (listening socket, structure pointer, len of structure pointer)
            // NULL - not fill
            int sock = accept(listener, NULL, NULL);
            if (sock < 0) {
                perror("accept");
                exit(4);
            }
            fcntl(sock, F_SETFL, O_NONBLOCK);
            clients.insert(sock);
        }
        for (std::set<int>::iterator it = clients.begin(); it != clients.end(); it++) {
            if (FD_ISSET(*it, &readset)) {
                // get data from client, read them
                bytes_read = recv(*it, buf, 1024, 0);
                if (bytes_read <= 0) {
                    // connection is broken, delete socket from set
                    close(*it);
                    clients.erase(*it);
                    continue;
                }
                // sending data back to the client
                send(*it, buf, bytes_read, 0);
            }
        }
    }
    return 0;
}

