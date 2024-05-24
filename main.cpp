#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>

// ���� �ࢥ� � ����
#define SERVER_ADDRESS "127.0.0.1"
#define PORT "8080"

std::atomic<bool> running(true);
SOCKET clientSocket;

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_CLOSE_EVENT || signal == CTRL_BREAK_EVENT) {
        if (send(clientSocket, "exit", 4, 0) == SOCKET_ERROR) {
            std::cerr << "�訡�� ��ࠢ�� ᮮ�饭�� 'exit' �� �ࢥ�\n";
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
            recvbuf[iResult] = '\0'; // ������塞 �������騩 ᨬ��� ��ப�
            //std::cout << "����祭 �⢥� �� �ࢥ�: " << recvbuf << std::endl;
            std::cout << recvbuf << std::endl;
        } else if (iResult == 0) {
            std::cerr << "���������� � �ࢥ஬ ������\n";
            break;
        } else {
            std::cerr << "�訡�� �� ����祭�� �⢥� �� �ࢥ�\n";
            break;
        }
    }
}

int main() {
    // ��⠭���� ��ࠡ��稪� �����襭�� ࠡ���
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "�訡�� ��⠭���� ��ࠡ��稪� �����襭�� ࠡ���\n";
        return 1;
    }

    // ���樠������ Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "�訡�� ���樠����樨 Winsock\n";
        return 1;
    }

    // �������� ᮪�� ������
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    std::cout << "������祭� � �ࢥ��.\n";

    // ����� ��⮪� ��� �ਥ�� ᮮ�饭�� �� �ࢥ�
    std::thread receiveThread(receiveMessages);
    std::string message;
    while (true) {
        message.clear();
        std::getline(std::cin, message);
        // ��ࠢ�� ᮮ�饭�� �� �ࢥ�
        iResult = send(clientSocket, message.c_str(), message.size(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "�訡�� ��ࠢ�� ᮮ�饭�� �� �ࢥ�\n";
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        if (message=="exit"){
            running=false;
            break;
        }
    }
    // �����⨥ ᮪�� ������ � �����襭�� ࠡ��� Winsock
    closesocket(clientSocket);
    WSACleanup();
    receiveThread.join(); // ��������� �����襭�� ��⮪� �ਥ�� ᮮ�饭��
    return 0;
}
