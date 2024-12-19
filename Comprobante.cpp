#include <winsock2.h>
#include <iostream>
#include <windows.h>


int main() {
    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Inicia Winsock 2.2

    if (wsaStartupResult != 0) {
        std::cerr << "Error al iniciar Winsock. Código de error: " << wsaStartupResult << std::endl;
        return 1;  // Finaliza con error
    }

    std::cout << "Sockets configurats correctament" << std::endl;

    // Limpiar y cerrar Winsock
    WSACleanup();

    return 0;
}