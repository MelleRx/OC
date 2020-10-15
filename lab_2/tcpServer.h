#ifndef LAB_2_TCPSERVER_H
#define LAB_2_TCPSERVER_H

#include <cstdint>
#include <functional>
#include <thread>
#include <list>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


static constexpr uint16_t buffer_size = 1024;

struct tcpServer {
    class Client;
    typedef std::function<void(Client)> handler_function_t;
    enum class status : uint8_t {
        up = 0,
        err_socket_init = 1,
        err_socket_bind = 2,
        err_socket_listening = 3,
        close = 4
    };

private:
    uint16_t port;
    status _status = status::close;
    handler_function_t handler;

    std::thread handler_thread;
    std::list<std::thread> client_handler_threads;
    std::list<std::thread::id> client_handling_end;

    int serv_socket;
    void handlingLoop();

public:
    tcpServer(const uint16_t port, handler_function_t handler);
    ~tcpServer();

    //! Set client handler
    void setHandler(handler_function_t handler);

    uint16_t getPort() const;
    uint16_t setPort(const uint16_t port);

    status getStatus() const {return _status;}

    status restart();
    status start();
    void stop();

    void joinLoop();
};

class tcpServer::Client {
    int socket;
    struct sockaddr_in address;
    char buffer[buffer_size];
public:
    Client(int socket, struct sockaddr_in address);
public:
    Client(const Client& other);
    ~Client();
    uint32_t getHost() const;
    uint16_t getPort() const;

    int loadData();
    char* getData();

    bool sendData(const char* buffer, const size_t size) const;
};


#endif
