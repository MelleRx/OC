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
#include <cstring>


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
    addr.sin_port = htons(80); // Host TO Network Short
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // Host TO Network Long(any interface)

    // naming a socket(socket , pointer to a structure with address, len of structure)
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }

    printf("Listener on port %d \n", 80);

    // create a queue of connection requests(socket, len of queue)
    if (listen(listener, 3) < 0) {
        perror("listen");
        exit(3);
    }

    puts("Waiting for connections ...");

    // server is ready to accept a request
    while (true) {
        // creates a new socket to communicate with client and returns descriptor
        // (listening socket, structure pointer, len of structure pointer)
        // NULL - not fill
        sock = accept(listener, NULL, NULL);
        if (sock < 0) {
            perror("accept");
            exit(4);
        }
        /*
        while (true) {
            // reading data from socket(socket, buf pointer, len of buf, combination of bit flags)
            bytes_read = recv(sock, buf, 1024, 0); // if flags = 0 -> delete data from the socket
            if (bytes_read <= 0) break;
            send(sock, buf, bytes_read, 0); // sending data(socket, buf pointer, len of buf, combination of bit flags)
        }
        close(sock); // closing a socket
        */
        FD_ZERO(&readfds);

        FD_SET(listener, &readfds);
        max_sd = listener;

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    getpeername(sd, (struct sockaddr *) &addr, (socklen_t * ) & sizeof(addr));
                    printf("Host disconnected , ip %s , port %d \n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                    close(sd);
                    user_count--;
                    client_socket[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    for (i = 0; i < max_clients; i++) {
                        sd = client_socket[i];
                        send(sd, buffer, strlen(buffer), 0);
                    }
                    buffer[1024] = {0};
                }
            }
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }
        sd = cline;

        void *server(void *);

        pthread_create(&threadA[0], NULL, server, NULL);
        pthread_join(threadA[0], NULL);

        void *write(void *);
        void *read(void *);

        pthread_create(&threadA[0], NULL, write, NULL);
        pthread_create(&threadA[1], NULL, read, NULL);

        pthread_join(threadA[1], NULL);
        pthread_join(threadA[0], NULL);

        void *write(void *dummyPt) {
            for (;;) {
                char s[1024];
                std::cout << "<----";
                bzero(s, 1024 + 1);
                std::cin.getline(s, 1024);
                send(sock, s, strlen(s), 0);
            }
            close(sock);
        }

        void *read(void *dummyPt) {
            char test[1024];
            bzero(test, 1024 + 1);
            bool loop = false;
            while (!loop) {
                bzero(test, 1024 + 1);
                int rc = read(sock, test, 1024);
                if (rc > 0) {
                    string tester(test);
                    std::cout << ": " << tester << std::endl;
                    if (tester == "exit_server") break;
                }
            }

            cout << "\nClosing thread and conn" << endl;
            close(sock);
        }
    }
    return 0;
}

