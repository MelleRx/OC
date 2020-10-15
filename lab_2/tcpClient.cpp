#include "tcpClient.h"
#include <ws2tcpip.h>
#include <stdio.h>

tcpClient::tcpClient() noexcept : _status(status::disconnected) {}

tcpClient::~tcpClient() {
    disconnect();
    WSACleanup();
}

tcpClient::status tcpClient::connectTo(uint32_t host, uint16_t port) noexcept {
    if(WSAStartup(MAKEWORD(2, 2), &w_data) != 0) {}

    if((client_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_IP)) == INVALID_SOCKET)
        return _status = status::err_socket_init;

    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.S_un.S_addr = host;
    dest_addr.sin_port = htons(port);

    if(connect(client_socket, (sockaddr *)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
        closesocket(client_socket);
        return _status = status::err_socket_connect;
    }
    return _status = status::connected;
}

tcpClient::status tcpClient::disconnect() noexcept {
    if(_status != status::connected)
        return _status;
    closesocket(client_socket);
    return _status = status::disconnected;
}

int tcpClient::loadData() {return recv (client_socket, buffer, buffer_size, 0);}
char* tcpClient::getData() {return buffer;}

bool tcpClient::sendData(const char* buffer, const size_t size) const {
    if(send(client_socket, buffer, size, 0) < 0) return false;
    return true;
}