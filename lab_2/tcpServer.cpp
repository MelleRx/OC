#include "tcpServer.h"
#include <chrono>

tcpServer::tcpServer(const uint16_t port, handler_function_t handler) : port(port), handler(handler) {}


tcpServer::~tcpServer() {
    if (_status == status::up)
        stop();
}

//Задаёт callback-функцию запускаямую при подключении клиента
void TcpServer::setHandler(tcpServer::handler_function_t handler) {this->handler = handler;}

//Getter/Setter порта
uint16_t tcpServer::getPort() const {return port;}
uint16_t tcpServer::setPort( const uint16_t port) {
    this->port = port;
    restart(); //Перезапустить если сервер был запущен
    return port;
}

//Перезапуск сервера
tcpServer::status tcpServer::restart() {
    if (_status == status::up)
        stop ();
    return start();
}

// Вход в поток обработки соединений
void tcpServer::joinLoop() {handler_thread.join();}

//Загружает в буфер данные от клиента и возвращает их размер
int tcpServer::Client::loadData() {return recv(socket, buffer, buffer_size, 0);}
//Возвращает указатель на буфер с данными от клиента
char* tServer::Client::getData() {return buffer;}
//Отправляет данные клиенту
bool tcpServer::Client::sendData(const char* buffer, const size_t size) const {
    if (send(socket, buffer, size, 0) < 0) return false;
    return true;
}

//Запуск сервера
tcpServer::status tcpServer::start() {
    WSAStartup(MAKEWORD(2, 2), &w_data); //Задаём версию WinSocket

    SOCKADDR_IN address; //Структура хост/порт/протокол для инициализации сокета
    address.sin_addr.S_un.S_addr = INADDR_ANY; //Любой IP адресс
    address.sin_port = htons(port); //Задаём порт
    address.sin_family = AF_INET; //AF_INET - Cемейство адресов для IPv4

    //Инициализируем наш сокет и проверяем корректно ли прошла инициализация
    //в противном случае возвращаем статус с ошибкой
    if (static_cast<int>(serv_socket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) return _status = status::err_socket_init;

    //Присваиваем к сокету адресс и порт и проверяем на коректность сокет
    //в противном случае возвращаем статус с ошибкой
    if (bind(serv_socket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) return _status = status::err_socket_bind;

    //Запускаем прослушку и проверяем запустилась ли она
    //в противном случае возвращаем статус с ошибкой
    if (listen(serv_socket, SOMAXCONN) == SOCKET_ERROR) return _status = status::err_socket_listening;

    //Меняем статус, запускаем обработчик соединений и возвращаем статус
    _status = status::up;
    handler_thread = std::thread([this]{handlingLoop();});
    return _status;
}

//Остановка сервера
void tcpServer::stop() {
    _status = status::close; //Изменение статуса
    closesocket (serv_socket); //Закрытие сокета
    joinLoop(); //Ожидание завершения
    for(std::thread& cl_thr : client_handler_threads) //Перебор всех клиентских потоков
        cl_thr.join(); // Ожидание их завершения
    client_handler_threads.clear (); // Очистка списка клиентских потоков
    client_handling_end.clear (); // Очистка списка идентификаторов завершённых клиентских потоков
}

// Функиця обработки соединений
void tcpServer::handlingLoop() {
    while(_status == status::up) {
        SOCKET client_socket; //Сокет клиента
        SOCKADDR_IN client_addr; //Адресс клиента
        int addrlen = sizeof(client_addr); //Размер адреса клиента
        //Получение сокета и адреса клиента
        //(если сокет коректен и сервер зарущен запуск потока обработки)
        if ((client_socket = accept(serv_socket, (struct sockaddr*)&client_addr, &addrlen)) != 0 && _status == status::up) {
            client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] {
                handler(Client(client_socket, client_addr)); //Запуск callback-обработчика
                //Добавление идентификатора в список идентификаторов завершённых клиентских потоков
                client_handling_end.push_back (std::this_thread::get_id());
            }));
        }
        //Очистка отработанных клиентских потоков
        if(!client_handling_end.empty())
          for(std::list<std::thread::id>::iterator id_it = client_handling_end.begin (); !client_handling_end.empty() ; id_it = client_handling_end.begin())
            for(std::list<std::thread>::iterator thr_it = client_handler_threads.begin (); thr_it != client_handler_threads.end () ; ++thr_it)
              if(thr_it->get_id () == *id_it) {
                thr_it->join();
                client_handler_threads.erase(thr_it);
                client_handling_end.erase (id_it);
                break;
              }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Конструктор клиента по сокету и адресу
tcpServer::Client::Client(SOCKET socket, SOCKADDR_IN address) : socket(socket), address(address) {}
// Конструктор копирования
tcpServer::Client::Client(const tcpServer::Client& other) : socket(other.socket), address(other.address) {}

tcpServer::Client::~Client() {
    shutdown(socket, 0); //Обрыв соединения сокета
    closesocket(socket); //Закрытие сокета
}

// Геттеры хоста и порта
uint32_t tcpServer::Client::getHost() const {return address.sin_addr.S_un.S_addr;}
uint16_t tcpServer::Client::getPort() const {return address.sin_port;}

//Запуск сервера (по аналогии с реализацией для Windows)
tcpServer::status tcpServer::start() {
    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
    server.sin_family = AF_INET;
    serv_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(serv_socket == -1) return _status = status::err_socket_init;
    if(bind(serv_socket,(struct sockaddr *)&server , sizeof(server)) < 0) return _status = status::err_socket_bind;
    if(listen(serv_socket, 3) < 0)return _status = status::err_socket_listening;

    _status = status::up;
    handler_thread = std::thread([this]{handlingLoop();});
    return _status;
}

//Остановка сервера
void tcpServer::stop() {
    _status = status::close;
    close(serv_socket);
    joinLoop();
    for(std::thread& cl_thr : client_handler_threads)
        cl_thr.join();
    client_handler_threads.clear ();
    client_handling_end.clear ();
}

// Функиця обработки соединений (по аналогии с реализацией для Windows)
void tcpServer::handlingLoop() {
    while (_status == status::up) {
        int client_socket;
        struct sockaddr_in client_addr;
        int addrlen = sizeof (struct sockaddr_in);
        if((client_socket = accept(serv_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) >= 0 && _status == status::up)
            client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] {
                handler(Client(client_socket, client_addr));
                client_handling_end.push_back (std::this_thread::get_id());
            }));

        if(!client_handling_end.empty())
            for(std::list<std::thread::id>::iterator id_it = client_handling_end.begin (); !client_handling_end.empty() ; id_it = client_handling_end.begin())
                for(std::list<std::thread>::iterator thr_it = client_handler_threads.begin (); thr_it != client_handler_threads.end () ; ++thr_it)
                    if(thr_it->get_id () == *id_it) {
                        thr_it->join();
                        client_handler_threads.erase(thr_it);
                        client_handling_end.erase (id_it);
                        break;
                    }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Конструктор клиента по сокету и адресу
tcpServer::Client::Client(int socket, struct sockaddr_in address) : socket(socket), address(address) {}
// Конструктор копирования
tcpServer::Client::Client(const tcpServer::Client& other) : socket(other.socket), address(other.address) {}

tcpServer::Client::~Client() {
    shutdown(socket, 0); //Обрыв соединения сокета
    close(socket); //Закрытие сокета
}

// Геттеры хоста и порта
uint32_t tcpServer::Client::getHost() {return address.sin_addr.s_addr;}
uint16_t tcpServer::Client::getPort() {return address.sin_port;}

#endif