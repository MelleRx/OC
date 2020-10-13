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

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

std::vector<std::string> g_quotes = {
        "Oleg molodec"
};

int main() {
    addrinfo hints, * ai_res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    int fd = getaddrinfo("localhost", "80", &hints, &ai_res);
    if(fd < 0) {
        std::cerr << gai_strerror(fd) << std::endl;
        return -1;
    }

    std::string errfn;

    for(addrinfo * ai = ai_res; ai != NULL; ai = ai->ai_next) {
        if((fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0) {
            errfn = "socket";
            continue;
        }

        int yes = 1;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
            errfn = "setsockopt";
            continue;
        }

        if(bind(fd, ai->ai_addr, ai->ai_addrlen) != 0) {
            errfn = "bind";
            close(fd);
            continue;
        } else {
            if(listen(fd, 10) < 0) {
                errfn = "listen";
                close(fd);
                continue;
            }

            std::ostringstream oss;
            char addr[INET6_ADDRSTRLEN];
            memset(addr, 0, sizeof(addr));

            std::cout << "Listening on " << oss.str() << std::endl;
            break;
        }
    }

    freeaddrinfo(ai_res);

    if(errfn != "") {
        std::cerr << "Could not bind to port: " << errfn << ": " << strerror(errno)
                  << std::endl;
        return -1;
    }

    srand(time(NULL));
    while(true) {
        int ret, newfd;
        char tmp[4];

        if((newfd = accept(fd, NULL, NULL)) < 0) {
            std::cerr << "Could not accept: " << strerror(errno) << std::endl;
            return -1;
        }

        while(recv(newfd, tmp, sizeof(tmp), MSG_DONTWAIT) == -1 &&
              (errno == EAGAIN || errno == EWOULDBLOCK)) {}

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

    return 0;
}

