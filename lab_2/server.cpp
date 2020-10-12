//
// Created by kukul on 12.10.2020.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <cerrno>
#include <cstring>

int main() {
    addrinfo serverCfg, *ai_res;
    memset(&serverCfg, 0, sizeof serverCfg);
    serverCfg.ai_socktype = SOCK_STREAM;
    serverCfg.ai_family = AF_UNSPEC;
    serverCfg.ai_flags = AI_PASSIVE;
    int fd = getaddrinfo("localhost", "80", &serverCfg, &ai_res);
    if (fd < 0) {
        std::cerr << gai_strerror(fd) << std::endl;
        return -1;
    }
    int s;
    s = socket(ai_res->ai_family, ai_res->ai_socktype, ai_res->ai_protocol);
    bind(s, ai_res->ai_addr, ai_res->ai_addrlen);
    backlog = 10;
    listen(s, backlog);
    while (true) {
        int new_fd;
        char tmp[1024];
        addr_size = sizeof their_addr;
        new_fd = accept(s, (struct sockaddr *)&their_addr, &addr_size);
        std::string quote = g_quotes[rand() % g_quotes.size()];
        std::stringstream buf;
        buf << "HTTP/1.1 200 OK\r\n";
        buf << "Server: space-quotes/0.0.1\r\n";
        buf << "Content-Type: text/plain; charset=utf-8\r\n";
        buf << "Content-Length: " << quote.size() << "\r\n";
        buf << "Connection: close\r\n\r\n";
        buf << quote;

        send(newfd, buf.str().c_str(), buf.str().size() , 0);
        close(newfd);
    }

}

