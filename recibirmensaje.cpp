#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void recibir_mensaje(int puerto) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);  // UDP para recibir mensajes
    if (sock < 0) {
        std::cerr << "Error al crear el socket de recepción.\n";
        return;
    }

    sockaddr_in direccion_cliente;
    direccion_cliente.sin_family = AF_INET;
    direccion_cliente.sin_addr.s_addr = INADDR_ANY;
    direccion_cliente.sin_port = htons(puerto);

    if (bind(sock, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente)) < 0) {
        std::cerr << "Error al vincular el socket de recepción al puerto " << puerto << ".\n";
        close(sock);
        return;
    }

    char buffer[1024];
    while (true) {
        ssize_t bytes_recibidos = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
        if (bytes_recibidos < 0) {
            std::cerr << "Error al recibir mensaje.\n";
            break;
        }

        buffer[bytes_recibidos] = '\0'; 
        std::cout << "Mensaje recibido: " << buffer << std::endl;
    }

    close(sock);
}
