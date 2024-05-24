#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>

// Адрес сервера и порт
#define SERVER_ADDRESS "127.0.0.1"
#define PORT "8080"

std::atomic<bool> running(true);
SOCKET clientSocket;

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_CLOSE_EVENT || signal == CTRL_BREAK_EVENT) {
        if (send(clientSocket, "exit", 4, 0) == SOCKET_ERROR) {
            std::cerr << "Ошибка отправки сообщения 'exit' на сервер\n";
        }
        running = false;
        closesocket(clientSocket);
        WSACleanup();
        return TRUE;
    }
    return FALSE;
}

void receiveMessages() {
    char recvbuf[1024];
    int iResult;

    while (true) {
        iResult = recv(clientSocket, recvbuf, 1024, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0'; // Добавляем завершающий символ строки
            //std::cout << "Получен ответ от сервера: " << recvbuf << std::endl;
            std::cout << recvbuf << std::endl;
        } else if (iResult == 0) {
            std::cerr << "Соединение с сервером закрыто\n";
            break;
        } else {
            std::cerr << "Ошибка при получении ответа от сервера\n";
            break;
        }
    }
}

int main() {
    // Установка обработчика завершения работы
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "Ошибка установки обработчика завершения работы\n";
        return 1;
    }

    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock\n";
        return 1;
    }

    // Создание сокета клиента
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    std::cout << "Подключено к серверу.\n";

    // Запуск потока для приема сообщений от сервера
    std::thread receiveThread(receiveMessages);
    std::string message;
    while (true) {
        message.clear();
        std::getline(std::cin, message);
        // Отправка сообщения на сервер
        iResult = send(clientSocket, message.c_str(), message.size(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Ошибка отправки сообщения на сервер\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        if (message=="exit"){
            running=false;
            break;
        }
    }
    // Закрытие сокета клиента и завершение работы Winsock
    closesocket(clientSocket);
    WSACleanup();
    receiveThread.join(); // Дождаться завершения потока приема сообщений
    return 0;
}
