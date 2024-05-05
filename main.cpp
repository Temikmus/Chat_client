#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// ���� �ࢥ� � ����
#define SERVER_ADDRESS "127.0.0.1"
#define PORT "8080"

int main() {
    // ���樠������ Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "�訡�� ���樠����樨 Winsock\n";
        return 1;
    }

    // �������� ᮪�� ������
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "�訡�� ᮧ����� ᮪��\n";
        WSACleanup();
        return 1;
    }

    // ����祭�� ���ଠ樨 � �ࢥ�
    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(SERVER_ADDRESS, PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "�訡�� ����祭�� ���ଠ樨 � �ࢥ�: " << iResult << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // ������祭�� � �ࢥ��
    iResult = connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "�訡�� ������祭�� � �ࢥ��\n";
        freeaddrinfo(result);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    std::cout << "������祭� � �ࢥ��. ����� ��ࠢ���� ᮮ�饭��.\n";

    while (true) {
        std::string message;
        std::getline(std::cin, message);

        // ��ࠢ�� ᮮ�饭�� �� �ࢥ�
        iResult = send(clientSocket, message.c_str(), message.size(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "�訡�� ��ࠢ�� ᮮ�饭�� �� �ࢥ�\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }

    // �����⨥ ᮪�� ������ � �����襭�� ࠡ��� Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
