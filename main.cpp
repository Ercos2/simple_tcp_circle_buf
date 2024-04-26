#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <deque>

const int MAX_BUFFER_SIZE = 10;  // Максимальный размер циклического буфера

std::deque<std::string> circular_buffer;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *ack = "Message received";

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Привязка сокета к порту 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(18080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while(true) {
        std::cout << "Waiting for a connection..." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // Чтение сообщения от клиента
        read(new_socket, buffer, 1024);
        std::string message(buffer);
        std::cout << "Received message: " << message << std::endl;

        // Добавление сообщения в циклический буфер
        if (circular_buffer.size() == MAX_BUFFER_SIZE) {
            circular_buffer.pop_front();
        }
        circular_buffer.push_back(message);

        // Отправка подтверждения о получении
        send(new_socket, ack, strlen(ack), 0);
        std::cout << "Acknowledgement sent" << std::endl;

        close(new_socket);
    }

    return 0;
}