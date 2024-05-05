#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Адрес сервера и порт
#define SERVER_ADDRESS "127.0.0.1"
#define PORT "8080"

int main() {
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock\n";
        return 1;
    }

    // Создание сокета клиента
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета\n";
        WSACleanup();
        return 1;
    }

    // Получение информации о сервере
    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(SERVER_ADDRESS, PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "Ошибка получения информации о сервере: " << iResult << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    iResult = connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Ошибка подключения к серверу\n";
        freeaddrinfo(result);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    std::cout << "Подключено к серверу. Можете отправлять сообщения.\n";

    while (true) {
        std::string message;
        std::getline(std::cin, message);

        // Отправка сообщения на сервер
        iResult = send(clientSocket, message.c_str(), message.size(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Ошибка отправки сообщения на сервер\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }

    // Закрытие сокета клиента и завершение работы Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
