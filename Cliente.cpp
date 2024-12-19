#include <winsock2.h>
#include <ws2tcpip.h> // Necesario para inet_pton()
#include <iostream>
#include <thread>
#include <string>
#include <iomanip> // Para alinear texto

#pragma comment(lib, "Ws2_32.lib") // Vincular la librería Winsock

void receive_messages(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0'; // Null-terminate the string

        // Mostrar mensajes recibidos alineados a la izquierda
        std::cout << "\r" << std::left << std::setw(50) << buffer << std::endl;
        std::cout << "> "; // Volver al prompt para el usuario
        std::cout.flush();
    }
}

void send_messages(SOCKET clientSocket) {
    std::string message;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);

        if (message == "/salir") {
            closesocket(clientSocket);
            break;
        }

        // Enviar el mensaje al servidor
        send(clientSocket, message.c_str(), message.length(), 0);

        // Mostrar mensaje enviado alineado a la derecha
        std::cout << std::right << std::setw(50) << message << std::endl;
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);

    // Reemplazo de inet_addr() por inet_pton()
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1) {
        std::cerr << "Error: Dirección IP inválida." << std::endl;
        return 1;
    }

    // Conexión al servidor
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: No se pudo conectar al servidor." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Conectado al servidor." << std::endl;

    // Lanzar hilo para recibir mensajes
    std::thread receiveThread(receive_messages, clientSocket);
    receiveThread.detach();

    // Enviar mensajes desde el cliente
    send_messages(clientSocket);

    WSACleanup();
    return 0;
}
