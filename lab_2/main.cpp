#include "tcpServer.h"
#include "tcpClient.h"
#include "tcpServer.cpp"
#include "tcpClient.cpp"
#include <iostream>

//Парсер ip в std::string
std::string getHostStr(const tcpServer::Client& client) {
    uint32_t ip = client.getHost ();
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
           std::to_string( client.getPort ());
}

int main() {
    //Создание объекта TcpServer с передачей аргументами порта и лябда-фунции для обработк клиента
    tcpServer server( 8080,
                      [](tcpServer::Client client){
                          //Вывод адреса подключившего клиента в консоль
                          std::cout<<"Connected host:"<<getHostStr(client)<<std::endl;

                          //Ожидание данных от клиента
                          int size = 0;
                          while (!(size = client.loadData ()));
                          //Вывод размера данных и самих данных в консоль
                          std::cout
                                  <<"size: "<<size<<" bytes"<<std::endl
                                  << client.getData() << std::endl;
                          //Отправка ответа клиенту
                          const char answer[] = "Hello World from Server";
                          client.sendData(answer, sizeof (answer));
                      }
    );

    //Запуск серевера
    if(server.start() == tcpServer::status::up) {
        //Если сервер запущен вывести сообщение и войти в поток ожиданий клиентов
        std::cout<<"Server is up!"<<std::endl;
        server.joinLoop();
    } else {
        //Если сервер не запущен вывод кода ошибки и заверешение программы
        std::cout<<"Server start error! Error code:"<< int(server.getStatus()) <<std::endl;
        return -1;
    }
}

