#include <winsock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib") // Enlaza la biblioteca Winsock2

std::vector<SOCKET> clients;
std::mutex clients_mutex;

void handle_client(SOCKET clientSocket) {
    char buffer[1024];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Un cliente se ha desconectado." << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0'; // Añadir terminador nulo al mensaje

        // Enviar mensaje a todos los clientes
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (SOCKET client : clients) {
            if (client != clientSocket) {
                send(client, buffer, bytesReceived, 0);
            }
        }
    }

    // Eliminar cliente de la lista y cerrar el socket
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    closesocket(clientSocket);
}

int main() {
    // Inicialización de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock." << std::endl;
        return 1;
    }

    // Crear socket del servidor
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket del servidor." << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurar dirección del servidor
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Puerto 12345
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Asociar el socket a la dirección y puerto
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al asociar el socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar conexiones." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor en funcionamiento en el puerto 12345." << std::endl;

    // Aceptar conexiones de clientes
    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error al aceptar la conexión." << std::endl;
            continue;
        }

        std::cout << "Nuevo cliente conectado." << std::endl;

        // Agregar cliente a la lista
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(clientSocket);

        // Manejar cliente en un hilo separado
        std::thread(handle_client, clientSocket).detach();
    }

    // Cerrar el socket del servidor y limpiar Winsock
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}